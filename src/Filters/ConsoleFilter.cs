using System.Runtime.CompilerServices;
using Elepheye.Core;

namespace Elepheye.Filters;

public sealed class ConsoleFilter(IAsyncEnumerable<IRecord> source, RecordName name)
{
    public async IAsyncEnumerable<IRecord> ReadAsync(
        [EnumeratorCancellation] CancellationToken ct = default)
    {
        var maxLen = name.FieldNames.Max(n => n.Length);

        await foreach (var record in source.WithCancellation(ct))
        {
            for (var i = 0; i < name.FieldNames.Count; i++)
            {
                var label = name.FieldNames[i].PadLeft(maxLen);
                Console.WriteLine($"{label}: {record.GetField(i)}");
            }
            Console.WriteLine();
            yield return record;
        }
    }
}