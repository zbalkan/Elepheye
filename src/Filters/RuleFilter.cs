using System.Runtime.CompilerServices;
using System.Text;
using Elepheye.Core;

namespace Elepheye.Filters;

internal sealed class RuleToken(string value, bool isSwitch, bool isQuoted)
{
    public string Value { get; } = value;
    public bool IsSwitch { get; } = isSwitch;
    public bool IsQuoted { get; } = isQuoted;
}

internal sealed class RuleCommands
{
    public bool Exits { get; set; }
    public bool Drops { get; set; }
    public List<int> FieldsToDisable { get; } = [];
    public List<int> FieldsToIgnoreCase { get; } = [];
    public List<int> OptionsToSet { get; } = [];
}

internal sealed class RuleMatch(int field, bool inverts, Wildcard pattern)
{
    public bool Match(IRecord record)
    {
        var result = pattern.Match(record.GetField(field));
        return inverts ? !result : result;
    }
}

internal sealed class RuleEntry
{
    public List<RuleMatch> Matches { get; } = [];
    public RuleCommands Commands { get; } = new();

    public bool Match(IRecord record) => Matches.All(m => m.Match(record));
}

public sealed class RuleFilter(IAsyncEnumerable<IRecord> source, RecordName name, string rulePath)
{
    private List<RuleEntry>? _rules;

    public async IAsyncEnumerable<IRecord> ReadAsync(
        [EnumeratorCancellation] CancellationToken ct = default)
    {
        _rules ??= await ParseRulesAsync(ct);

        var disables = new bool[name.FieldNames.Count];
        var ignoresCase = new bool[name.FieldNames.Count];
        var cachedFields = new string?[name.FieldNames.Count];

        await foreach (var record in source.WithCancellation(ct))
        {
            Array.Clear(disables, 0, disables.Length);
            Array.Clear(ignoresCase, 0, ignoresCase.Length);
            Array.Clear(cachedFields, 0, cachedFields.Length);

            var drops = false;
            var wrapped = new RuleWrappedRecord(record, disables, ignoresCase, cachedFields, name.FieldNames.Count);

            foreach (var entry in _rules)
            {
                if (entry.Match(wrapped))
                {
                    ApplyCommands(entry.Commands, disables, ignoresCase, wrapped, record);
                    if (entry.Commands.Drops)
                    {
                        drops = true;
                    }

                    if (entry.Commands.Exits)
                    {
                        break;
                    }
                }
            }

            if (!drops)
            {
                yield return wrapped;
            }
        }
    }

    private static void ApplyCommands(RuleCommands cmds, bool[] disables, bool[] ignoresCase,
        RuleWrappedRecord wrapped, IRecord source)
    {
        foreach (var f in cmds.FieldsToDisable)
        {
            disables[f] = true;
        }

        foreach (var f in cmds.FieldsToIgnoreCase)
        {
            ignoresCase[f] = true;
        }

        foreach (var o in cmds.OptionsToSet)
        {
            source.SetOption(o);
        }
    }

    private async Task<List<RuleEntry>> ParseRulesAsync(CancellationToken ct)
    {
        var entries = new List<RuleEntry>();
        var lines = await TokenizeFileAsync(ct);

        foreach (var line in lines)
        {
            if (line.Count == 0)
            {
                continue;
            }

            var entry = ParseLine(line);
            if (entry is not null)
            {
                entries.Add(entry);
            }
        }

        return entries;
    }

    private async Task<List<List<RuleToken>>> TokenizeFileAsync(CancellationToken ct)
    {
        var lines = new List<List<RuleToken>>();
        var text = await File.ReadAllTextAsync(rulePath, Encoding.UTF8, ct);
        var span = text.AsSpan();
        var i = 0;

        while (i <= span.Length)
        {
            var tokens = new List<RuleToken>();
            var tokenBuf = new StringBuilder();
            var quoted = false;
            var hasContent = false;

            while (i < span.Length)
            {
                var c = span[i];

                if (c == '#' && !quoted)
                {
                    // Comment to end of line
                    while (i < span.Length && span[i] != '\n' && span[i] != '\r')
                    {
                        i++;
                    }

                    break;
                }
                if ((c == '\n' || c == '\r') && !quoted)
                {
                    FlushToken(tokens, tokenBuf, ref hasContent, quoted: false);
                    i++;
                    if (i < span.Length && span[i] == '\n' && c == '\r')
                    {
                        i++;
                    }

                    break;
                }
                if ((c == ' ' || c == '\t') && !quoted)
                {
                    FlushToken(tokens, tokenBuf, ref hasContent, quoted: false);
                    i++;
                    continue;
                }
                if (c == '"')
                {
                    if (!quoted)
                    {
                        // Start of quoted token
                        FlushToken(tokens, tokenBuf, ref hasContent, quoted: false);
                        quoted = true;
                        hasContent = true;
                        i++;
                    }
                    else
                    {
                        i++;
                        if (i < span.Length && span[i] == '"')
                        {
                            tokenBuf.Append('"');
                            i++;
                        }
                        else
                        {
                            // End of quoted token
                            FlushToken(tokens, tokenBuf, ref hasContent, quoted: true);
                            quoted = false;
                        }
                    }
                    continue;
                }

                tokenBuf.Append(c);
                hasContent = true;
                i++;
            }

            FlushToken(tokens, tokenBuf, ref hasContent, quoted: false);

            if (tokens.Count > 0)
            {
                lines.Add(tokens);
            }

            if (i >= span.Length)
            {
                break;
            }
        }

        return lines;
    }

