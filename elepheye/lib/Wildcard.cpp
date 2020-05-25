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

#include <elepheye/lib/Field.hpp>
#include <elepheye/lib/Wildcard.hpp>

namespace elepheye
{
  namespace lib
  {
    Wildcard::Wildcard(const String &pattern, bool ignores_case)
      : ignores_case_(ignores_case)
    {
      // Convert the pattern to uppercase if ignores case.
      if (ignores_case_)
      {
        pattern_ = Field::toUpper(pattern);
      }
      else
      {
        pattern_ = pattern;
      }
    }

    bool Wildcard::match(const String& s) const
    {
      // Convert the string to uppercase if ignores case.
      if (ignores_case_)
      {
        String upper(Field::toUpper(s));
        return match(pattern_.c_str(), upper.c_str());
      }
      else
      {
        return match(pattern_.c_str(), s.c_str());
      }
    }

    bool Wildcard::match(const ::TCHAR* p, const ::TCHAR* s) const
    {
      switch (*p)
      {
      case _T('\0'):
        return *p == *s;
      case _T('*'):
        return match(p + 1, s) || *s && match(p, s + 1);
      case _T('?'):
        return *s && match(p + 1, s + 1);
      default:
        return *p == *s && match(p + 1, s + 1);
      }
    }
  }
}
