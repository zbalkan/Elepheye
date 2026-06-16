using System.Buffers;
using System.Runtime.InteropServices;
using System.Security.AccessControl;
using System.Security.Cryptography;
using Elepheye.Core;
using Elepheye.Native;
using Microsoft.IO;

namespace Elepheye.Sources;

internal enum FilesystemOption { IgnoreChildren = 0, EnableLastAccessTime = 1 }

internal sealed class FilesystemRecord : IRecord
{
    private static readonly RecyclableMemoryStreamManager _msm = new();
    private static readonly ArrayPool<byte> _pool = ArrayPool<byte>.Shared;
    private const int ReadBufferSize = 0x4000;

    private readonly string _path;
    private readonly FileSystemInfo? _info;
    private readonly uint _reparseTag;
    private readonly bool _isStream;
    private readonly bool[] _options = new bool[2];

    private string?[] _fields = new string?[14];

    public static readonly string[] FieldNames =
    [
        "path", "attributes", "creation_time", "last_access_time",
        "last_write_time", "size", "reparse_tag", "reparse_path",
        "owner", "group", "dacl", "sacl", "md5", "sha1"
    ];
    public static readonly string[] OptionNames =
        ["ignore_children", "enable_last_access_time"];

    internal FilesystemRecord(string path, FileSystemInfo? info = null, uint reparseTag = 0, bool isStream = false)
    {
        _path = path;
        _info = info;
        _reparseTag = reparseTag;
        _isStream = isStream;
    }

    public string Path => _path;
    public bool IsDirectory => _info is DirectoryInfo;
    public bool IsStream => _isStream;
    public bool IsReparsePoint => (_info?.Attributes & FileAttributes.ReparsePoint) != 0;
    public uint ReparseTag => _reparseTag;

    public string GetField(int index)
    {
        if (index == 0) return _path;
        return _fields[index] ??= ComputeField(index);
    }

    public bool GetOption(int index) => _options[index];
    public void SetOption(int index) => _options[index] = true;

    private string ComputeField(int index)
    {
        if (_info is null) return string.Empty;
        try
        {
            return index switch
            {
                1 => FieldFormatter.FormatFileAttributes(_info.Attributes),
                2 => _isStream ? string.Empty : FieldFormatter.FormatDateTime(_info.CreationTimeUtc),
                3 => (!_isStream && _options[(int)FilesystemOption.EnableLastAccessTime])
                    ? FieldFormatter.FormatDateTime(_info.LastAccessTimeUtc) : string.Empty,
                4 => _isStream ? string.Empty : FieldFormatter.FormatDateTime(_info.LastWriteTimeUtc),
                5 => IsDirectory ? string.Empty : FormatSize(),
                6 => (!_isStream && IsReparsePoint) ? FieldFormatter.FormatReparseTag(_reparseTag) : string.Empty,
                7 => (!_isStream && IsReparsePoint) ? FormatReparsePath() : string.Empty,
                8 => _isStream ? string.Empty : FormatSddl(AccessControlSections.Owner),
                9 => _isStream ? string.Empty : FormatSddl(AccessControlSections.Group),
                10 => _isStream ? string.Empty : FormatSddl(AccessControlSections.Access),
                11 => _isStream ? string.Empty : FormatSddl(AccessControlSections.Audit),
                12 => IsDirectory ? string.Empty : ComputeHashMd5(),
                13 => IsDirectory ? string.Empty : ComputeHashSha1(),
                _ => string.Empty
            };
        }
        catch (Exception e) when (e is UnauthorizedAccessException or IOException)
        {
            ExceptionSink.Log(e, $"reading field {FieldNames[index]} for {_path}",
                ExceptionLevel.Warning);
            return string.Empty;
        }
        catch (Exception e)
        {
            ExceptionSink.Log(e, $"reading field {FieldNames[index]} for {_path}");
            return string.Empty;
        }
    }

    private string FormatSize()
    {
        if (_info is FileInfo fi) return fi.Length.ToString();
        return string.Empty;
    }

