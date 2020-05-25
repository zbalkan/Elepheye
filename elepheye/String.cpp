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

#include <elepheye/String.hpp>
#include <vector>

namespace elepheye
{
  String string_load(::UINT id)
  {
    // Get the instance of the module.
    ::HINSTANCE instance = ::GetModuleHandle(0);
    if (!instance)
    {
      const ::HRESULT hr = HRESULT_FROM_WIN32(::GetLastError());
      const ::TCHAR s[]
        = _T("(ERROR: Failed to load the string: HRESULT=0x%X)");
      ::TCHAR alt[sizeof(s) / sizeof(s[0]) + sizeof(hr) * 2];
      _stprintf(alt, s, hr);
      return alt;
    }

    // Load the string.
    std::vector<::TCHAR> buffer(0x100, 0);
    const std::size_t max_length = 0x1000;
    do
    {
      const int copied = ::LoadString(
        instance, id, &buffer.at(0), static_cast<int>(buffer.size()));
      if (0 >= copied)
      {
        const ::HRESULT hr = HRESULT_FROM_WIN32(::GetLastError());
        const ::TCHAR s[]
          = _T("(ERROR: ID=%u: Failed to load the string: HRESULT=0x%X)");
        ::TCHAR alt[sizeof(s) / sizeof(s[0]) + sizeof(id) * 3 + sizeof(hr) * 2];
        _stprintf(alt, s, id, hr);
        return alt;
      }
      if (static_cast<int>(buffer.size()) == copied + 1)
      {
        // Double the buffer length, and retry.
        const std::size_t next_length = buffer.size() * 2;
        if (max_length < next_length)
        {
          const ::TCHAR s[]
            = _T("(ERROR: ID=%u: Failed to load the string: Too log string)");
          ::TCHAR alt[sizeof(s) / sizeof(s[0]) + sizeof(id) * 3];
          _stprintf(alt, s, id);
          return alt;
        }
        buffer.resize(next_length);
        ::SecureZeroMemory(&buffer.at(0), buffer.size() * sizeof(buffer.at(0)));
      }
    }
    while (!buffer.at(0));
    return &buffer.at(0);
  }
}
