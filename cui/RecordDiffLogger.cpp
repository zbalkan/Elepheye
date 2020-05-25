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
#include <cui/RecordDiffLogger.hpp>
#include <cui/resource.h>

namespace cui
{
  RecordDiffLogger::RecordDiffLogger(const elepheye::RecordName& name)
    : field_names_(name.getFieldNames()),
      label_removed_(
        elepheye::string_load(CUI_RESOURCE_IDS_LABEL_REMOVED)),
      label_added_(
        elepheye::string_load(CUI_RESOURCE_IDS_LABEL_ADDED)),
      label_changed_(
        elepheye::string_load(CUI_RESOURCE_IDS_LABEL_CHANGED)),
      label_changed_field_(
        elepheye::string_load(CUI_RESOURCE_IDS_LABEL_CHANGED_FIELD)),
      label_changed_from_(
        elepheye::string_load(CUI_RESOURCE_IDS_LABEL_CHANGED_FROM)),
      label_changed_to_(
        elepheye::string_load(CUI_RESOURCE_IDS_LABEL_CHANGED_TO))
  {
  }

  void RecordDiffLogger::logRemoved(const elepheye::Record& removed)
  {
    // Print the key field.
    const elepheye::String& key = removed.getField(0);
    Console::out << label_removed_ << key << std::endl;
  }

  void RecordDiffLogger::logAdded(const elepheye::Record& added)
  {
    // Print the key field.
    const elepheye::String& key = added.getField(0);
    Console::out << label_added_ << key << std::endl;
  }

  void RecordDiffLogger::logChanged(
    const elepheye::Record& from,
    const elepheye::Record& to,
    const std::list<std::size_t>& fields)
  {
    // Print the key field.
    const elepheye::String& key = from.getField(0);
    Console::out << label_changed_ << key << std::endl;

    // Print changed fields.
    for (
      std::list<std::size_t>::const_iterator i = fields.begin();
      fields.end() != i;
      ++i)
    {
      const elepheye::String& field_name = field_names_.at(*i);
      const elepheye::String& field_from = from.getField(*i);
      const elepheye::String& field_to = to.getField(*i);
      Console::out
        << label_changed_field_
        << field_name
        << std::endl
        << label_changed_from_
        << field_from
        << std::endl
        << label_changed_to_
        << field_to
        << std::endl;
    }
  }
}
