using System.Buffers;
using System.Text;
using Microsoft.Win32;

namespace Elepheye.Core;

public static class FieldFormatter
{
    private static readonly SearchValues<char> _csvSpecialChars =
        SearchValues.Create(['"', ',', '\r', '\n']);

    public static string FormatDateTime(DateTime utc)
    {
        return utc.Kind == DateTimeKind.Utc
            ? $"{utc.Year:D4}-{utc.Month:D2}-{utc.Day:D2}T{utc.Hour:D2}:{utc.Minute:D2}:{utc.Second:D2}.{utc.Millisecond:D3}Z"
            : string.Empty;
    }

    public static string FormatFileAttributes(FileAttributes attrs)
    {
        var sb = new StringBuilder();
        sb.Append($"0x{(uint)attrs:X8}");
        if ((attrs & FileAttributes.Archive) != 0) sb.Append(":ARCHIVE");
        if ((attrs & FileAttributes.Compressed) != 0) sb.Append(":COMPRESSED");
        if ((attrs & FileAttributes.Device) != 0) sb.Append(":DEVICE");
        if ((attrs & FileAttributes.Directory) != 0) sb.Append(":DIRECTORY");
        if ((attrs & FileAttributes.Encrypted) != 0) sb.Append(":ENCRYPTED");
        if ((attrs & FileAttributes.Hidden) != 0) sb.Append(":HIDDEN");
        if ((attrs & FileAttributes.Normal) != 0) sb.Append(":NORMAL");
        if ((attrs & FileAttributes.NotContentIndexed) != 0) sb.Append(":NOT_CONTENT_INDEXED");
        if ((attrs & FileAttributes.Offline) != 0) sb.Append(":OFFLINE");
        if ((attrs & FileAttributes.ReadOnly) != 0) sb.Append(":READONLY");
        if ((attrs & FileAttributes.ReparsePoint) != 0) sb.Append(":REPARSE_POINT");
        if ((attrs & FileAttributes.SparseFile) != 0) sb.Append(":SPARSE_FILE");
        if ((attrs & FileAttributes.System) != 0) sb.Append(":SYSTEM");
        if ((attrs & FileAttributes.Temporary) != 0) sb.Append(":TEMPORARY");
        const uint FILE_ATTRIBUTE_VIRTUAL = 0x00010000;
        if (((uint)attrs & FILE_ATTRIBUTE_VIRTUAL) != 0) sb.Append(":VIRTUAL");
        return sb.ToString();
    }

    public static string FormatReparseTag(uint tag) => tag switch
    {
        0x8000000A => "DFS",
        0x80000012 => "DFSR",
        0x80000005 => "DRIVER_EXTENDER",
        0x8000000B => "FILTER_MANAGER",
        0x80000004 => "HSM",
        0x80000006 => "HSM2",
        0xA0000003 => "MOUNT_POINT",
        0x80000007 => "SIS",
        0xA000000C => "SYMLINK",
        _ => $"0x{tag:X8}"
    };

    public static string FormatHashBytes(byte[] bytes)
    {
        var sb = new StringBuilder(bytes.Length * 2);
        foreach (var b in bytes)
            sb.Append($"{b:x2}");
        return sb.ToString();
    }

    public static string FormatRegistryValueType(RegistryValueKind kind) => kind switch
    {
        RegistryValueKind.Binary => "BINARY",
        RegistryValueKind.DWord => "DWORD",
        RegistryValueKind.ExpandString => "EXPAND_SZ",
        RegistryValueKind.MultiString => "MULTI_SZ",
        RegistryValueKind.None => "NONE",
        RegistryValueKind.QWord => "QWORD",
        RegistryValueKind.String => "SZ",
        RegistryValueKind.Unknown => $"0x{(int)kind:X8}",
        _ => $"0x{(int)kind:X8}"
    };

    public static string FormatVariantValue(object? value)
    {
        if (value is null) return string.Empty;
        return value switch
        {
            string s => s,
            bool b => b ? "True" : "False",
            byte by => by.ToString(),
            sbyte sb => sb.ToString(),
            short s16 => s16.ToString(),
            ushort u16 => u16.ToString(),
            int i32 => i32.ToString(),
            uint u32 => u32.ToString(),
            long i64 => i64.ToString(),
            ulong u64 => u64.ToString(),
            float f => f.ToString("G"),
            double d => d.ToString("G"),
            DateTime dt => FormatDateTime(dt.Kind == DateTimeKind.Local ? dt.ToUniversalTime() : dt),
            Array arr => FormatVariantArray(arr),
            _ => value.ToString() ?? string.Empty
        };
    }

    public static string FormatCimValue(object? value, string cimTypeName)
    {
        if (value is null || value is DBNull) return string.Empty;
        if (value is Array arr)
        {
            var sb = new StringBuilder();
            int i = 0;
            foreach (var item in arr)
            {
                string formatted = FormatVariantValue(item);
                sb.Append($"[{i}:{formatted.Length}]{formatted}");
                i++;
            }
            return sb.ToString();
        }
        return FormatVariantValue(value);
    }

    private static string FormatVariantArray(Array arr)
    {
        var sb = new StringBuilder();
        for (int i = 0; i < arr.Length; i++)
        {
            var item = arr.GetValue(i);
            var formatted = FormatVariantValue(item);
            sb.Append($"[{i}:{formatted.Length}]{formatted}");
        }
        return sb.ToString();
    }

    public static string EscapeCsvField(string field)
    {
        if (field.AsSpan().IndexOfAny(_csvSpecialChars) < 0)
            return field;
        return $"\"{field.Replace("\"", "\"\"")}\"";
    }

    public static string ToUpperInvariant(string s) =>
        s.ToUpperInvariant();

    public static int CompareKey(string key1, string key2)
    {
        int caseSensitive = string.CompareOrdinal(key1, key2);
        if (caseSensitive == 0) return 0;
        int caseInsensitive = string.Compare(key1, key2, StringComparison.OrdinalIgnoreCase);
        return caseInsensitive != 0 ? caseInsensitive : caseSensitive;
    }
}
