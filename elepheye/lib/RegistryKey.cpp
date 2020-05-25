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
#include <elepheye/lib/RegistryKey.hpp>

namespace elepheye
{
  namespace lib
  {
    RegistryKey::RegistryKey(
      const String& machine_name,
      ::HKEY key,
      const String& sub_key,
      ::DWORD options,
      ::REGSAM sam_desired,
      const String& path,
      ::UINT issue)
    {
      // Open the remote registry if required.
      if (!machine_name.empty())
      {
        const ::LONG error = ::RegConnectRegistry(
          machine_name.c_str(), key, &key_remote_.get());
        if (error)
        {
          Exceptions::System e;
          e.setContext(path);
          e.setIssue(issue);
          e.setWin32(error);
          throw e;
        }
        key_remote_.setName(path);
        key = key_remote_.get();
      }

      // Open the key.
      const ::LONG error = ::RegOpenKeyEx(
        key, sub_key.c_str(), options, sam_desired, &key_.get());
      if (error)
      {
        Exceptions::System e;
        e.setContext(path);
        e.setIssue(issue);
        e.setWin32(error);
        throw e;
      }
      key_.setName(path);
    }

    const ::HKEY& RegistryKey::get() const throw()
    {
      return key_.get();
    }

    ::HKEY& RegistryKey::get() throw()
    {
      return key_.get();
    }

    const ::TCHAR* RegistryKey::getPath() const throw()
    {
      return key_.getName();
    }
  }
}
