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

#include <elepheye/ExceptionProxy.hpp>
#include <elepheye/Exceptions.hpp>
#include <elepheye/lib/RecordComparator.hpp>
#include <elepheye/source/Registry.hpp>
#include <elepheye/source/RegistryScanner.hpp>

namespace elepheye
{
  namespace source
  {
    Registry::Registry(const std::vector<String>& paths)
      : is_first_input_(true),
        backup_privilege(SE_BACKUP_NAME),
        security_privilege(SE_SECURITY_NAME)
    {
      // Create the name.
      createName();

      // Create records.
      for (
        std::vector<String>::const_iterator i = paths.begin();
        paths.end() != i;
        ++i)
      {
        values_.push_back(RegistryRecord(*i));
      }

      // Sort records.
      values_.sort(lib::RecordComparator());
    }

    bool Registry::input()
    {
      // No more records.
      if (values_.empty())
      {
        return false;
      }

      // For the first input.
      if (is_first_input_)
      {
        is_first_input_ = false;
        return true;
      }

      // Find sub keys of the last record.
      std::list<RegistryRecord> sub_keys;
      try
      {
        RegistryScanner::findSubKeys(values_.front(), sub_keys);
      }
      catch (const Exceptions::System& e)
      {
        // Log system errors.
        const ::DWORD error = HRESULT_CODE(e.getHresult());
        if (ERROR_ACCESS_DENIED == error || ERROR_SHARING_VIOLATION == error)
        {
          ExceptionProxy::log(e, ExceptionLogger::LEVEL_WARNING);
        }
        else
        {
          ExceptionProxy::log(e);
        }
      }
      if (!sub_keys.empty())
      {
        sub_keys.sort(lib::RecordComparator());
        values_.merge(sub_keys, lib::RecordComparator());
      }

      // Find values of the last record.
      std::list<RegistryRecord> values;
      try
      {
        RegistryScanner::findValues(values_.front(), values);
      }
      catch (const Exceptions::System& e)
      {
        // Log system errors.
        const ::DWORD error = HRESULT_CODE(e.getHresult());
        if (ERROR_ACCESS_DENIED == error || ERROR_SHARING_VIOLATION == error)
        {
          ExceptionProxy::log(e, ExceptionLogger::LEVEL_WARNING);
        }
        else
        {
          ExceptionProxy::log(e);
        }
      }
      if (!values.empty())
      {
        values.sort(lib::RecordComparator());
        values_.merge(values, lib::RecordComparator());
      }

      // Remove the last record.
      values_.pop_front();
      return !values_.empty();
    }

    const Registry::ValueType& Registry::get() const
    {
      return values_.front();
    }

    Registry::ValueType& Registry::get()
    {
      return values_.front();
    }

    const Registry::NameType& Registry::getName() const
    {
      return name_;
    }

    void Registry::createName()
    {
      // Set field names.
      name_.getFieldNames().resize(RegistryRecord::FIELD_INDEX_COUNT);
      name_.getFieldNames().at(RegistryRecord::FIELD_INDEX_PATH)
        =_T("path");
      name_.getFieldNames().at(RegistryRecord::FIELD_INDEX_CLASS)
        =_T("class");
      name_.getFieldNames().at(RegistryRecord::FIELD_INDEX_LAST_WRITE_TIME)
        =_T("last_write_time");
      name_.getFieldNames().at(RegistryRecord::FIELD_INDEX_OWNER)
        =_T("owner");
      name_.getFieldNames().at(RegistryRecord::FIELD_INDEX_GROUP)
        =_T("group");
      name_.getFieldNames().at(RegistryRecord::FIELD_INDEX_DACL)
        =_T("dacl");
      name_.getFieldNames().at(RegistryRecord::FIELD_INDEX_SACL)
        =_T("sacl");
      name_.getFieldNames().at(RegistryRecord::FIELD_INDEX_TYPE)
        =_T("type");
      name_.getFieldNames().at(RegistryRecord::FIELD_INDEX_SIZE)
        =_T("size");
      name_.getFieldNames().at(RegistryRecord::FIELD_INDEX_MD5)
        =_T("md5");
      name_.getFieldNames().at(RegistryRecord::FIELD_INDEX_SHA1)
        =_T("sha1");

      // Set option names.
      name_.getOptionNames().resize(RegistryRecord::OPTION_INDEX_COUNT);
      name_.getOptionNames().at(RegistryRecord::OPTION_INDEX_IGNORE_CHILDREN)
        =_T("ignore_children");
    }
  }
}
