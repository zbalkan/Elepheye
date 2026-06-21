using System.Runtime.CompilerServices;
using System.Security.AccessControl;
using Elepheye.Core;
using Elepheye.Native;
using Microsoft.Win32;

namespace Elepheye.Sources;

public sealed class RegistrySource(IReadOnlyList<string> paths)
{
    public RecordName Name { get; } = BuildName();

    private static readonly string[] _fieldNames =
    [
        "path", "class", "last_write_time", "type", "size",
        "owner", "group", "dacl", "sacl", "md5", "sha1"
    ];

    private static RecordName BuildName()
    {
        var n = new RecordName();
        n.FieldNames.AddRange(_fieldNames);
        n.OptionNames.Add("ignore_children");
        return n;
    }

    public async IAsyncEnumerable<IRecord> ReadAsync(
        [EnumeratorCancellation] CancellationToken ct = default)
    {
        var all = new List<IRecord>();
        foreach (var path in paths)
        {
            var (hive, machineName, subKey, valueName, isKey) = ParseRegistryPath(path);
            if (hive is null)
            {
                ExceptionSink.Log(new UserException(path, "parse registry path", "unknown root key"));
                continue;
            }

            RegistryKey? root = null;
            try
            {
                root = machineName is not null
                    ? RegistryKey.OpenRemoteBaseKey(hive.Value, machineName)
                    : RegistryKey.OpenBaseKey(hive.Value, RegistryView.Default);

                if (isKey)
                {
                    EnumerateKey(root, subKey, path, all, ct);
                }
                else
                {
                    using var key = subKey is not null ? root.OpenSubKey(subKey) : root;
                    if (key is not null)
                    {
                        all.Add(BuildValueRecord(path, key, valueName!));
                    }
                }
            }
            catch (Exception e)
            {
                ExceptionSink.Log(e, $"opening {path}");
            }
            finally
            {
                root?.Dispose();
            }
        }

        all.Sort((a, b) => FieldFormatter.CompareKey(a.GetField(0), b.GetField(0)));

        foreach (var rec in all)
        {
            ct.ThrowIfCancellationRequested();
            yield return rec;
            await Task.Yield();
        }
    }

    private static void EnumerateKey(
        RegistryKey root, string? subKey, string keyPath, List<IRecord> results, CancellationToken ct)
    {
        ct.ThrowIfCancellationRequested();

        RegistryKey? key = null;
        try
        {
            key = subKey is not null
                ? root.OpenSubKey(subKey, RegistryKeyPermissionCheck.ReadSubTree,
                    RegistryRights.ReadKey)
                : root;

            if (key is null)
            {
                return;
            }

            results.Add(BuildKeyRecord(keyPath, key));

            // Enumerate sub-keys
            foreach (var subKeyName in key.GetSubKeyNames())
            {
                ct.ThrowIfCancellationRequested();
                var childPath = keyPath + "\\" + subKeyName;
                try
                {
                    EnumerateKey(root,
                        subKey is not null ? subKey + "\\" + subKeyName : subKeyName,
                        childPath, results, ct);
                }
                catch (Exception e)
                {
                    ExceptionSink.Log(e, $"enumerating {childPath}",
                        e is UnauthorizedAccessException ? ExceptionLevel.Warning : ExceptionLevel.Error);
                }
            }

            // Enumerate values
            foreach (var valueName in key.GetValueNames())
            {
                ct.ThrowIfCancellationRequested();
                var valuePath = keyPath + "\\\\" + valueName;
                try
                {
                    results.Add(BuildValueRecord(valuePath, key, valueName));
                }
                catch (Exception e)
                {
                    ExceptionSink.Log(e, $"reading value {valuePath}",
                        e is UnauthorizedAccessException ? ExceptionLevel.Warning : ExceptionLevel.Error);
                }
            }
        }
        catch (Exception e) when (e is not OperationCanceledException)
        {
            ExceptionSink.Log(e, $"reading key {keyPath}",
                e is UnauthorizedAccessException ? ExceptionLevel.Warning : ExceptionLevel.Error);
        }
        finally
        {
            if (key != root)
            {
                key?.Dispose();
            }
        }
    }

    private static IRecord BuildKeyRecord(string path, RegistryKey key)
    {
        var r = new SkeletonRecord(11);
        r.SetField(0, path);

        TrySetKeyInfo(r, key);

        // owner, group, dacl, sacl via RegistrySecurity
        TrySetSddl(r, 5, 6, 7, 8, key, path);

        // md5, sha1 not applicable for keys
        return r;
    }

    private static IRecord BuildValueRecord(string path, RegistryKey key, string valueName)
    {
        var r = new SkeletonRecord(11);
        r.SetField(0, path);

        try
        {
            var raw = QueryRawValue(key, valueName);
            if (raw is not null)
            {
                r.SetField(3, FieldFormatter.FormatRegistryValueType(raw.Value.Type));
                r.SetField(4, raw.Value.Data.Length.ToString());
                r.SetField(9, FieldFormatter.FormatHashBytes(
                    System.Security.Cryptography.MD5.HashData(raw.Value.Data)));
                r.SetField(10, FieldFormatter.FormatHashBytes(
                    System.Security.Cryptography.SHA1.HashData(raw.Value.Data)));
            }
        }
        catch (Exception e)
        {
            ExceptionSink.Log(e, $"reading value {path}",
                e is UnauthorizedAccessException ? ExceptionLevel.Warning : ExceptionLevel.Error);
        }

        return r;
    }

