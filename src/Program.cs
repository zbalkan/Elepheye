using Elepheye.Core;
using Elepheye.Filters;
using Elepheye.Pipeline;
using Elepheye.Sources;

const string Version = "1.0.0";
const string Help = """
elepheye - System state scanner and comparator

Usage:
  elepheye [-V] [-h] [-w] <source> [args...] [-c <source> [args...]] [-f <filter> [args...]]

Sources:
  csv      <file>            Read records from a CSV file
  filesystem <path...>      Scan filesystem paths recursively
  registry  <path...>       Scan registry keys/values
  wmi       <ns>:<class>    Query WMI class (e.g. root\cimv2:Win32_Process)

Filters (applied to the last source):
  csv      <file>            Write records to CSV file (and pass through)
  rule     <file>            Apply rule file to filter/transform records
  console                   Print records to console (and pass through)

Commands:
  -c <source> [args...]     Add a second source and compare with the first
  -f <filter> [args...]     Apply a filter to the current source

Options:
  -V    Print version
  -h    Print this help
  -w    Enable warning messages
""";

using var cts = new CancellationTokenSource();
Console.CancelKeyPress += (_, e) => { e.Cancel = true; cts.Cancel(); };

try
{
    await RunAsync(args, cts.Token);
}
catch (OperationCanceledException)
{
    Console.Error.WriteLine("Cancelled.");
    return 1;
}
catch (ElepheyeException e)
{
    ExceptionSink.Log(e);
}
catch (Exception e)
{
    ExceptionSink.Log(e, "unexpected error");
}

return ExceptionSink.MaxLevel >= ExceptionLevel.Error ? 1 : 0;

static async Task RunAsync(string[] args, CancellationToken ct)
{
    var tokens = new Queue<string>(args);

    if (tokens.Count == 0)
    {
        Console.Error.WriteLine(Help);
        return;
    }

    // Parse global options
    while (tokens.Count > 0 && tokens.Peek().StartsWith('-') && tokens.Peek().Length == 2)
    {
        string opt = tokens.Peek();
        switch (opt)
        {
            case "-V":
                tokens.Dequeue();
                Console.WriteLine($"elepheye {Version}");
                return;
            case "-h":
                tokens.Dequeue();
                Console.WriteLine(Help);
                return;
            case "-w":
                tokens.Dequeue();
                ExceptionSink.EnableWarnings();
                break;
            default:
                throw new UserException(opt, "parse arguments", "unknown option");
        }
    }

    // Parse first source
    RecordPipeline pipeline = await ParseSourceAsync(tokens, ct);

    // Parse commands and filters
    while (tokens.Count > 0)
    {
        if (!tokens.Peek().StartsWith('-'))
            throw new UserException(tokens.Peek(), "parse arguments", "unexpected token");

        string cmd = tokens.Dequeue();
        switch (cmd)
        {
            case "-c":
            {
                // Parse second source and compare
                RecordPipeline second = await ParseSourceAsync(tokens, ct);
                while (tokens.Count > 0 && tokens.Peek() == "-f")
                {
                    tokens.Dequeue();
                    second = ApplyFilter(second, tokens);
                }
                await CompareAsync(second, pipeline, ct);
                return;
            }
            case "-f":
                pipeline = ApplyFilter(pipeline, tokens);
                break;
            default:
                throw new UserException(cmd, "parse arguments", "unknown command");
        }
    }

    // Input mode: drain all records through the pipeline
    pipeline.WithValidator();
    await pipeline.DrainAsync(ct);
}

