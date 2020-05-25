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
#include <elepheye/lib/RegistryKey.hpp>
#include <elepheye/source/RegistryScanner.hpp>

namespace elepheye
{
  namespace source
  {
    void RegistryScanner::findSubKeys(
      const RegistryRecord& parent, std::list<RegistryRecord>& sub_keys)
    {
      // Clear records.
      sub_keys.clear();

      // Do nothing if the parent is not a key.
      if (!parent.getPath().isKey())
      {
        return;
      }

      // Do nothing if ignored.
      if (parent.getOption(RegistryRecord::OPTION_INDEX_IGNORE_CHILDREN))
      {
        return;
      }

      // Open the key.
      lib::RegistryKey key(
        parent.getPath().getMachineName(),
        parent.getPath().getKey(),
        parent.getPath().getSubKeyName(),
        0,
        KEY_READ,
        parent.getPath().get(),
        ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_REGISTRY_SUB_KEYS);

      // Find sub keys.
      // lpcMaxSubKeyLen of ::RegQueryInfoKey is not trusted.
      std::vector<::TCHAR> name(0x100, 0);
      const std::size_t max_name_length = 0x10000;
      ::LONG error = 0;
      for (::DWORD i = 0; ERROR_NO_MORE_ITEMS != error; ++i)
      {
        do
        {
          ::DWORD name_length = static_cast<::DWORD>(name.size());
          error = ::RegEnumKeyEx(
            key.get(), i, &name.at(0), &name_length, 0, 0, 0, 0);
          if (error)
          {
            if (ERROR_NO_MORE_ITEMS == error)
            {
              break;
            }
            if (ERROR_MORE_DATA == error)
            {
              // Double the buffer length, and retry.
              const std::size_t next_length = name.size() * 2;
              if (max_name_length >= next_length)
              {
                name.resize(next_length);
                ::SecureZeroMemory(
                  &name.at(0), name.size() * sizeof(name.at(0)));
              }
              else
              {
                Exceptions::System e;
                e.setContext(parent.getField(0));
                e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_REGISTRY_SUB_KEYS);
                e.setWin32(error);
                e.setSource(_T(__FILE__), __LINE__);
                throw e;
              }
            }
            else
            {
              Exceptions::System e;
              e.setContext(parent.getField(0));
              e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_REGISTRY_SUB_KEYS);
              e.setWin32(error);
              e.setSource(_T(__FILE__), __LINE__);
              throw e;
            }
          }
          else
          {
            if (!name.at(0))
            {
              Exceptions::System e;
              e.setContext(parent.getField(0));
              e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_REGISTRY_SUB_KEYS);
              e.setSource(_T(__FILE__), __LINE__);
              throw e;
            }
            if (name.size() < name_length)
            {
              Exceptions::System e;
              e.setContext(parent.getField(0));
              e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_REGISTRY_SUB_KEYS);
              e.setSource(_T(__FILE__), __LINE__);
              throw e;
            }
            // Add the new record.
            try
            {
              sub_keys.push_back(
                RegistryRecord(parent, String(&name.at(0), name_length), true));
            }
            catch (const Exceptions::System& e)
            {
              // Log system errors.
              ExceptionProxy::log(e);
            }
          }
        }
        while (error);
      }
    }

    void RegistryScanner::findValues(
      const RegistryRecord& parent, std::list<RegistryRecord>& values)
    {
      // Clear records.
      values.clear();

      // Do nothing if the parent is not a key.
      if (!parent.getPath().isKey())
      {
        return;
      }

      // Do nothing if ignored.
      if (parent.getOption(RegistryRecord::OPTION_INDEX_IGNORE_CHILDREN))
      {
        return;
      }

      // Open the key.
      lib::RegistryKey key(
        parent.getPath().getMachineName(),
        parent.getPath().getKey(),
        parent.getPath().getSubKeyName(),
        0,
        KEY_READ,
        parent.getPath().get(),
        ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_REGISTRY_VALUES);

      // Find values.
      // lpcMaxValueNameLen of ::RegQueryInfoKey is not trusted.
      std::vector<::TCHAR> name(0x100, 0);
      const std::size_t max_name_length = 0x10000;
      ::LONG error = 0;
      for (::DWORD i = 0; ERROR_NO_MORE_ITEMS != error; ++i)
      {
        do
        {
          ::DWORD name_length = static_cast<::DWORD>(name.size());
          error = ::RegEnumValue(
            key.get(), i, &name.at(0), &name_length, 0, 0, 0, 0);
          if (error)
          {
            if (ERROR_NO_MORE_ITEMS == error)
            {
              break;
            }
            if (ERROR_MORE_DATA == error)
            {
              // Double the buffer length, and retry.
              const std::size_t next_length = name.size() * 2;
              if (max_name_length >= next_length)
              {
                name.resize(next_length);
                ::SecureZeroMemory(
                  &name.at(0), name.size() * sizeof(name.at(0)));
              }
              else
              {
                Exceptions::System e;
                e.setContext(parent.getField(0));
                e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_REGISTRY_VALUES);
                e.setWin32(error);
                e.setSource(_T(__FILE__), __LINE__);
                throw e;
              }
            }
            else
            {
              Exceptions::System e;
              e.setContext(parent.getField(0));
              e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_REGISTRY_VALUES);
              e.setWin32(error);
              e.setSource(_T(__FILE__), __LINE__);
              throw e;
            }
          }
          else
          {
            if (name.size() < name_length)
            {
              Exceptions::System e;
              e.setContext(parent.getField(0));
              e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_REGISTRY_VALUES);
              e.setSource(_T(__FILE__), __LINE__);
              throw e;
            }
            // Add the new record.
            try
            {
              values.push_back(
                RegistryRecord(
                  parent, String(&name.at(0), name_length), false));
            }
            catch (const Exceptions::System& e)
            {
              // Log system errors.
              ExceptionProxy::log(e);
            }
          }
        }
        while (error);
      }
    }
  }
}
