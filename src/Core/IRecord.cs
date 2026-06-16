namespace Elepheye.Core;

public interface IRecord
{
    string GetField(int index);
    bool GetOption(int index);
    void SetOption(int index);
}
