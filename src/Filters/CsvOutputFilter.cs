using System.Runtime.CompilerServices;
using System.Text;
using Elepheye.Core;

namespace Elepheye.Filters;

public sealed class CsvOutputFilter(IAsyncEnumerable<IRecord> source, RecordName name, string outputPath)
{
    public async IAsyncEnumerable<IRecord> ReadAsync(
        [EnumeratorCancellation] CancellationToken ct = default)
    {
        await using var writer = new StreamWriter(outputPath, append: false,
            new UTF8Encoding(encoderShouldEmitUTF8Identifier: false));

        // Write header
        await WriteRow(writer, name.FieldNames, ct);

        await foreach (var record in source.WithCancellation(ct))
        {
            var fields = Enumerable.Range(0, name.FieldNames.Count)
                .Select(i => record.GetField(i))
                .ToList();
            await WriteRow(writer, fields, ct);
            yield return record;
        }
    }

    private static async Task WriteRow(StreamWriter w, IReadOnlyList<string> fields, CancellationToken ct)
    {
        var sb = new StringBuilder();
        for (var i = 0; i < fields.Count; i++)
        {
            if (i > 0)
            {
                sb.Append(',');
            }

            sb.Append(FieldFormatter.EscapeCsvField(fields[i]));
        }
        sb.Append("\r\n");
        await w.WriteAsync(sb.ToString().AsMemory(), ct);
    }
}
