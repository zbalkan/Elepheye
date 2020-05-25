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
#include <elepheye/filter/Validator.hpp>
#include <elepheye/lib/Field.hpp>

namespace elepheye
{
  namespace filter
  {
    Validator::Validator(const Shared<SourceType>& src)
      : InputFilter(src), is_first_input_(true)
    {
      // Check the field count.
      if (getName().getFieldNames().empty())
      {
        Exceptions::User e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_RECORD);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_FIELDS);
        throw e;
      }
    }

    bool Validator::input()
    {
      // No more records.
      if (!getSource()->input())
      {
        return false;
      }

      // Just save the key field for the first input.
      const String& key(get().getField(0));
      if (is_first_input_)
      {
        is_first_input_ = false;
        prev_key_ = key;
        return true;
      }

      // Check the order of records.
      const int comparison = lib::Field::compareKey(prev_key_, key);
      if (!comparison)
      {
        Exceptions::User e;
        e.setContext(key);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_RECORD);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_KEY_FIELD_NOT_UNIQUE);
        throw e;
      }
      if (0 < comparison)
      {
        Exceptions::User e;
        e.setContext(key);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_RECORD);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_KEY_FIELD_NOT_SORTED);
        throw e;
      }

      // Save the key field for the next input.
      prev_key_ = key;
      return true;
    }
  }
}
