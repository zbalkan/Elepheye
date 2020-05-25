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

#include <elepheye/filter/Csv.hpp>
#include <elepheye/lib/FileOutput.hpp>

namespace elepheye
{
  namespace filter
  {
    Csv::Csv(const String& path, const Shared<SourceType>& src)
      : InputFilter(src),
        text_(
          Shared<lib::FileOutput<String> >(
            new lib::FileOutput<String>(
              Shared<lib::File>(
                new lib::File(
                  path,
                  GENERIC_WRITE,
                  FILE_SHARE_READ,
                  0,
                  CREATE_ALWAYS,
                  FILE_ATTRIBUTE_NORMAL,
                  0)))))
    {
      // Write the header record.
      const std::size_t field_count = getName().getFieldNames().size();
      for (std::size_t i = 0; field_count > i; ++i)
      {
        const String& field = getName().getFieldNames().at(i);
        if (needsToEscapeField(field))
        {
          text_.output(escapeField(field));
        }
        else
        {
          text_.output(field);
        }
        if (field_count - 1 == i)
        {
          text_.output(_T("\r\n"));
        }
        else
        {
          text_.output(_T(","));
        }
      }
    }

    bool Csv::input()
    {
      // No more records.
      if (!getSource()->input())
      {
        return false;
      }

      // Write the record.
      const std::size_t field_count = getName().getFieldNames().size();
      for (std::size_t i = 0; field_count > i; ++i)
      {
        const String& field = get().getField(i);
        if (needsToEscapeField(field))
        {
          text_.output(escapeField(field));
        }
        else
        {
          text_.output(field);
        }
        if (field_count - 1 == i)
        {
          text_.output(_T("\r\n"));
        }
        else
        {
          text_.output(_T(","));
        }
      }
      return true;
    }

    bool Csv::needsToEscapeField(const String& field) const
    {
      // Find characters to be escaped.
      if (String::npos == field.find(_T('"'))
          && String::npos == field.find(_T(','))
          && String::npos == field.find(_T('\r'))
          && String::npos == field.find(_T('\n')))
      {
        return false;
      }
      return true;
    }

    String Csv::escapeField(const String& field) const
    {
      // Escape double quotes.
      String escaped(field);
      String::size_type double_quote = field.find(_T('"'));
      while (String::npos != double_quote)
      {
        escaped.insert(double_quote, 1, _T('"'));
        double_quote = escaped.find(_T('"'), double_quote + 2);
      }

      // Escape the field.
      return _T('"') + escaped + _T('"');
    }
  }
}
