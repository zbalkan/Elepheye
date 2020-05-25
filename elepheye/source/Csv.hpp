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

#ifndef ELEPHEYE_SOURCE_CSV_HPP
#define ELEPHEYE_SOURCE_CSV_HPP

#include <elepheye/Input.hpp>
#include <elepheye/Record.hpp>
#include <elepheye/RecordName.hpp>
#include <elepheye/source/CsvParser.hpp>

namespace elepheye
{
  namespace source
  {
    /**
     * Implementation of Input and Record to read a CSV file (RFC4180).
     */
    class Csv : public Input<Record, RecordName>, public Record
    {
    public:

      /**
       * Constructor.
       * @param path - The path to the CSV file.
       */
      explicit Csv(const String& path);

      bool input();

      const ValueType& get() const;

      ValueType& get();

      const NameType& getName() const;

      const String& getField(std::size_t index) const;

      bool getOption(std::size_t index) const;

      void setOption(std::size_t index);

    private:
      CsvParser parser_;
      NameType name_;
    };
  }
}

#endif // #ifndef ELEPHEYE_SOURCE_CSV_HPP
