namespace Elepheye.Core;

public static class ExceptionSink
{
    private static ExceptionLevel _maxLevel = ExceptionLevel.Warning - 1;
    private static bool _verbose;

    public static void EnableWarnings() => _verbose = true;

    public static ExceptionLevel MaxLevel => _maxLevel;

    public static void Log(ElepheyeException e)
    {
        if (e.Level > _maxLevel)
        {
            _maxLevel = e.Level;
        }

        if (e.Level == ExceptionLevel.Warning && !_verbose)
        {
            return;
        }

        var label = e.Level == ExceptionLevel.Warning ? "WARNING" : "ERROR";
        Console.Error.WriteLine($"{label} | {e.Message}");
    }

    public static void Log(Exception e, string issue, ExceptionLevel level = ExceptionLevel.Error)
    {
        var wrapped = new ElepheyeException(null, issue, e.Message, e.HResult) { Level = level };
        Log(wrapped);
    }
}
