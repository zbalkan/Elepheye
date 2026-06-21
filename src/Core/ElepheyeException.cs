using System.ComponentModel;

namespace Elepheye.Core;

public enum ExceptionLevel
{ Warning, Error }

public class ElepheyeException(string? context, string issue, string? reason = null, int hresult = 0)
    : Exception(BuildMessage(context, issue, reason, hresult))
{
    public ExceptionLevel Level { get; init; } = ExceptionLevel.Error;
    public string? Context { get; } = context;
    public string Issue { get; } = issue;
    public string? Reason { get; } = reason;

    private static string BuildMessage(string? context, string issue, string? reason, int hresult)
    {
        var parts = new List<string>(4);
        if (context is not null)
        {
            parts.Add(context);
        }

        parts.Add(issue);
        if (reason is not null)
        {
            parts.Add(reason);
        }

        if (hresult != 0)
        {
            parts.Add(HResultToString(hresult));
        }

        return string.Join(" | ", parts);
    }

    private static string HResultToString(int hr)
    {
        try { return new Win32Exception(hr).Message + $" (0x{hr:X8})"; }
        catch { return $"0x{hr:X8}"; }
    }
}

public sealed class UserException(string? context, string issue, string? reason = null)
    : ElepheyeException(context, issue, reason);

public sealed class SystemException(string? context, string issue, int hresult)
    : ElepheyeException(context, issue, hresult: hresult)
{
    public SystemException(string? context, string issue, System.Exception inner)
        : this(context, issue, inner.HResult) { }
}