static async Task<RecordPipeline> ParseSourceAsync(Queue<string> tokens, CancellationToken ct)
{
    if (tokens.Count == 0 || tokens.Peek().StartsWith('-'))
        throw new UserException(null, "parse arguments", "missing source type");

    string sourceType = tokens.Dequeue();
    var sourceArgs = new List<string>();
    while (tokens.Count > 0 && !tokens.Peek().StartsWith('-'))
        sourceArgs.Add(tokens.Dequeue());

    return sourceType switch
    {
        "csv" => sourceArgs.Count == 1
            ? RecordPipeline.FromCsv(sourceArgs[0])
            : throw new UserException("csv", "create source",
                sourceArgs.Count == 0 ? "missing arguments" : "too many arguments"),

        "filesystem" => sourceArgs.Count > 0
            ? RecordPipeline.FromFilesystem(sourceArgs)
            : throw new UserException("filesystem", "create source", "missing arguments"),

        "registry" => sourceArgs.Count > 0
            ? RecordPipeline.FromRegistry(sourceArgs)
            : throw new UserException("registry", "create source", "missing arguments"),

        "wmi" => sourceArgs.Count == 1
            ? await RecordPipeline.FromWmiAsync(sourceArgs[0], ct)
            : throw new UserException("wmi", "create source",
                sourceArgs.Count == 0 ? "missing arguments" : "too many arguments"),

        _ => throw new UserException(sourceType, "create source", "unknown source type")
    };
}

static RecordPipeline ApplyFilter(RecordPipeline pipeline, Queue<string> tokens)
{
    if (tokens.Count == 0 || tokens.Peek().StartsWith('-'))
        throw new UserException(null, "parse arguments", "missing filter type");

    string filterType = tokens.Dequeue();
    var filterArgs = new List<string>();
    while (tokens.Count > 0 && !tokens.Peek().StartsWith('-'))
        filterArgs.Add(tokens.Dequeue());

    return filterType switch
    {
        "csv" => filterArgs.Count == 1
            ? pipeline.WithCsvOutput(filterArgs[0])
            : throw new UserException("csv", "create filter",
                filterArgs.Count == 0 ? "missing arguments" : "too many arguments"),

        "rule" => filterArgs.Count == 1
            ? pipeline.WithRuleFilter(filterArgs[0])
            : throw new UserException("rule", "create filter",
                filterArgs.Count == 0 ? "missing arguments" : "too many arguments"),

        "console" => filterArgs.Count == 0
            ? pipeline.WithConsoleOutput()
            : throw new UserException("console", "create filter", "too many arguments"),

        _ => throw new UserException(filterType, "create filter", "unknown filter type")
    };
}

static async Task CompareAsync(RecordPipeline from, RecordPipeline to, CancellationToken ct)
{
    var fromName = from.Name;
    var toName = to.Name;

    if (fromName.FieldNames.Count != toName.FieldNames.Count)
        throw new UserException(null, "compare records", "mismatched field count");

    for (int i = 0; i < fromName.FieldNames.Count; i++)
    {
        if (fromName.FieldNames[i] != toName.FieldNames[i])
            throw new UserException(null, "compare records",
                $"mismatched field name at index {i}: {fromName.FieldNames[i]} vs {toName.FieldNames[i]}");
    }

    int fieldCount = fromName.FieldNames.Count;
    var fieldNames = fromName.FieldNames;

    var fromValidated = new ValidatorFilter(from.Stream).ReadAsync(ct);
    var toValidated = new ValidatorFilter(to.Stream).ReadAsync(ct);

    await foreach (var diff in RecordComparator.CompareAsync(fromValidated, toValidated, fieldCount, ct))
    {
        switch (diff.Kind)
        {
            case DiffKind.Removed:
                Console.WriteLine($"Removed: {diff.Key}");
                break;
            case DiffKind.Added:
                Console.WriteLine($"  Added: {diff.Key}");
                break;
            case DiffKind.Changed:
                Console.WriteLine($"Changed: {diff.Key}");
                foreach (int idx in diff.ChangedFields!)
                {
                    Console.WriteLine($"         {fieldNames[idx]}");
                    Console.WriteLine($"           From: {diff.From!.GetField(idx)}");
                    Console.WriteLine($"             To: {diff.To!.GetField(idx)}");
                }
                break;
        }
    }
}
