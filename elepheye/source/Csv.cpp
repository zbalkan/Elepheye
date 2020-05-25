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
#include <elepheye/lib/FileInput.hpp>
#include <elepheye/lib/InputBuffer.hpp>
#include <elepheye/lib/TextInput.hpp>
#include <elepheye/source/Csv.hpp>

namespace elepheye
{
  namespace source
  {
    Csv::Csv(const String& path)
      : parser_(
          Shared<lib::TextInput>(
            new lib::TextInput(
              Shared<lib::InputBuffer<::TCHAR, String> >(
                new lib::InputBuffer<::TCHAR, String>(
                  0x1000,
                  Shared<lib::FileInput<std::vector<::TCHAR> > >(
                    new lib::FileInput<std::vector<::TCHAR> >(
                      Shared<lib::File>(
                        new lib::File(
                          path,
                          GENERIC_READ,
                          FILE_SHARE_READ,
                          0,
                          OPEN_EXISTING,
                          FILE_FLAG_SEQUENTIAL_SCAN,
                          0)))))))))
    {
      // Parse the header record.
      if (!parser_.input())
      {
        Exceptions::User e;
        e.setContext(path);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_CSV);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_FIELDS);
        throw e;
      }
      name_.getFieldNames() = parser_.get();
    }

    bool Csv::input()
    {
      return parser_.input();
    }

    const Csv::ValueType& Csv::get() const
    {
      return *this;
    }

    Csv::ValueType& Csv::get()
    {
      return *this;
    }

    const Csv::NameType& Csv::getName() const
    {
      return name_;
    }

    const String& Csv::getField(std::size_t index) const
    {
      return parser_.get().at(index);
    }

    bool Csv::getOption(std::size_t) const
    {
      // No options.
      return false;
    }

    void Csv::setOption(std::size_t)
    {
      // No options.
    }
  }
}
