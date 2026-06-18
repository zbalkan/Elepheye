using System.Runtime.InteropServices;

namespace Elepheye.Native;

[StructLayout(LayoutKind.Sequential)]
internal struct LARGE_INTEGER
{
    public uint LowPart;
    public int HighPart;

    public long QuadPart => ((long)HighPart << 32) | LowPart;
}

[StructLayout(LayoutKind.Sequential)]
internal struct WIN32_STREAM_ID
{
    public uint dwStreamId;
    public uint dwStreamAttributes;
    public LARGE_INTEGER Size;
    public uint dwStreamNameSize;
}

[StructLayout(LayoutKind.Sequential)]
internal struct TOKEN_PRIVILEGES
{
    public uint PrivilegeCount;
    public LUID_AND_ATTRIBUTES Privileges;
}

[StructLayout(LayoutKind.Sequential)]
internal struct LUID_AND_ATTRIBUTES
{
    public LUID Luid;
    public uint Attributes;
}

[StructLayout(LayoutKind.Sequential)]
internal struct LUID
{
    public uint LowPart;
    public int HighPart;
}

[StructLayout(LayoutKind.Sequential)]
internal struct REPARSE_DATA_BUFFER_HEADER
{
    public uint ReparseTag;
    public ushort ReparseDataLength;
    public ushort Reserved;
}

// Used for MOUNT_POINT and SYMLINK reparse points
[StructLayout(LayoutKind.Sequential)]
internal struct MOUNT_POINT_REPARSE_BUFFER
{
    public ushort SubstituteNameOffset;
    public ushort SubstituteNameLength;
    public ushort PrintNameOffset;
    public ushort PrintNameLength;
}

[StructLayout(LayoutKind.Sequential)]
internal struct SYMLINK_REPARSE_BUFFER
{
    public ushort SubstituteNameOffset;
    public ushort SubstituteNameLength;
    public ushort PrintNameOffset;
    public ushort PrintNameLength;
    public uint Flags;
}

internal static class StreamId
{
    public const uint BACKUP_DATA = 0x00000001;
    public const uint BACKUP_ALTERNATE_DATA = 0x00000004;
}

internal static class TokenPrivilegeAttributes
{
    public const uint SE_PRIVILEGE_ENABLED = 0x00000002;
}

internal static class ReparseTag
{
    public const uint IO_REPARSE_TAG_MOUNT_POINT = 0xA0000003;
    public const uint IO_REPARSE_TAG_SYMLINK = 0xA000000C;
}

internal static class FileDesiredAccess
{
    public const uint GENERIC_READ = 0x80000000u;
}

internal static class FileFlagAttributes
{
    public const uint FILE_FLAG_BACKUP_SEMANTICS = 0x02000000;
    public const uint FILE_FLAG_OPEN_REPARSE_POINT = 0x00200000;
    public const uint FILE_FLAG_SEQUENTIAL_SCAN = 0x08000000;
    public const uint FILE_SHARE_READ = 0x00000001;
    public const uint OPEN_EXISTING = 3;
    public const uint FSCTL_GET_REPARSE_POINT = 0x000900A8;
}
