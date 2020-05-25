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

#ifndef ELEPHEYE_SOURCE_CSV_PARSER_HPP
#define ELEPHEYE_SOURCE_CSV_PARSER_HPP

#include <elepheye/ForwardIterator.hpp>
#include <elepheye/InputFilter.hpp>
#include <elepheye/lib/TextValue.hpp>
#include <vector>

namespace elepheye
{
  namespace source
  {
    /**
     * Implementation of InputFilter to parse the CSV text.
     */
    class CsvParser
      : public InputFilter<std::vector<String>, String, lib::TextValue>
    {
    public:

      /**
       * Constructor.
       * @param src - The source instance.
       */
      explicit CsvParser(const Shared<SourceType>& src);

      bool input();

      const ValueType& get() const;

      ValueType& get();

    private:
      void prepareField();
      void terminateRecord();
      void parseEscapedField();
      ForwardIterator<SourceType> src_;
      ForwardIterator<SourceType> src_end_;
      ValueType value_;
      std::size_t line_;
      std::size_t index_;
    };
  }
}

#endif // #ifndef ELEPHEYE_SOURCE_CSV_PARSER_HPP