    private static void FlushToken(List<RuleToken> tokens, StringBuilder buf, ref bool hasContent, bool quoted)
    {
        if (!hasContent && !quoted)
        {
            return;
        }

        var val = buf.ToString();
        buf.Clear();
        hasContent = false;

        var isSwitch = false;
        if (!quoted && val.StartsWith("--", StringComparison.Ordinal))
        {
            val = val[2..];
            isSwitch = true;
        }

        tokens.Add(new RuleToken(val, isSwitch, quoted));
    }

    private RuleEntry? ParseLine(List<RuleToken> tokens)
    {
        var entry = new RuleEntry();
        var pos = 0;

        // Parse matches: <field> [--ignore-case] [--not] <pattern> [--and ...]
        do
        {
            if (pos >= tokens.Count)
            {
                return null;
            }

            var fieldIdx = FindField(tokens[pos].Value);
            if (fieldIdx < 0)
            {
                ExceptionSink.Log(new UserException(tokens[pos].Value, "parse rule", "unknown field name"));
                return null;
            }
            pos++;

            var ignoreCase = false;
            var invert = false;
            while (pos < tokens.Count && tokens[pos].IsSwitch)
            {
                var sw = tokens[pos].Value;
                if (sw == "ignore-case") { ignoreCase = true; pos++; }
                else if (sw == "not") { invert = true; pos++; }
                else
                {
                    break;
                }
            }

            if (pos >= tokens.Count)
            {
                return null;
            }

            var pattern = tokens[pos].Value;
            pos++;

            entry.Matches.Add(new RuleMatch(fieldIdx, invert, new Wildcard(pattern, ignoreCase)));
        }
        while (pos < tokens.Count && tokens[pos].IsSwitch && tokens[pos].Value == "and" && ++pos > 0);

        // Parse commands
        while (pos < tokens.Count)
        {
            if (!tokens[pos].IsSwitch)
            {
                ExceptionSink.Log(new UserException(tokens[pos].Value, "parse rule", "expected switch"));
                return null;
            }
            var cmd = tokens[pos].Value;
            pos++;

            switch (cmd)
            {
                case "exit":
                    entry.Commands.Exits = true;
                    break;
                case "drop":
                    entry.Commands.Drops = true;
                    break;
                case "disable":
                    while (pos < tokens.Count && !tokens[pos].IsSwitch)
                    {
                        var idx = FindField(tokens[pos].Value);
                        if (idx == 0)
                        {
                            entry.Commands.Drops = true;
                        }
                        else if (idx > 0)
                        {
                            entry.Commands.FieldsToDisable.Add(idx);
                        }

                        pos++;
                    }
                    break;
                case "ignore-case":
                    while (pos < tokens.Count && !tokens[pos].IsSwitch)
                    {
                        var idx = FindField(tokens[pos].Value);
                        if (idx >= 0)
                        {
                            entry.Commands.FieldsToIgnoreCase.Add(idx);
                        }

                        pos++;
                    }
                    break;
                case "set":
                    while (pos < tokens.Count && !tokens[pos].IsSwitch)
                    {
                        var idx = FindOption(tokens[pos].Value);
                        if (idx >= 0)
                        {
                            entry.Commands.OptionsToSet.Add(idx);
                        }

                        pos++;
                    }
                    break;
                default:
                    ExceptionSink.Log(new UserException(cmd, "parse rule", "unknown command"));
                    return null;
            }
        }

        return entry;
    }

    private int FindField(string fieldName)
    {
        for (var i = 0; i < name.FieldNames.Count; i++)
        {
            if (name.FieldNames[i] == fieldName)
            {
                return i;
            }
        }

        return -1;
    }

    private int FindOption(string optionName)
    {
        for (var i = 0; i < name.OptionNames.Count; i++)
        {
            if (name.OptionNames[i] == optionName)
            {
                return i;
            }
        }

        return -1;
    }
}

internal sealed class RuleWrappedRecord(
    IRecord source, bool[] disables, bool[] ignoresCase, string?[] cache, int fieldCount) : IRecord
{
    public string GetField(int index)
    {
        if (disables[index])
        {
            return string.Empty;
        }

        if (ignoresCase[index])
        {
            return cache[index] ??= FieldFormatter.ToUpperInvariant(source.GetField(index));
        }

        return source.GetField(index);
    }

    public bool GetOption(int index) => source.GetOption(index);
    public void SetOption(int index) => source.SetOption(index);
}
