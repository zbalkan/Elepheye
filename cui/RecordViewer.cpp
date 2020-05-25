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

#include <cui/Console.hpp>
#include <cui/RecordViewer.hpp>
#include <cui/resource.h>
#include <iomanip>
#include <sstream>

namespace cui
{
  RecordViewer::RecordViewer(const elepheye::Shared<SourceType>& src)
    : InputFilter(src)
  {
    // Get the max length of the field name.
    elepheye::String::size_type max_length = 0;
    for (
      std::vector<elepheye::String>::const_iterator i
        = getName().getFieldNames().begin();
      getName().getFieldNames().end() != i;
      ++i)
    {
      if (i->length() > max_length)
      {
        max_length = i->length();
      }
    }

    // Format field names.
    // The field name with full-width characters not supported.
    const elepheye::String sep(
      elepheye::string_load(CUI_RESOURCE_IDS_LABEL_SEPARATOR));
    for (
      std::vector<elepheye::String>::const_iterator i
        = getName().getFieldNames().begin();
      getName().getFieldNames().end() != i;
      ++i)
    {
      std::basic_ostringstream<::TCHAR> s;
      s << std::right
        << std::setfill(_T(' '))
        << std::setw(static_cast<std::streamsize>(max_length))
        << *i
        << sep;
      field_names_.push_back(s.str());
    }
  }

  bool RecordViewer::input()
  {
    // No more records.
    if (!getSource()->input())
    {
      return false;
    }

    // Print the record.
    for (
      std::vector<elepheye::String>::size_type i = 0;
      field_names_.size() > i;
      ++i)
    {
      Console::out << field_names_.at(i) << get().getField(i) << std::endl;
    }
    Console::out << std::endl;
    return true;
  }
}
