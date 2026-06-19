using System.Runtime.CompilerServices;
using Elepheye.Core;

namespace Elepheye.Pipeline;

public sealed class RecordDiff(string key, DiffKind kind, IRecord? from, IRecord? to, IReadOnlyList<int>? changedFields)
{
    public string Key { get; } = key;
    public DiffKind Kind { get; } = kind;
    public IRecord? From { get; } = from;
    public IRecord? To { get; } = to;
    public IReadOnlyList<int>? ChangedFields { get; } = changedFields;
}

public enum DiffKind { Removed, Added, Changed }

public static class RecordComparator
{
    public static async IAsyncEnumerable<RecordDiff> CompareAsync(
        IAsyncEnumerable<IRecord> from,
        IAsyncEnumerable<IRecord> to,
        int fieldCount,
        [EnumeratorCancellation] CancellationToken ct = default)
    {
        await using var fromEnum = from.GetAsyncEnumerator(ct);
        await using var toEnum = to.GetAsyncEnumerator(ct);

        var hasFrom = await fromEnum.MoveNextAsync();
        var hasTo = await toEnum.MoveNextAsync();

        while (hasFrom && hasTo)
        {
            ct.ThrowIfCancellationRequested();
            var f = fromEnum.Current;
            var t = toEnum.Current;

            var cmp = FieldFormatter.CompareKey(f.GetField(0), t.GetField(0));

            if (cmp == 0)
            {
                var changed = new List<int>();
                for (var i = 1; i < fieldCount; i++)
                {
                    if (f.GetField(i) != t.GetField(i))
                    {
                        changed.Add(i);
                    }
                }
                if (changed.Count > 0)
                {
                    yield return new RecordDiff(f.GetField(0), DiffKind.Changed, f, t, changed);
                }

                hasFrom = await fromEnum.MoveNextAsync();
                hasTo = await toEnum.MoveNextAsync();
            }
            else if (cmp < 0)
            {
                yield return new RecordDiff(f.GetField(0), DiffKind.Removed, f, null, null);
                hasFrom = await fromEnum.MoveNextAsync();
            }
            else
            {
                yield return new RecordDiff(t.GetField(0), DiffKind.Added, null, t, null);
                hasTo = await toEnum.MoveNextAsync();
            }
        }

        while (hasFrom)
        {
            ct.ThrowIfCancellationRequested();
            yield return new RecordDiff(fromEnum.Current.GetField(0), DiffKind.Removed, fromEnum.Current, null, null);
            hasFrom = await fromEnum.MoveNextAsync();
        }

        while (hasTo)
        {
            ct.ThrowIfCancellationRequested();
            yield return new RecordDiff(toEnum.Current.GetField(0), DiffKind.Added, null, toEnum.Current, null);
            hasTo = await toEnum.MoveNextAsync();
        }
    }
}
