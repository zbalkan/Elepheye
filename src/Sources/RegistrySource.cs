using System.Runtime.CompilerServices;
using System.Security.AccessControl;
using Elepheye.Core;
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
                        all.Add(BuildValueRecord(path, key, valueName!));
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
                    RegistryRights.ReadKey | RegistryRights.EnumerateSubKeys |
                    RegistryRights.QueryValues | RegistryRights.ReadPermissions)
                : root;

            if (key is null) return;

            results.Add(BuildKeyRecord(keyPath, key));

            // Enumerate sub-keys
            foreach (var subKeyName in key.GetSubKeyNames())
            {
                ct.ThrowIfCancellationRequested();
                string childPath = keyPath + "\\" + subKeyName;
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
                string valuePath = keyPath + "\\\\" + valueName;
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
            if (key != root) key?.Dispose();
        }
    }

    private static IRecord BuildKeyRecord(string path, RegistryKey key)
    {
        var r = new SkeletonRecord(11);
        r.SetField(0, path);

        // class (key class name - not available via managed RegistryKey API, leave empty)
        r.SetField(1, string.Empty);

        // last_write_time — not directly available via managed API, use P/Invoke or skip
        r.SetField(2, string.Empty);

        // owner, group, dacl, sacl via RegistrySecurity
        TrySetSddl(r, 5, 6, 7, 8, key, path);

        // md5, sha1 not applicable for keys
        return r;
    }

    private static IRecord BuildValueRecord(string path, RegistryKey key, string valueName)
    {
        var r = new SkeletonRecord(11);
        r.SetField(0, path);

        // type
        try
        {
            var kind = key.GetValueKind(valueName);
            r.SetField(3, FieldFormatter.FormatRegistryValueType(kind));

            // size — get raw bytes
            var raw = key.GetValue(valueName, null, RegistryValueOptions.DoNotExpandEnvironmentNames);
            if (raw is byte[] bytes)
            {
                r.SetField(4, bytes.Length.ToString());
                r.SetField(9, FieldFormatter.FormatHashBytes(
                    System.Security.Cryptography.MD5.HashData(bytes)));
                r.SetField(10, FieldFormatter.FormatHashBytes(
                    System.Security.Cryptography.SHA1.HashData(bytes)));
            }
            else if (raw is not null)
            {
                var encoded = System.Text.Encoding.Unicode.GetBytes(raw.ToString() ?? string.Empty);
                r.SetField(4, encoded.Length.ToString());
                r.SetField(9, FieldFormatter.FormatHashBytes(
                    System.Security.Cryptography.MD5.HashData(encoded)));
                r.SetField(10, FieldFormatter.FormatHashBytes(
                    System.Security.Cryptography.SHA1.HashData(encoded)));
            }
        }
        catch (Exception e)
        {
            ExceptionSink.Log(e, $"reading value {path}",
                e is UnauthorizedAccessException ? ExceptionLevel.Warning : ExceptionLevel.Error);
        }

        return r;
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
        string localPath = path;
        string? machineName = null;

        if (path.StartsWith(@"\\", StringComparison.Ordinal))
        {
            int nextSep = path.IndexOf('\\', 2);
            if (nextSep < 0) return (null, null, null, null, false);
            machineName = path[..nextSep];
            localPath = path[(nextSep + 1)..];
        }

        // Detect value separator "\\"
        string? valueName = null;
        bool isKey = true;
        int valueSepIdx = localPath.IndexOf(@"\\", StringComparison.Ordinal);
        if (valueSepIdx >= 0)
        {
            valueName = localPath[(valueSepIdx + 2)..];
            localPath = localPath[..valueSepIdx];
            isKey = false;
        }

        // Split root key from sub key
        int firstSep = localPath.IndexOf('\\');
        string rootName = firstSep < 0 ? localPath : localPath[..firstSep];
        string? subKey = firstSep < 0 ? null : localPath[(firstSep + 1)..];
        if (subKey?.EndsWith('\\') == true)
            subKey = subKey[..^1];

        RegistryHive? hive = rootName.ToUpperInvariant() switch
        {
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
