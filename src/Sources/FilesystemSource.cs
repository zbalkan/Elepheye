using System.Buffers;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Elepheye.Core;
using Elepheye.Native;
using Elepheye.Security;

namespace Elepheye.Sources;

public sealed class FilesystemSource(IReadOnlyList<string> paths)
{
    public RecordName Name { get; } = BuildName();

    private static RecordName BuildName()
    {
        var n = new RecordName();
        n.FieldNames.AddRange(FilesystemRecord.FieldNames);
        n.OptionNames.AddRange(FilesystemRecord.OptionNames);
        return n;
    }

    public async IAsyncEnumerable<IRecord> ReadAsync(
        [EnumeratorCancellation] CancellationToken ct = default)
    {
        using var priv = new PrivilegeManager();
        try { priv.EnablePrivilege("SeBackupPrivilege"); } catch { /* best effort */ }
        try { priv.EnablePrivilege("SeSecurityPrivilege"); } catch { /* best effort */ }

        var comparer = Comparer<FilesystemRecord>.Create(
            (a, b) => FieldFormatter.CompareKey(a.Path, b.Path));

        var queue = new SortedSet<FilesystemRecord>(comparer);

        foreach (var p in paths)
        {
            var rec = BuildRecord(p);
            if (rec is not null) queue.Add(rec);
        }

        while (queue.Count > 0)
        {
            ct.ThrowIfCancellationRequested();
            var current = queue.Min!;
            queue.Remove(current);

            yield return current;

            if (!current.IsStream)
            {
                foreach (var stream in EnumerateStreams(current))
                {
                    if (stream is not null) queue.Add(stream);
                }

                if (current.IsDirectory && !current.IsReparsePoint &&
                    !current.GetOption((int)FilesystemOption.IgnoreChildren))
                {
                    foreach (var child in EnumerateChildren(current))
                    {
                        if (child is not null) queue.Add(child);
                    }
                }
            }

            await Task.Yield();
        }
    }

    private static FilesystemRecord? BuildRecord(string path)
    {
        try
        {
            FileSystemInfo info = Directory.Exists(path)
                ? new DirectoryInfo(path)
                : (FileSystemInfo)new FileInfo(path);

            uint reparseTag = 0;
            if ((info.Attributes & FileAttributes.ReparsePoint) != 0)
                reparseTag = GetReparseTag(path);

            return new FilesystemRecord(path, info, reparseTag);
        }
        catch (Exception e)
        {
            ExceptionSink.Log(e, $"reading {path}");
            return null;
        }
    }

    private static uint GetReparseTag(string path)
    {
        try
        {
            var di = new DirectoryInfo(path);
            if (di.Exists && di.LinkTarget is not null)
                return 0xA000000C; // SYMLINK as approximation; exact tag via DeviceIoControl
        }
        catch { /* ignore */ }

        // Read exact tag via DeviceIoControl
        nint handle = NativeMethods.CreateFileW(
            path,
            FileDesiredAccess.GENERIC_READ,
            FileFlagAttributes.FILE_SHARE_READ,
            nint.Zero,
            FileFlagAttributes.OPEN_EXISTING,
            FileFlagAttributes.FILE_FLAG_BACKUP_SEMANTICS | FileFlagAttributes.FILE_FLAG_OPEN_REPARSE_POINT,
            nint.Zero);

        if (handle == -1) return 0;

        try
        {
            byte[] buf = ArrayPool<byte>.Shared.Rent(16384); // need full buffer for DeviceIoControl
            var pin = GCHandle.Alloc(buf, GCHandleType.Pinned);
            try
            {
                nint ptr = pin.AddrOfPinnedObject();
                if (NativeMethods.DeviceIoControl(
                        handle,
                        FileFlagAttributes.FSCTL_GET_REPARSE_POINT,
                        nint.Zero, 0,
                        ptr, (uint)buf.Length,
                        out uint returned, nint.Zero) && returned >= 4)
                {
                    return System.Buffers.Binary.BinaryPrimitives.ReadUInt32LittleEndian(buf.AsSpan());
                }
                return 0;
            }
            finally
            {
                pin.Free();
                ArrayPool<byte>.Shared.Return(buf);
            }
        }
        finally
        {
            NativeMethods.CloseHandle(handle);
        }
    }

