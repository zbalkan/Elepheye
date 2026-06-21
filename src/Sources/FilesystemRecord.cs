using System.Buffers;
using System.Runtime.InteropServices;
using System.Security.AccessControl;
using System.Security.Cryptography;
using Elepheye.Core;
using Elepheye.Native;
using Microsoft.IO;

namespace Elepheye.Sources;

internal enum FilesystemOption
{ IgnoreChildren = 0, EnableLastAccessTime = 1 }

internal sealed class FilesystemRecord : IRecord
{
    private static readonly RecyclableMemoryStreamManager _msm = new();
    private static readonly ArrayPool<byte> _pool = ArrayPool<byte>.Shared;
    private const int ReadBufferSize = 0x4000;
    private readonly FileSystemInfo? _info;
    private readonly bool[] _options = new bool[2];

    private readonly string?[] _fields = new string?[14];

    public static readonly string[] FieldNames =
    [
        "path", "attributes", "creation_time", "last_access_time",
        "last_write_time", "size", "reparse_tag", "reparse_path",
        "owner", "group", "dacl", "sacl", "md5", "sha1"
    ];

    public static readonly string[] OptionNames =
        ["ignore_children", "enable_last_access_time"];

    internal FilesystemRecord(string path, FileSystemInfo? info = null, IoReparseTag reparseTag = 0, bool isStream = false)
    {
        Path = path;
        _info = info;
        ReparseTag = reparseTag;
        IsStream = isStream;
    }

    public string Path { get; }
    public bool IsDirectory => _info is DirectoryInfo;
    public bool IsStream { get; }
    public bool IsReparsePoint => (_info?.Attributes & FileAttributes.ReparsePoint) != 0;
    public IoReparseTag ReparseTag { get; }

    public string GetField(int index)
    {
        if (index == 0)
        {
            return Path;
        }

        return _fields[index] ??= ComputeField(index);
    }

    public bool GetOption(int index) => _options[index];

    public void SetOption(int index) => _options[index] = true;

    private string ComputeField(int index)
    {
        if (_info is null)
        {
            return string.Empty;
        }

        try
        {
            return index switch {
                1 => FieldFormatter.FormatFileAttributes(_info.Attributes),
                2 => IsStream ? string.Empty : FieldFormatter.FormatDateTime(_info.CreationTimeUtc),
                3 => (!IsStream && _options[(int)FilesystemOption.EnableLastAccessTime])
                    ? FieldFormatter.FormatDateTime(_info.LastAccessTimeUtc) : string.Empty,
                4 => IsStream ? string.Empty : FieldFormatter.FormatDateTime(_info.LastWriteTimeUtc),
                5 => IsDirectory ? string.Empty : FormatSize(),
                6 => (!IsStream && IsReparsePoint) ? FieldFormatter.FormatReparseTag(ReparseTag) : string.Empty,
                7 => (!IsStream && IsReparsePoint) ? FormatReparsePath() : string.Empty,
                8 => IsStream ? string.Empty : FormatSddl(AccessControlSections.Owner),
                9 => IsStream ? string.Empty : FormatSddl(AccessControlSections.Group),
                10 => IsStream ? string.Empty : FormatSddl(AccessControlSections.Access),
                11 => IsStream ? string.Empty : FormatSddl(AccessControlSections.Audit),
                12 => IsDirectory ? string.Empty : ComputeHashMd5(),
                13 => IsDirectory ? string.Empty : ComputeHashSha1(),
                _ => string.Empty
            };
        }
        catch (Exception e) when (e is UnauthorizedAccessException or IOException)
        {
            ExceptionSink.Log(e, $"reading field {FieldNames[index]} for {Path}",
                ExceptionLevel.Warning);
            return string.Empty;
        }
        catch (Exception e)
        {
            ExceptionSink.Log(e, $"reading field {FieldNames[index]} for {Path}");
            return string.Empty;
        }
    }

    private string FormatSize()
    {
        if (_info is FileInfo fi)
        {
            return fi.Length.ToString();
        }

        return string.Empty;
    }

