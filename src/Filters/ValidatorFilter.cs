using System.Runtime.CompilerServices;
using Elepheye.Core;

namespace Elepheye.Filters;

public sealed class ValidatorFilter(IAsyncEnumerable<IRecord> source)
{
    public async IAsyncEnumerable<IRecord> ReadAsync(
        [EnumeratorCancellation] CancellationToken ct = default)
    {
        string? prevKey = null;
        await foreach (var record in source.WithCancellation(ct))
        {
            var key = record.GetField(0);
            if (prevKey is not null)
            {
                var cmp = FieldFormatter.CompareKey(prevKey, key);
                if (cmp > 0)
                {
                    throw new UserException(key, "validate records", "records not sorted");
                }

                if (cmp == 0)
                {
                    throw new UserException(key, "validate records", "duplicate key");
                }
            }
            prevKey = key;
            yield return record;
        }
    }
}