    private string FormatReparsePath()
    {
        uint tag = _reparseTag;
        if (tag != ReparseTag.IO_REPARSE_TAG_MOUNT_POINT &&
            tag != ReparseTag.IO_REPARSE_TAG_SYMLINK)
            return string.Empty;

        nint handle = NativeMethods.CreateFileW(
            _path,
            FileDesiredAccess.GENERIC_READ,
            FileFlagAttributes.FILE_SHARE_READ,
            nint.Zero,
            FileFlagAttributes.OPEN_EXISTING,
            FileFlagAttributes.FILE_FLAG_BACKUP_SEMANTICS | FileFlagAttributes.FILE_FLAG_OPEN_REPARSE_POINT,
            nint.Zero);

        if (handle == -1) return string.Empty;

        try
        {
            byte[] buffer = _pool.Rent(16384);
            var pin = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            try
            {
                nint ptr = pin.AddrOfPinnedObject();
                if (!NativeMethods.DeviceIoControl(
                        handle,
                        FileFlagAttributes.FSCTL_GET_REPARSE_POINT,
                        nint.Zero, 0,
                        ptr, (uint)buffer.Length,
                        out uint returned, nint.Zero))
                    return string.Empty;

                // Parse reparse buffer: header is 8 bytes (tag=4, size=2, reserved=2)
                if (tag == ReparseTag.IO_REPARSE_TAG_MOUNT_POINT)
                {
                    if (returned < 20) return string.Empty;
                    const int offset = 8; // skip header
                    ushort substOffset = System.Buffers.Binary.BinaryPrimitives.ReadUInt16LittleEndian(buffer.AsSpan(offset));
                    ushort substLen = System.Buffers.Binary.BinaryPrimitives.ReadUInt16LittleEndian(buffer.AsSpan(offset + 2));
                    int pathStart = offset + 8 + substOffset;
                    if (pathStart + substLen > returned) return string.Empty;
                    return System.Text.Encoding.Unicode.GetString(buffer, pathStart, substLen);
                }
                else // SYMLINK
                {
                    if (returned < 24) return string.Empty;
                    const int offset = 8; // skip header
                    ushort substOffset = System.Buffers.Binary.BinaryPrimitives.ReadUInt16LittleEndian(buffer.AsSpan(offset));
                    ushort substLen = System.Buffers.Binary.BinaryPrimitives.ReadUInt16LittleEndian(buffer.AsSpan(offset + 2));
                    int pathStart = offset + 12 + substOffset; // +4 for Flags field
                    if (pathStart + substLen > returned) return string.Empty;
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
            throw new UnauthorizedAccessException($"SDDL for {_path}", e);
        }
    }

    private string ComputeHashMd5() => ComputeHashCore(usemd5: true);
    private string ComputeHashSha1() => ComputeHashCore(usemd5: false);

    private string ComputeHashCore(bool usemd5)
    {
        nint handle = NativeMethods.CreateFileW(
            _path,
            FileDesiredAccess.GENERIC_READ,
            FileFlagAttributes.FILE_SHARE_READ,
            nint.Zero,
            FileFlagAttributes.OPEN_EXISTING,
            FileFlagAttributes.FILE_FLAG_BACKUP_SEMANTICS |
            FileFlagAttributes.FILE_FLAG_OPEN_REPARSE_POINT |
            FileFlagAttributes.FILE_FLAG_SEQUENTIAL_SCAN,
            nint.Zero);

        if (handle == -1) return string.Empty;

        try
        {
            using var sfh = new Microsoft.Win32.SafeHandles.SafeFileHandle(handle, ownsHandle: false);
            using var fs = new FileStream(sfh, FileAccess.Read, ReadBufferSize, isAsync: false);
            using var stream = _msm.GetStream();
            byte[] buf = _pool.Rent(ReadBufferSize);
            try
            {
                int read;
                while ((read = fs.Read(buf, 0, buf.Length)) > 0)
                    stream.Write(buf, 0, read);
            }
            finally
            {
                _pool.Return(buf);
            }

            stream.Position = 0;
            byte[] hash = usemd5
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
