using System.Runtime.CompilerServices;
using Elepheye.Core;
using Microsoft.Management.Infrastructure;

namespace Elepheye.Sources;

public sealed class WmiSource(string wmiPath)
{
    public RecordName? Name { get; private set; }

    public async IAsyncEnumerable<IRecord> ReadAsync(
        [EnumeratorCancellation] CancellationToken ct = default)
    {
        var colon = wmiPath.IndexOf(':');
        if (colon < 0 || colon == 0 || colon == wmiPath.Length - 1)
        {
            throw new UserException(wmiPath, "read WMI", "invalid path (expected namespace:class)");
        }

        var ns = wmiPath[..colon];
        var className = wmiPath[(colon + 1)..];

        using var session = CimSession.Create(".");

        // Get field names from class definition
        using var classDef = session.GetClass(ns, className);
        var fieldNames = new List<string> { "__PATH" };
        foreach (var prop in classDef.CimClassProperties)
        {
            if (prop.Name != "__PATH")
            {
                fieldNames.Add(prop.Name);
            }
        }

        Name = new RecordName();
        Name.FieldNames.AddRange(fieldNames);

        // Enumerate instances and collect all (then sort)
        var all = new List<IRecord>();
        foreach (var instance in session.EnumerateInstances(ns, className))
        {
            ct.ThrowIfCancellationRequested();
            var record = new SkeletonRecord(fieldNames.Count);

            for (var i = 0; i < fieldNames.Count; i++)
            {
                var fname = fieldNames[i];
                try
                {
                    var prop = instance.CimInstanceProperties[fname];
                    var val = prop is null
                        ? string.Empty
                        : FieldFormatter.FormatCimValue(prop.Value, prop.CimType.ToString());
                    record.SetField(i, val);
                }
                catch (Exception e)
                {
                    ExceptionSink.Log(e, $"reading WMI property {fname}");
                    record.SetField(i, string.Empty);
                }
            }

            all.Add(record);
        }

        all.Sort((a, b) => FieldFormatter.CompareKey(a.GetField(0), b.GetField(0)));

        foreach (var rec in all)
        {
            ct.ThrowIfCancellationRequested();
            yield return rec;
            await Task.Yield();
        }
    }
}