    private static IEnumerable<FilesystemRecord> EnumerateStreams(FilesystemRecord parent)
    {
        nint handle = NativeMethods.CreateFileW(
            parent.Path,
            FileDesiredAccess.GENERIC_READ,
            FileFlagAttributes.FILE_SHARE_READ,
            nint.Zero,
            FileFlagAttributes.OPEN_EXISTING,
            FileFlagAttributes.FILE_FLAG_BACKUP_SEMANTICS | FileFlagAttributes.FILE_FLAG_OPEN_REPARSE_POINT,
            nint.Zero);

        if (handle == -1) yield break;

        nint context = nint.Zero;
        int headerSize = Marshal.SizeOf<WIN32_STREAM_ID>();
        byte[] headerBuf = ArrayPool<byte>.Shared.Rent(headerSize);
        var headerPin = GCHandle.Alloc(headerBuf, GCHandleType.Pinned);

        try
        {
            while (true)
            {
                Array.Clear(headerBuf, 0, headerSize);
                nint headerPtr = headerPin.AddrOfPinnedObject();

                if (!NativeMethods.BackupRead(handle, headerPtr, (uint)headerSize,
                        out uint read, false, false, ref context))
                    break;
                if (read == 0) break;

                var streamId = Marshal.PtrToStructure<WIN32_STREAM_ID>(headerPtr);

                if (streamId.dwStreamNameSize > 0)
                {
                    int nameSize = (int)streamId.dwStreamNameSize;
                    byte[] nameBuf = ArrayPool<byte>.Shared.Rent(nameSize);
                    var namePin = GCHandle.Alloc(nameBuf, GCHandleType.Pinned);
                    try
                    {
                        nint namePtr = namePin.AddrOfPinnedObject();
                        if (!NativeMethods.BackupRead(handle, namePtr, (uint)nameSize,
                                out uint nameRead, false, false, ref context))
                            break;

                        if (streamId.dwStreamId == StreamId.BACKUP_ALTERNATE_DATA && nameRead == nameSize)
                        {
                            string streamName = System.Text.Encoding.Unicode.GetString(nameBuf, 0, nameSize);
                            string streamPath = parent.Path + streamName;

                            FilesystemRecord? streamRec = null;
                            try
                            {
                                var fi = new FileInfo(streamPath);
                                streamRec = new FilesystemRecord(streamPath, fi, 0, isStream: true);
                            }
                            catch (Exception e)
                            {
                                ExceptionSink.Log(e, $"reading stream {streamPath}");
                            }
                            if (streamRec is not null) yield return streamRec;
                        }
                    }
                    finally
                    {
                        namePin.Free();
                        ArrayPool<byte>.Shared.Return(nameBuf);
                    }
                }

                // Skip stream body
                long bodySize = streamId.Size.QuadPart;
                if (bodySize > 0)
                {
                    NativeMethods.BackupSeek(handle,
                        streamId.Size.LowPart, (uint)streamId.Size.HighPart,
                        out _, out _, ref context);
                }
            }
        }
        finally
        {
            headerPin.Free();
            // Abort to release context
            if (context != nint.Zero)
                NativeMethods.BackupRead(handle, nint.Zero, 0, out _, true, false, ref context);
            ArrayPool<byte>.Shared.Return(headerBuf);
            NativeMethods.CloseHandle(handle);
        }
    }

    private static IEnumerable<FilesystemRecord> EnumerateChildren(FilesystemRecord parent)
    {
        string dirPath = parent.Path;
        IEnumerable<string> entries;
        try
        {
            entries = Directory.EnumerateFileSystemEntries(dirPath);
        }
        catch (Exception e)
        {
            ExceptionSink.Log(e, $"enumerating {dirPath}",
                e is UnauthorizedAccessException ? ExceptionLevel.Warning : ExceptionLevel.Error);
            yield break;
        }

        foreach (var entry in entries)
        {
            var rec = BuildRecord(entry);
            if (rec is not null) yield return rec;
        }
    }
}