    private string FormatReparsePath()
    {
        var tag = ReparseTag;
        if (tag != IoReparseTag.IO_REPARSE_TAG_MOUNT_POINT &&
            tag != IoReparseTag.IO_REPARSE_TAG_SYMLINK)
        {
            return string.Empty;
        }

        var handle = NativeMethods.CreateFileW(
            Path,
            FileDesiredAccess.GENERIC_READ,
            FileFlagAttributes.FILE_SHARE_READ,
            nint.Zero,
            FileFlagAttributes.OPEN_EXISTING,
            FileFlagAttributes.FILE_FLAG_BACKUP_SEMANTICS | FileFlagAttributes.FILE_FLAG_OPEN_REPARSE_POINT,
            nint.Zero);

        if (handle == -1)
        {
            return string.Empty;
        }

        try
        {
            var buffer = _pool.Rent(16384);
            var pin = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            try
            {
                var ptr = pin.AddrOfPinnedObject();
                if (!NativeMethods.DeviceIoControl(
                        handle,
                        FileFlagAttributes.FSCTL_GET_REPARSE_POINT,
                        nint.Zero, 0,
                        ptr, (uint)buffer.Length,
                        out var returned, nint.Zero))
                {
                    return string.Empty;
                }

                // Parse reparse buffer: header is 8 bytes (tag=4, size=2, reserved=2)
                if (tag == IoReparseTag.IO_REPARSE_TAG_MOUNT_POINT)
                {
                    if (returned < 20)
                    {
                        return string.Empty;
                    }

                    const int offset = 8; // skip header
                    var substOffset = System.Buffers.Binary.BinaryPrimitives.ReadUInt16LittleEndian(buffer.AsSpan(offset));
                    var substLen = System.Buffers.Binary.BinaryPrimitives.ReadUInt16LittleEndian(buffer.AsSpan(offset + 2));
                    var pathStart = offset + 8 + substOffset;
                    if (pathStart + substLen > returned)
                    {
                        return string.Empty;
                    }

                    return System.Text.Encoding.Unicode.GetString(buffer, pathStart, substLen);
                }
                else // SYMLINK
                {
                    if (returned < 24)
                    {
                        return string.Empty;
                    }

                    const int offset = 8; // skip header
                    var substOffset = System.Buffers.Binary.BinaryPrimitives.ReadUInt16LittleEndian(buffer.AsSpan(offset));
                    var substLen = System.Buffers.Binary.BinaryPrimitives.ReadUInt16LittleEndian(buffer.AsSpan(offset + 2));
                    var pathStart = offset + 12 + substOffset; // +4 for Flags field
                    if (pathStart + substLen > returned)
                    {
                        return string.Empty;
                    }

                    return System.Text.Encoding.Unicode.GetString(buffer, pathStart, substLen);
                }
            }
            finally
            {
                pin.Free();
                _pool.Return(buffer);
            }
        }
        finally
        {
            NativeMethods.CloseHandle(handle);
        }
    }

    private string FormatSddl(AccessControlSections section)
    {
        try
        {
            if (_info is FileInfo fi)
            {
                var security = fi.GetAccessControl(section);
                return security.GetSecurityDescriptorSddlForm(section) ?? string.Empty;
            }
            if (_info is DirectoryInfo di)
            {
                var security = di.GetAccessControl(section);
                return security.GetSecurityDescriptorSddlForm(section) ?? string.Empty;
            }
            return string.Empty;
        }
        catch (Exception e) when (e is UnauthorizedAccessException or InvalidOperationException)
        {
            throw new UnauthorizedAccessException($"SDDL for {Path}", e);
        }
    }

    private string ComputeHashMd5() => ComputeHashCore(usemd5: true);

    private string ComputeHashSha1() => ComputeHashCore(usemd5: false);

    private string ComputeHashCore(bool usemd5)
    {
        var handle = NativeMethods.CreateFileW(
            Path,
            FileDesiredAccess.GENERIC_READ,
            FileFlagAttributes.FILE_SHARE_READ,
            nint.Zero,
            FileFlagAttributes.OPEN_EXISTING,
            FileFlagAttributes.FILE_FLAG_BACKUP_SEMANTICS |
            FileFlagAttributes.FILE_FLAG_OPEN_REPARSE_POINT |
            FileFlagAttributes.FILE_FLAG_SEQUENTIAL_SCAN,
            nint.Zero);

        if (handle == -1)
        {
            return string.Empty;
        }

        try
        {
            using var sfh = new Microsoft.Win32.SafeHandles.SafeFileHandle(handle, ownsHandle: false);
            using var fs = new FileStream(sfh, FileAccess.Read, ReadBufferSize, isAsync: false);
            using var stream = _msm.GetStream();
            var buf = _pool.Rent(ReadBufferSize);
            try
            {
                int read;
                while ((read = fs.Read(buf, 0, buf.Length)) > 0)
                {
                    stream.Write(buf, 0, read);
                }
            }
            finally
            {
                _pool.Return(buf);
            }

            stream.Position = 0;
            var hash = usemd5
                ? MD5.HashData(stream)
                : SHA1.HashData(stream);
            return FieldFormatter.FormatHashBytes(hash);
        }
        finally
        {
            NativeMethods.CloseHandle(handle);
        }
    }
}