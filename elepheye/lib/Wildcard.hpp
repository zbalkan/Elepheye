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

#ifndef ELEPHEYE_LIB_WILDCARD_HPP
#define ELEPHEYE_LIB_WILDCARD_HPP

#include <elepheye/String.hpp>

namespace elepheye
{
  namespace lib
  {
    /**
     * Class for the wildcard match.
     */
    class Wildcard
    {
    public:

      /**
       * Constructor.
       * @param pattern - The wildcard pattern.
       * @param ignores_case - True if ignores case.
       */
      Wildcard(const String& pattern, bool ignores_case);

      /**
       * Match a string.
       * @param s - The string.
       * @return True if matches.
       */
      bool match(const String& s) const;

    private:
      bool match(const ::TCHAR* p, const ::TCHAR* s) const;
      String pattern_;
      bool ignores_case_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_WILDCARD_HPP
