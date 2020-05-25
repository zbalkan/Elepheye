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

#ifndef ELEPHEYE_LIB_TEXT_INPUT_HPP
#define ELEPHEYE_LIB_TEXT_INPUT_HPP

#include <elepheye/InputFilter.hpp>
#include <elepheye/lib/TextValue.hpp>

namespace elepheye
{
  namespace lib
  {
    /**
     * Implementation of InputFilter for a text.
     */
    class TextInput : public InputFilter<TextValue, String, ::TCHAR>
    {
    public:

      /**
       * Constructor.
       * @param src - The source instance.
       */
      explicit TextInput(const Shared<SourceType>& src);

      bool input();

      const ValueType& get() const;

      ValueType& get();

    private:
      enum Newline
      {
        NEWLINE_NONE,
        NEWLINE_CR,
        NEWLINE_CRLF,
        NEWLINE_LF,
        NEWLINE_LFCR,
      };
      Newline newline_;
      ValueType value_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_TEXT_INPUT_HPP
