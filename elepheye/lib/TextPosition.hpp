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

#ifndef ELEPHEYE_LIB_TEXT_POSITION_HPP
#define ELEPHEYE_LIB_TEXT_POSITION_HPP

#include <elepheye/String.hpp>

namespace elepheye
{
  namespace lib
  {
    /**
     * Class for the text position.
     */
    class TextPosition
    {
    public:

      /**
       * Constructor.
       */
      TextPosition();

      /**
       * Get the line.
       * @return The line.
       */
      std::size_t getLine() const;

      /**
       * Set the line.
       * @param line - The line.
       */
      void setLine(std::size_t line);

      /**
       * Get the column.
       * @return The column.
       */
      std::size_t getColumn() const;

      /**
       * Set the column.
       * @param column - The column.
       */
      void setColumn(std::size_t column);

      /**
       * Format to the string.
       * @param name - The name.
       * @param adds_column - True to add column.
       * @return The string.
       */
      String format(const String& name, bool adds_column = true) const;

    private:
      std::size_t line_;
      std::size_t column_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_TEXT_POSITION_HPP
