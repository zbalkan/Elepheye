using System.ComponentModel;
using System.Runtime.InteropServices;
using Elepheye.Native;

namespace Elepheye.Security;

public sealed class PrivilegeManager : IDisposable
{
    private const uint TOKEN_ADJUST_PRIVILEGES = 0x0020;
    private const uint TOKEN_QUERY = 0x0008;

    private readonly nint _token;
    private bool _disposed;

    public PrivilegeManager()
    {
        if (!NativeMethods.OpenProcessToken(
                NativeMethods.GetCurrentProcess(),
                TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                out _token))
        {
            throw new Win32Exception(Marshal.GetLastWin32Error(), "OpenProcessToken failed");
        }
    }

    public void EnablePrivilege(string privilegeName)
    {
        if (!NativeMethods.LookupPrivilegeValueW(null, privilegeName, out var luid))
        {
            throw new Win32Exception(Marshal.GetLastWin32Error(),
                $"LookupPrivilegeValue({privilegeName}) failed");
        }

        var tp = new TOKEN_PRIVILEGES
        {
            PrivilegeCount = 1,
            Privileges = new LUID_AND_ATTRIBUTES
            {
                Luid = luid,
                Attributes = TokenPrivilegeAttributes.SE_PRIVILEGE_ENABLED
            }
        };

        NativeMethods.AdjustTokenPrivileges(_token, false, ref tp, 0, nint.Zero, nint.Zero);
        var err = Marshal.GetLastWin32Error();
        if (err != 0)
        {
            throw new Win32Exception(err, $"AdjustTokenPrivileges({privilegeName}) failed");
        }
    }

    public void Dispose()
    {
        if (!_disposed)
        {
            NativeMethods.CloseHandle(_token);
            _disposed = true;
        }
    }
}
