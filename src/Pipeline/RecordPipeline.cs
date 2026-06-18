using System.Runtime.CompilerServices;
using System.Threading.Channels;
using Elepheye.Core;
using Elepheye.Filters;
using Elepheye.Sources;

namespace Elepheye.Pipeline;

public sealed class RecordPipeline
{
    private IAsyncEnumerable<IRecord> _stream;

    public RecordPipeline(RecordName name, IAsyncEnumerable<IRecord> stream)
    {
        Name = name;
        _stream = stream;
    }

    public RecordName Name { get; }
    public IAsyncEnumerable<IRecord> Stream => _stream;

    public RecordPipeline WithCsvOutput(string path)
    {
        _stream = new CsvOutputFilter(_stream, Name, path).ReadAsync();
        return this;
    }

    public RecordPipeline WithConsoleOutput()
    {
        _stream = new ConsoleFilter(_stream, Name).ReadAsync();
        return this;
    }

    public RecordPipeline WithRuleFilter(string rulePath)
    {
        _stream = new RuleFilter(_stream, Name, rulePath).ReadAsync();
        return this;
    }

    public RecordPipeline WithValidator()
    {
        _stream = new ValidatorFilter(_stream).ReadAsync();
        return this;
    }

    public async Task DrainAsync(CancellationToken ct = default)
    {
        await foreach (var _ in _stream.WithCancellation(ct)) { }
    }

    public static RecordPipeline FromCsv(string path)
    {
        var src = new CsvSource(path);
        return new RecordPipeline(src.Name, src.ReadAsync());
    }

    public static RecordPipeline FromFilesystem(IReadOnlyList<string> paths)
    {
        var src = new FilesystemSource(paths);
        return new RecordPipeline(src.Name, src.ReadAsync());
    }

    public static RecordPipeline FromRegistry(IReadOnlyList<string> paths)
    {
        var src = new RegistrySource(paths);
        return new RecordPipeline(src.Name, src.ReadAsync());
    }

    public static async Task<RecordPipeline> FromWmiAsync(string wmiPath, CancellationToken ct = default)
    {
        var src = new WmiSource(wmiPath);

        // Use Channel to buffer WMI records from background task
        var channel = Channel.CreateBounded<IRecord>(new BoundedChannelOptions(512)
        {
            SingleReader = true,
            SingleWriter = true,
            FullMode = BoundedChannelFullMode.Wait
        });

        var producer = Task.Run(async () =>
        {
            try
            {
                await foreach (var r in src.ReadAsync(ct))
                {
                    await channel.Writer.WriteAsync(r, ct);
                }
            }
            finally
            {
                channel.Writer.Complete();
            }
        }, ct);

        // Eagerly read a batch to let the source populate Name
        var buffered = new List<IRecord>();
        using var readCts = CancellationTokenSource.CreateLinkedTokenSource(ct);
        readCts.CancelAfter(TimeSpan.FromSeconds(30));

        try
        {
            while (src.Name is null && await channel.Reader.WaitToReadAsync(readCts.Token))
            {
                while (channel.Reader.TryRead(out var r))
                {
                    buffered.Add(r);
                }
            }
            // Drain remaining batch
            while (channel.Reader.TryRead(out var r2))
            {
                buffered.Add(r2);
            }
        }
        catch (OperationCanceledException) when (!ct.IsCancellationRequested) { /* timeout ok */ }

        var name = src.Name ?? new RecordName();

        async IAsyncEnumerable<IRecord> Merged(
            [EnumeratorCancellation] CancellationToken c = default)
        {
            foreach (var r in buffered)
            {
                yield return r;
            }

            await foreach (var r in channel.Reader.ReadAllAsync(c))
            {
                yield return r;
            }

            await producer;
        }

        return new RecordPipeline(name, Merged());
    }
}
