using System.Runtime.CompilerServices;
using System.Text;
using Elepheye.Core;

namespace Elepheye.Sources;

public sealed class CsvSource(string path)
{
    public RecordName Name { get; } = ParseHeader(path);

    private static RecordName ParseHeader(string path)
    {
        using var reader = new StreamReader(path, Encoding.UTF8);
        string? header = reader.ReadLine();
        var n = new RecordName();
        if (header is not null)
            n.FieldNames.AddRange(ParseCsvLine(header.AsSpan()));
        return n;
    }

    public async IAsyncEnumerable<IRecord> ReadAsync(
        [EnumeratorCancellation] CancellationToken ct = default)
    {
        using var reader = new StreamReader(path, Encoding.UTF8);
        string? header = await reader.ReadLineAsync(ct);
        if (header is null) yield break;

        var fieldNames = Name.FieldNames;

        int lineNumber = 2;
        string? line;
        while ((line = await reader.ReadLineAsync(ct)) is not null)
        {
            ct.ThrowIfCancellationRequested();
            var fields = ParseCsvLine(line.AsSpan());
            if (fields.Count != fieldNames.Count)
                throw new UserException($"line {lineNumber}", "parse CSV",
                    $"expected {fieldNames.Count} fields, got {fields.Count}");

            var record = new SkeletonRecord(fields.Count);
            for (int i = 0; i < fields.Count; i++)
                record.SetField(i, fields[i]);
            yield return record;
            lineNumber++;
        }
    }

    internal static List<string> ParseCsvLine(ReadOnlySpan<char> line)
    {
        var fields = new List<string>();
        var sb = new StringBuilder();
        int i = 0;

        while (i <= line.Length)
        {
            if (i == line.Length)
            {
                fields.Add(sb.ToString());
                break;
            }
            char c = line[i];
            if (c == ',')
            {
                fields.Add(sb.ToString());
                sb.Clear();
                i++;
            }
            else if (c == '"')
            {
                i++; // consume opening quote
                while (i < line.Length)
                {
                    if (line[i] == '"')
                    {
                        i++;
                        if (i < line.Length && line[i] == '"')
                        {
                            sb.Append('"'); // escaped quote
                            i++;
                        }
                        else break; // closing quote
                    }
                    else
                    {
                        sb.Append(line[i]);
                        i++;
                    }
                }
            }
            else
            {
                sb.Append(c);
                i++;
            }
        }
        return fields;
    }
}
