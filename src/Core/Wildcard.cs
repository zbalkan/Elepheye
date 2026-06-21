namespace Elepheye.Core;

public sealed class Wildcard(string pattern, bool ignoreCase)
{
    private readonly string _pattern = ignoreCase
        ? FieldFormatter.ToUpperInvariant(pattern) : pattern;

    private readonly bool _ignoreCase = ignoreCase;

    public bool Match(string s)
    {
        var target = _ignoreCase ? FieldFormatter.ToUpperInvariant(s) : s;
        return MatchCore(_pattern.AsSpan(), target.AsSpan());
    }

    private static bool MatchCore(ReadOnlySpan<char> pattern, ReadOnlySpan<char> s)
    {
        while (true)
        {
            if (pattern.IsEmpty)
            {
                return s.IsEmpty;
            }

            if (pattern[0] == '*')
            {
                pattern = pattern[1..];
                if (pattern.IsEmpty)
                {
                    return true;
                }

                for (var i = 0; i <= s.Length; i++)
                {
                    if (MatchCore(pattern, s[i..]))
                    {
                        return true;
                    }
                }

                return false;
            }
            if (s.IsEmpty)
            {
                return false;
            }

            if (pattern[0] != '?' && pattern[0] != s[0])
            {
                return false;
            }

            pattern = pattern[1..];
            s = s[1..];
        }
    }
}