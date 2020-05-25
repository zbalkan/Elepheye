/*
 * Elepheye
 *
 * Copyright (c) 2009 Michito Kaji
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <elepheye/ExceptionProxy.hpp>
#include <elepheye/Exceptions.hpp>
#include <elepheye/lib/Handles.hpp>
#include <elepheye/lib/Privilege.hpp>

namespace elepheye
{
  namespace lib
  {
    Privilege::Privilege(const String& name, bool is_mandatory) : name_(name)
    {
      try
      {
        set(is_mandatory);
      }
      catch (...)
      {
        reset();
        throw;
      }
    }

    Privilege::~Privilege() throw()
    {
      reset();
    }

    void Privilege::set(bool is_mandatory)
    {
      // Initialize the previous state.
      ::SecureZeroMemory(&prev_, sizeof(prev_));

      // Lookup the privilege value from the name.
      ::TOKEN_PRIVILEGES tp;
      ::SecureZeroMemory(&tp, sizeof(tp));
      tp.PrivilegeCount = 1;
      tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
      if (!::LookupPrivilegeValue(0, name_.c_str(), &tp.Privileges[0].Luid))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setContext(name_);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_ENABLE_PRIVILEGE);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Get the access token of the current process.
      Handles::CloseHandle token;
      if (!::OpenProcessToken(
            ::GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
            &token.get()))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setContext(name_);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_ENABLE_PRIVILEGE);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Enable the privilege and save the previous state.
      ::DWORD prev_size = 0;
      if (!::AdjustTokenPrivileges(
            token.get(), FALSE, &tp, sizeof(prev_), &prev_, &prev_size))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setContext(name_);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_ENABLE_PRIVILEGE);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // The access token does not have the privilege.
      const ::DWORD error = ::GetLastError();
      if (is_mandatory && ERROR_NOT_ALL_ASSIGNED == error)
      {
        Exceptions::System e;
        e.setContext(name_);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_ENABLE_PRIVILEGE);
        e.setWin32(error);
        throw e;
      }
    }

    void Privilege::reset() throw()
    {
      // Reset to the previous status if changed.
      if (prev_.PrivilegeCount)
      {
        Handles::CloseHandle token;
        if (!::OpenProcessToken(
              ::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token.get()))
        {
          const ::DWORD error = ::GetLastError();
          Exception e;
          e.setContext(name_);
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_DISABLE_PRIVILEGE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          ExceptionProxy::log(e);
        }
        else
        {
          if (!::AdjustTokenPrivileges(token.get(), FALSE, &prev_, 0, 0, 0))
          {
            const ::DWORD error = ::GetLastError();
            Exception e;
            e.setContext(name_);
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_DISABLE_PRIVILEGE);
            e.setWin32(error);
            e.setSource(_T(__FILE__), __LINE__);
            ExceptionProxy::log(e);
          }
        }
      }
    }
  }
}
