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
#include <elepheye/lib/Handles.hpp>

namespace elepheye
{
  namespace lib
  {
    Handles::BackupRead::BackupRead() throw()
      : Handle(0), file_(INVALID_HANDLE_VALUE)
    {
    }

    Handles::BackupRead::~BackupRead() throw()
    {
      if (INVALID_HANDLE_VALUE != file_ && get())
      {
        if (!::BackupRead(file_, 0, 0, 0, TRUE, FALSE, &get()))
        {
          const ::DWORD error = ::GetLastError();
          Exception e;
          e.setContext(getName());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CLOSE_HANDLE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          ExceptionProxy::log(e);
        }
      }
    }

    const ::HANDLE& Handles::BackupRead::getFile() const throw()
    {
      return file_;
    }

    ::HANDLE& Handles::BackupRead::getFile() throw()
    {
      return file_;
    }

    Handles::CloseHandle::CloseHandle() throw() : Handle(INVALID_HANDLE_VALUE)
    {
    }

    Handles::CloseHandle::~CloseHandle() throw()
    {
      if (INVALID_HANDLE_VALUE != get())
      {
        if (!::CloseHandle(get()))
        {
          const ::DWORD error = ::GetLastError();
          Exception e;
          e.setContext(getName());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CLOSE_HANDLE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          ExceptionProxy::log(e);
        }
      }
    }

    Handles::CryptDestroyHash::CryptDestroyHash() throw() : Handle(0)
    {
    }

    Handles::CryptDestroyHash::~CryptDestroyHash() throw()
    {
      if (get())
      {
        if (!::CryptDestroyHash(get()))
        {
          const ::DWORD error = ::GetLastError();
          Exception e;
          e.setContext(getName());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CLOSE_HANDLE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          ExceptionProxy::log(e);
        }
      }
    }

    Handles::CryptReleaseContext::CryptReleaseContext() throw() : Handle(0)
    {
    }

    Handles::CryptReleaseContext::~CryptReleaseContext() throw()
    {
      if (get())
      {
        if (!::CryptReleaseContext(get(), 0))
        {
          const ::DWORD error = ::GetLastError();
          Exception e;
          e.setContext(getName());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CLOSE_HANDLE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          ExceptionProxy::log(e);
        }
      }
    }

    Handles::FindClose::FindClose() throw() : Handle(INVALID_HANDLE_VALUE)
    {
    }

    Handles::FindClose::~FindClose() throw()
    {
      if (INVALID_HANDLE_VALUE != get())
      {
        if (!::FindClose(get()))
        {
          const ::DWORD error = ::GetLastError();
          Exception e;
          e.setContext(getName());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CLOSE_HANDLE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          ExceptionProxy::log(e);
        }
      }
    }

    Handles::LocalFree::LocalFree() throw() : Handle(0)
    {
    }

    Handles::LocalFree::~LocalFree() throw()
    {
      if (get())
      {
        if (::LocalFree(get()))
        {
          const ::DWORD error = ::GetLastError();
          Exception e;
          e.setContext(getName());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CLOSE_HANDLE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          ExceptionProxy::log(e);
        }
      }
    }

    Handles::RegCloseKey::RegCloseKey() throw() : Handle(0)
    {
    }

    Handles::RegCloseKey::~RegCloseKey() throw()
    {
      if (get())
      {
        const ::LONG error = ::RegCloseKey(get());
        if (error)
        {
          Exception e;
          e.setContext(getName());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CLOSE_HANDLE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          ExceptionProxy::log(e);
        }
      }
    }

    Handles::SafeArrayDestroy::SafeArrayDestroy() throw() : Handle(0)
    {
    }

    Handles::SafeArrayDestroy::~SafeArrayDestroy() throw()
    {
      if (get())
      {
        const ::HRESULT hr = ::SafeArrayDestroy(get());
        if (FAILED(hr))
        {
          Exception e;
          e.setContext(getName());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CLOSE_HANDLE);
          e.setHresult(hr);
          e.setSource(_T(__FILE__), __LINE__);
          ExceptionProxy::log(e);
        }
      }
    }

    Handles::SafeArrayUnaccessData::SafeArrayUnaccessData() throw() : Handle(0)
    {
    }

    Handles::SafeArrayUnaccessData::~SafeArrayUnaccessData() throw()
    {
      if (get())
      {
        const ::HRESULT hr = ::SafeArrayUnaccessData(get());
        if (FAILED(hr))
        {
          Exception e;
          e.setContext(getName());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CLOSE_HANDLE);
          e.setHresult(hr);
          e.setSource(_T(__FILE__), __LINE__);
          ExceptionProxy::log(e);
        }
      }
    }
  }
}
