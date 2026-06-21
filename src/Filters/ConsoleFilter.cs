using System.Runtime.CompilerServices;
using Elepheye.Core;

namespace Elepheye.Filters;

public sealed class ConsoleFilter(IAsyncEnumerable<IRecord> source, RecordName name)
{
    public async IAsyncEnumerable<IRecord> ReadAsync(
        [EnumeratorCancellation] CancellationToken ct = default)
    {
        int maxLen = name.FieldNames.Max(n => n.Length);

        await foreach (var record in source.WithCancellation(ct))
        {
            for (int i = 0; i < name.FieldNames.Count; i++)
            {
                string label = name.FieldNames[i].PadLeft(maxLen);
                Console.WriteLine($"{label}: {record.GetField(i)}");
            }
            Console.WriteLine();
            yield return record;
        }
    }
}
