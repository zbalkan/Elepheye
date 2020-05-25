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

#include <elepheye/Exceptions.hpp>
#include <elepheye/lib/File.hpp>

namespace elepheye
{
  namespace lib
  {
    File::File(
      const String& path,
      ::DWORD desired_access,
      ::DWORD share_mode,
      ::LPSECURITY_ATTRIBUTES security_attributes,
      ::DWORD creation_disposition,
      ::DWORD flags_and_attributes,
      ::HANDLE template_file,
      ::UINT issue)
    {
      // Open the file.
      file_.get() = ::CreateFile(
        path.c_str(),
        desired_access,
        share_mode,
        security_attributes,
        creation_disposition,
        flags_and_attributes,
        template_file);
      if (INVALID_HANDLE_VALUE == file_.get())
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setContext(path);
        e.setIssue(issue);
        e.setWin32(error);
        throw e;
      }
      file_.setName(path);
    }

    const ::HANDLE& File::get() const throw()
    {
      return file_.get();
    }

    ::HANDLE& File::get() throw()
    {
      return file_.get();
    }

    const ::TCHAR* File::getPath() const throw()
    {
      return file_.getName();
    }
  }
}
