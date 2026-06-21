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

[Flags]
public enum FileAttribute : uint
{
    FILE_ATTRIBUTE_VIRTUAL = 0x00010000
}

public enum IoReparseTag : uint
{
    /// <summary>Reserved reparse tag value.</summary>
    IO_REPARSE_TAG_RESERVED_ZERO = 0x00000000,

    /// <summary>Reserved reparse tag value.</summary>
    IO_REPARSE_TAG_RESERVED_ONE = 0x00000001,

    /// <summary>Reserved reparse tag value.</summary>
    IO_REPARSE_TAG_RESERVED_TWO = 0x00000002,

    /// <summary>Used for mount point support.</summary>
    IO_REPARSE_TAG_MOUNT_POINT = 0xA0000003,

    /// <summary>Obsolete. Used by legacy Hierarchical Storage Management products.</summary>
    IO_REPARSE_TAG_HSM = 0xC0000004,

    /// <summary>Home server drive extender.</summary>
    IO_REPARSE_TAG_DRIVE_EXTENDER = 0x80000005,

    /// <summary>Obsolete. Used by legacy Hierarchical Storage Management products.</summary>
    IO_REPARSE_TAG_HSM2 = 0x80000006,

    /// <summary>Used by the Single Instance Storage filter driver.</summary>
    IO_REPARSE_TAG_SIS = 0x80000007,

    /// <summary>Used by the WIM Mount filter.</summary>
    IO_REPARSE_TAG_WIM = 0x80000008,

    /// <summary>Obsolete. Used by Clustered Shared Volumes version 1 in Windows Server 2008 R2.</summary>
    IO_REPARSE_TAG_CSV = 0x80000009,

    /// <summary>Used by the Distributed File System filter.</summary>
    IO_REPARSE_TAG_DFS = 0x8000000A,

    /// <summary>Used by the Filter Manager test harness.</summary>
    IO_REPARSE_TAG_FILTER_MANAGER = 0x8000000B,

    /// <summary>Used for symbolic link support.</summary>
    IO_REPARSE_TAG_SYMLINK = 0xA000000C,

    /// <summary>Used by Microsoft Internet Information Services caching.</summary>
    IO_REPARSE_TAG_IIS_CACHE = 0xA0000010,

    /// <summary>Used by the Distributed File System Replication filter.</summary>
    IO_REPARSE_TAG_DFSR = 0x80000012,

    /// <summary>Used by the Data Deduplication filter.</summary>
    IO_REPARSE_TAG_DEDUP = 0x80000013,

    /// <summary>Not used.</summary>
    IO_REPARSE_TAG_APPXSTRM = 0xC0000014,

    /// <summary>Used by the Network File System component.</summary>
    IO_REPARSE_TAG_NFS = 0x80000014,

    /// <summary>Obsolete. Used by Windows Shell for legacy placeholder files in Windows 8.1.</summary>
    IO_REPARSE_TAG_FILE_PLACEHOLDER = 0x80000015,

    /// <summary>Used by the Dynamic File filter.</summary>
    IO_REPARSE_TAG_DFM = 0x80000016,

    /// <summary>Used by the Windows Overlay filter for WIMBoot or single-file compression.</summary>
    IO_REPARSE_TAG_WOF = 0x80000017,

    /// <summary>Used by the Windows Container Isolation filter.</summary>
    IO_REPARSE_TAG_WCI = 0x80000018,

    /// <summary>Used by the Windows Container Isolation filter.</summary>
    IO_REPARSE_TAG_WCI_1 = 0x90001018,

    /// <summary>Used by NPFS to indicate a named pipe symbolic link from a server silo into the host silo.</summary>
    IO_REPARSE_TAG_GLOBAL_REPARSE = 0xA0000019,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD = 0x9000001A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_1 = 0x9000101A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_2 = 0x9000201A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_3 = 0x9000301A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_4 = 0x9000401A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_5 = 0x9000501A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_6 = 0x9000601A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_7 = 0x9000701A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_8 = 0x9000801A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_9 = 0x9000901A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_A = 0x9000A01A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_B = 0x9000B01A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_C = 0x9000C01A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_D = 0x9000D01A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_E = 0x9000E01A,

    /// <summary>Used by the Cloud Files filter for files managed by a sync engine such as Microsoft OneDrive.</summary>
    IO_REPARSE_TAG_CLOUD_F = 0x9000F01A,

    /// <summary>Used by UWP packages to encode information allowing the application to be launched by CreateProcess.</summary>
    IO_REPARSE_TAG_APPEXECLINK = 0x8000001B,

    /// <summary>Used by Windows Projected File System for files managed by a user-mode provider such as VFS for Git.</summary>
    IO_REPARSE_TAG_PROJFS = 0x9000001C,

    /// <summary>Used by Windows Subsystem for Linux to represent a UNIX symbolic link.</summary>
    IO_REPARSE_TAG_LX_SYMLINK = 0xA000001D,

    /// <summary>Used by the Azure File Sync filter.</summary>
    IO_REPARSE_TAG_STORAGE_SYNC = 0x8000001E,

    /// <summary>Used by the Windows Container Isolation filter.</summary>
    IO_REPARSE_TAG_WCI_TOMBSTONE = 0xA000001F,

    /// <summary>Used by the Windows Container Isolation filter.</summary>
    IO_REPARSE_TAG_UNHANDLED = 0x80000020,

    /// <summary>Not used.</summary>
    IO_REPARSE_TAG_ONEDRIVE = 0x80000021,

    /// <summary>Used by Windows Projected File System for tombstone files.</summary>
    IO_REPARSE_TAG_PROJFS_TOMBSTONE = 0xA0000022,

    /// <summary>Used to represent a UNIX domain socket.</summary>
    IO_REPARSE_TAG_AF_UNIX = 0x80000023,

    /// <summary>Used by Windows Subsystem for Linux to represent a UNIX FIFO.</summary>
    IO_REPARSE_TAG_LX_FIFO = 0x80000024,

    /// <summary>Used by Windows Subsystem for Linux to represent a UNIX character special file.</summary>
    IO_REPARSE_TAG_LX_CHR = 0x80000025,

    /// <summary>Used by Windows Subsystem for Linux to represent a UNIX block special file.</summary>
    IO_REPARSE_TAG_LX_BLK = 0x80000026,

    /// <summary>Used by Azure File Sync for folders.</summary>
    IO_REPARSE_TAG_STORAGE_SYNC_FOLDER = 0x90000027,

    /// <summary>Used by the Windows Container Isolation filter.</summary>
    IO_REPARSE_TAG_WCI_LINK = 0xA0000027,

    /// <summary>Used by the Windows Container Isolation filter.</summary>
    IO_REPARSE_TAG_WCI_LINK_1 = 0xA0001027
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