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

#ifndef ELEPHEYE_STRING_HPP
#define ELEPHEYE_STRING_HPP

#include <string>
#include <tchar.h>
#include <windows.h>

namespace elepheye
{
  /**
   * Alias for the string.
   */
  typedef std::basic_string<::TCHAR> String;

  /**
   * Function to load the string from the resource.
   * @param id - The resource ID.
   * @return The string.
   */
  String string_load(::UINT id);

  /**
   * Function to load the string from the resource to the static buffer.
   * @param id - The resource ID.
   * @param buffer - The buffer.
   * @return The buffer.
   */
  template<std::size_t length>
  ::TCHAR* string_load(::UINT id, ::TCHAR(&buffer)[length]) throw()
  {
    // Clear the buffer.
    ::SecureZeroMemory(buffer, sizeof(buffer));

    // Get the instance of the module.
    ::HINSTANCE instance = ::GetModuleHandle(0);
    if (!instance)
    {
      const ::HRESULT hr = HRESULT_FROM_WIN32(::GetLastError());
      const ::TCHAR s[]
        = _T("(ERROR: Failed to load the string: HRESULT=0x%X)");
      ::TCHAR alt[sizeof(s) / sizeof(s[0]) + sizeof(hr) * 2];
      _stprintf(alt, s, hr);
      _tcsncpy(buffer, alt, length - 1);
      return buffer;
    }

    // Load the string.
    const int copied = ::LoadString(instance, id, buffer, length);
    if (0 >= copied)
    {
      const ::HRESULT hr = HRESULT_FROM_WIN32(::GetLastError());
      const ::TCHAR s[]
        = _T("(ERROR: ID=%u: Failed to load the string: HRESULT=0x%X)");
      ::TCHAR alt[sizeof(s) / sizeof(s[0]) + sizeof(id) * 3 + sizeof(hr) * 2];
      _stprintf(alt, s, id, hr);
      _tcsncpy(buffer, alt, length - 1);
      return buffer;
    }
    return buffer;
  }
}

#endif // #ifndef ELEPHEYE_STRING_HPP
