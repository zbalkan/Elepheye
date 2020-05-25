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

#ifndef ELEPHEYE_LIB_TEXT_VALUE_HPP
#define ELEPHEYE_LIB_TEXT_VALUE_HPP

#include <elepheye/lib/TextPosition.hpp>

namespace elepheye
{
  namespace lib
  {
    /**
     * Class for the value of TextInput.
     */
    class TextValue
    {
    public:

      /**
       * Constructor.
       */
      TextValue();

      /**
       * Get the character.
       * @return The character.
       */
      ::TCHAR get() const;

      /**
       * Set the character.
       * @param c - The character.
       */
      void set(::TCHAR c);

      /**
       * Get the position.
       * @return The position.
       */
      const TextPosition& getPosition() const;

      /**
       * Get the position.
       * @return The position.
       */
      TextPosition& getPosition();

    private:
      ::TCHAR c_;
      TextPosition position_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_TEXT_VALUE_HPP