    private static void TrySetKeyInfo(SkeletonRecord r, RegistryKey key)
    {
        var classLength = 256u;
        var classBuffer = new char[classLength + 1];
        var result = NativeMethods.RegQueryInfoKeyW(
            key.Handle,
            classBuffer,
            ref classLength,
            nint.Zero,
            out _, out _, out _, out _, out _, out _, out _,
            out var lastWriteTime);

        if (result != 0)
        {
            return;
        }

        r.SetField(1, classLength == 0 ? string.Empty : new string(classBuffer, 0, (int)classLength));
        r.SetField(2, FieldFormatter.FormatDateTime(DateTime.FromFileTimeUtc(lastWriteTime)));
    }

    private static (uint Type, byte[] Data)? QueryRawValue(RegistryKey key, string valueName)
    {
        var size = 0u;
        var result = NativeMethods.RegQueryValueExW(
            key.Handle,
            valueName.Length == 0 ? null : valueName,
            nint.Zero,
            out var type,
            null,
            ref size);

        const int ERROR_SUCCESS = 0;
        const int ERROR_MORE_DATA = 234;
        if (result is not (ERROR_SUCCESS or ERROR_MORE_DATA))
        {
            return null;
        }

        var data = new byte[size];
        result = NativeMethods.RegQueryValueExW(
            key.Handle,
            valueName.Length == 0 ? null : valueName,
            nint.Zero,
            out type,
            data,
            ref size);

        if (result != ERROR_SUCCESS)
        {
            return null;
        }

        if (size != data.Length)
        {
            Array.Resize(ref data, (int)size);
        }

        return (type, data);
    }

    private static void TrySetSddl(SkeletonRecord r, int ownerIdx, int groupIdx,
        int daclIdx, int saclIdx, RegistryKey key, string path)
    {
        try
        {
            var sec = key.GetAccessControl(AccessControlSections.Owner | AccessControlSections.Group |
                                           AccessControlSections.Access);
            r.SetField(ownerIdx, sec.GetSecurityDescriptorSddlForm(AccessControlSections.Owner) ?? string.Empty);
            r.SetField(groupIdx, sec.GetSecurityDescriptorSddlForm(AccessControlSections.Group) ?? string.Empty);
            r.SetField(daclIdx, sec.GetSecurityDescriptorSddlForm(AccessControlSections.Access) ?? string.Empty);
        }
        catch (Exception e)
        {
            ExceptionSink.Log(e, $"reading ACL for {path}", ExceptionLevel.Warning);
        }

        try
        {
            var sacl = key.GetAccessControl(AccessControlSections.Audit);
            r.SetField(saclIdx, sacl.GetSecurityDescriptorSddlForm(AccessControlSections.Audit) ?? string.Empty);
        }
        catch (Exception e)
        {
            ExceptionSink.Log(e, $"reading SACL for {path}", ExceptionLevel.Warning);
        }
    }

    internal static (RegistryHive? hive, string? machineName, string? subKey, string? valueName, bool isKey)
        ParseRegistryPath(string path)
    {
        var localPath = path;
        string? machineName = null;

        if (path.StartsWith(@"\\", StringComparison.Ordinal))
        {
            var nextSep = path.IndexOf('\\', 2);
            if (nextSep < 0)
            {
                return (null, null, null, null, false);
            }

            machineName = path[..nextSep];
            localPath = path[(nextSep + 1)..];
        }

        // Detect value separator "\\"
        string? valueName = null;
        var isKey = true;
        var valueSepIdx = localPath.IndexOf(@"\\", StringComparison.Ordinal);
        if (valueSepIdx >= 0)
        {
            valueName = localPath[(valueSepIdx + 2)..];
            localPath = localPath[..valueSepIdx];
            isKey = false;
        }

        // Split root key from sub key
        var firstSep = localPath.IndexOf('\\');
        var rootName = firstSep < 0 ? localPath : localPath[..firstSep];
        var subKey = firstSep < 0 ? null : localPath[(firstSep + 1)..];
        if (subKey?.EndsWith('\\') == true)
        {
            subKey = subKey[..^1];
        }

        var hive = rootName.ToUpperInvariant() switch {
            "HKCR" or "HKEY_CLASSES_ROOT" => RegistryHive.ClassesRoot,
            "HKCC" or "HKEY_CURRENT_CONFIG" => RegistryHive.CurrentConfig,
            "HKCU" or "HKEY_CURRENT_USER" => RegistryHive.CurrentUser,
            "HKLM" or "HKEY_LOCAL_MACHINE" => RegistryHive.LocalMachine,
            "HKU" or "HKEY_USERS" => RegistryHive.Users,
            _ => (RegistryHive?)null
        };

        return (hive, machineName, subKey, valueName, isKey);
    }
}