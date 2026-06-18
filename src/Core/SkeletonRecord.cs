namespace Elepheye.Core;

public sealed class SkeletonRecord(int fieldCount, int optionCount = 0) : IRecord
{
    private readonly string[] _fields = new string[fieldCount];
    private readonly bool[] _options = new bool[optionCount];

    public void SetField(int index, string value) => _fields[index] = value;

    public string GetField(int index) => _fields[index] ?? string.Empty;

    public bool GetOption(int index) => _options[index];

    public void SetOption(int index) => _options[index] = true;
}
