using System.Buffers;
using System.Text;
using Elepheye.Native;
using Microsoft.Management.Infrastructure;
using Microsoft.Win32;

namespace Elepheye.Core;

public static class FieldFormatter
{
    private static readonly SearchValues<char> _csvSpecialChars =
        SearchValues.Create(['"', ',', '\r', '\n']);

    public static string FormatDateTime(DateTime utc) => utc.Kind == DateTimeKind.Utc
            ? $"{utc.Year:D4}-{utc.Month:D2}-{utc.Day:D2}T{utc.Hour:D2}:{utc.Minute:D2}:{utc.Second:D2}.{utc.Millisecond:D3}Z"
            : string.Empty;

    public static string FormatFileAttributes(FileAttributes attrs)
    {
        var sb = new StringBuilder();
        sb.Append($"0x{(uint)attrs:X8}");
        if ((attrs & FileAttributes.Archive) != 0)
        {
            sb.Append(":ARCHIVE");
        }

        if ((attrs & FileAttributes.Compressed) != 0)
        {
            sb.Append(":COMPRESSED");
        }

        if ((attrs & FileAttributes.Device) != 0)
        {
            sb.Append(":DEVICE");
        }

        if ((attrs & FileAttributes.Directory) != 0)
        {
            sb.Append(":DIRECTORY");
        }

        if ((attrs & FileAttributes.Encrypted) != 0)
        {
            sb.Append(":ENCRYPTED");
        }

        if ((attrs & FileAttributes.Hidden) != 0)
        {
            sb.Append(":HIDDEN");
        }

        if ((attrs & FileAttributes.Normal) != 0)
        {
            sb.Append(":NORMAL");
        }

        if ((attrs & FileAttributes.NotContentIndexed) != 0)
        {
            sb.Append(":NOT_CONTENT_INDEXED");
        }

        if ((attrs & FileAttributes.Offline) != 0)
        {
            sb.Append(":OFFLINE");
        }

        if ((attrs & FileAttributes.ReadOnly) != 0)
        {
            sb.Append(":READONLY");
        }

        if ((attrs & FileAttributes.ReparsePoint) != 0)
        {
            sb.Append(":REPARSE_POINT");
        }

        if ((attrs & FileAttributes.SparseFile) != 0)
        {
            sb.Append(":SPARSE_FILE");
        }

        if ((attrs & FileAttributes.System) != 0)
        {
            sb.Append(":SYSTEM");
        }

        if ((attrs & FileAttributes.Temporary) != 0)
        {
            sb.Append(":TEMPORARY");
        }

        const uint FILE_ATTRIBUTE_VIRTUAL = 0x00010000;
        if (((uint)attrs & FILE_ATTRIBUTE_VIRTUAL) != 0)
        {
            sb.Append(":VIRTUAL");
        }

        return sb.ToString();
    }

    public static string FormatReparseTag(IoReparseTag tag) => (uint)tag switch {
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
        {
            sb.Append($"{b:x2}");
        }

        return sb.ToString();
    }

    public static string FormatRegistryValueType(RegistryValueKind kind) =>
        FormatRegistryValueType(unchecked((uint)kind));

    public static string FormatRegistryValueType(uint type) => type switch {
        0 => "NONE",
        1 => "SZ",
        2 => "EXPAND_SZ",
        3 => "BINARY",
        4 => "DWORD",
        5 => "DWORD_BIG_ENDIAN",
        6 => "LINK",
        7 => "MULTI_SZ",
        8 => "RESOURCE_LIST",
        9 => "FULL_RESOURCE_DESCRIPTOR",
        10 => "RESOURCE_REQUIREMENTS_LIST",
        11 => "QWORD",
        _ => $"0x{type:X8}"
    };

    public static string FormatVariantValue(object? value)
    {
        if (value is null)
        {
            return string.Empty;
        }

        return value switch {
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

    public static string FormatCimValue(object? value, CimType cimType)
    {
        if (value is null || value is DBNull)
        {
            return string.Empty;
        }

        var vtPrefix = $"0x{MapCimTypeToVariantType(cimType):X4}:";
        if (value is Array arr)
        {
            return vtPrefix + FormatVariantArray(arr);
        }

        return vtPrefix + FormatVariantValue(value);
    }

    private static ushort MapCimTypeToVariantType(CimType cimType)
    {
        var isArray = false;
        var name = cimType.ToString();
        if (name.EndsWith("Array", StringComparison.Ordinal))
        {
            isArray = true;
            name = name[..^"Array".Length];
        }

        var vt = name switch {
            nameof(CimType.Boolean) => (ushort)0x000B,
            nameof(CimType.Char16) => (ushort)0x0012,
            nameof(CimType.DateTime) => (ushort)0x0008,
            nameof(CimType.Real32) => (ushort)0x0004,
            nameof(CimType.Real64) => (ushort)0x0005,
            nameof(CimType.Reference) => (ushort)0x0008,
            nameof(CimType.SInt8) => (ushort)0x0010,
            nameof(CimType.SInt16) => (ushort)0x0002,
            nameof(CimType.SInt32) => (ushort)0x0003,
            nameof(CimType.SInt64) => (ushort)0x0014,
            nameof(CimType.String) => (ushort)0x0008,
            nameof(CimType.UInt8) => (ushort)0x0011,
            nameof(CimType.UInt16) => (ushort)0x0012,
            nameof(CimType.UInt32) => (ushort)0x0013,
            nameof(CimType.UInt64) => (ushort)0x0015,
            _ => (ushort)0x000C
        };

        return isArray ? (ushort)(0x2000 | vt) : vt;
    }


    private static string FormatVariantArray(Array arr)
    {
        var sb = new StringBuilder();
        var indices = new int[arr.Rank];
        AppendArrayValues(arr, 0, indices, sb);
        return sb.ToString();
    }

    private static void AppendArrayValues(Array arr, int dimension, int[] indices, StringBuilder sb)
    {
        for (var i = arr.GetLowerBound(dimension); i <= arr.GetUpperBound(dimension); i++)
        {
            indices[dimension] = i;
            if (dimension + 1 < arr.Rank)
            {
                AppendArrayValues(arr, dimension + 1, indices, sb);
                continue;
            }

            var formatted = FormatVariantValue(arr.GetValue(indices));
            sb.Append('[');
            sb.AppendJoin(',', indices);
            sb.Append($":{formatted.Length}]");
            sb.Append(formatted);
        }
    }

    public static string EscapeCsvField(string field)
    {
        if (field.AsSpan().IndexOfAny(_csvSpecialChars) < 0)
        {
            return field;
        }

        return $"\"{field.Replace("\"", "\"\"")}\"";
    }

    public static string ToUpperInvariant(string s) =>
        s.ToUpperInvariant();

    public static int CompareKey(string key1, string key2)
    {
        var caseSensitive = string.CompareOrdinal(key1, key2);
        if (caseSensitive == 0)
        {
            return 0;
        }

        var caseInsensitive = string.Compare(key1, key2, StringComparison.OrdinalIgnoreCase);
        return caseInsensitive != 0 ? caseInsensitive : caseSensitive;
    }
}