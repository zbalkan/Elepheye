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
#include <elepheye/lib/Field.hpp>
#include <elepheye/lib/RegistryKey.hpp>
#include <elepheye/source/RegistryRecord.hpp>

namespace elepheye
{
  namespace source
  {
    RegistryRecord::RegistryRecord(const String& path)
      : path_(path),
        fields_(FIELD_INDEX_COUNT),
        options_(OPTION_INDEX_COUNT, false)
    {
      // Validate the path by setting a field.
      if (path_.isKey())
      {
        setField(FIELD_INDEX_LAST_WRITE_TIME);
      }
      else
      {
        setField(FIELD_INDEX_TYPE);
      }
    }

    RegistryRecord::RegistryRecord(
      const RegistryRecord& parent, const String& child, bool is_key)
      : path_(parent.path_, child, is_key),
        fields_(FIELD_INDEX_COUNT),
        options_(OPTION_INDEX_COUNT, false)
    {
    }

    const String& RegistryRecord::getField(std::size_t index) const
    {
      // Return the key field.
      if (!index)
      {
        return path_.get();
      }

      // Set the field if empty.
      if (fields_.at(index).empty())
      {
        try
        {
          setField(index);
        }
        catch (const Exceptions::System& e)
        {
          // Log system errors.
          const ::DWORD error = HRESULT_CODE(e.getHresult());
          if (
            ERROR_ACCESS_DENIED == error
            || ERROR_SHARING_VIOLATION == error
            || ERROR_PRIVILEGE_NOT_HELD == error)
          {
            ExceptionProxy::log(e, ExceptionLogger::LEVEL_WARNING);
          }
          else
          {
            ExceptionProxy::log(e);
          }
        }
      }
      return fields_.at(index);
    }

    bool RegistryRecord::getOption(std::size_t index) const
    {
      return options_.at(index);
    }

    void RegistryRecord::setOption(std::size_t index)
    {
      options_.at(index) = true;
    }

    const RegistryPath& RegistryRecord::getPath() const
    {
      return path_;
    }

    void RegistryRecord::setField(std::size_t index) const
    {
      switch (index)
      {
      case FIELD_INDEX_CLASS:
        if (path_.isKey())
        {
          fields_.at(index) = formatClass();
        }
        break;
      case FIELD_INDEX_LAST_WRITE_TIME:
        if (path_.isKey())
        {
          fields_.at(index) = formatLastWriteTime();
        }
        break;
      case FIELD_INDEX_TYPE:
        if (!path_.isKey())
        {
          fields_.at(index) = formatType();
        }
        break;
      case FIELD_INDEX_SIZE:
        if (!path_.isKey())
        {
          fields_.at(index) = formatSize();
        }
        break;
      case FIELD_INDEX_OWNER:
        if (path_.isKey())
        {
          fields_.at(index) = formatSecurityInfo(
            OWNER_SECURITY_INFORMATION, ELEPHEYE_RESOURCE_IDS_ISSUE_READ_OWNER);
        }
        break;
      case FIELD_INDEX_GROUP:
        if (path_.isKey())
        {
          fields_.at(index) = formatSecurityInfo(
            GROUP_SECURITY_INFORMATION, ELEPHEYE_RESOURCE_IDS_ISSUE_READ_GROUP);
        }
        break;
      case FIELD_INDEX_DACL:
        if (path_.isKey())
        {
          fields_.at(index) = formatSecurityInfo(
            DACL_SECURITY_INFORMATION, ELEPHEYE_RESOURCE_IDS_ISSUE_READ_DACL);
        }
        break;
      case FIELD_INDEX_SACL:
        if (path_.isKey())
        {
          fields_.at(index) = formatSecurityInfo(
            SACL_SECURITY_INFORMATION, ELEPHEYE_RESOURCE_IDS_ISSUE_READ_SACL);
        }
        break;
      case FIELD_INDEX_MD5:
        if (!path_.isKey())
        {
          fields_.at(index) = formatHash(lib::Hash::ALGORITHM_MD5);
        }
        break;
      case FIELD_INDEX_SHA1:
        if (!path_.isKey())
        {
          fields_.at(index) = formatHash(lib::Hash::ALGORITHM_SHA1);
        }
        break;
      }
    }

    String RegistryRecord::formatClass() const
    {
      // Open the key.
      lib::RegistryKey key(
        path_.getMachineName(),
        path_.getKey(),
        path_.getSubKeyName(),
        0,
        KEY_READ,
        path_.get(),
        ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);

      // Get the class name.
      std::vector<::TCHAR> name(0x100, 0);
      const std::size_t max_name_length = 0x10000;
      ::DWORD name_length = 0;
      ::LONG error = 0;
      do
      {
        name_length = static_cast<::DWORD>(name.size());
        error = ::RegQueryInfoKey(
          key.get(), &name.at(0), &name_length, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        if (error)
        {
          if (ERROR_MORE_DATA == error)
          {
            // Double the buffer length, and retry.
            const std::size_t next_length = name.size() * 2;
            if (max_name_length >= next_length)
            {
              name.resize(next_length);
              ::SecureZeroMemory(&name.at(0), name.size() * sizeof(name.at(0)));
            }
            else
            {
              Exceptions::System e;
              e.setContext(path_.get());
              e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);
              e.setWin32(error);
              e.setSource(_T(__FILE__), __LINE__);
              throw e;
            }
          }
          else
          {
            Exceptions::System e;
            e.setContext(path_.get());
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);
            e.setWin32(error);
            e.setSource(_T(__FILE__), __LINE__);
            throw e;
          }
        }
      }
      while (error);
      if (name.size() < name_length)
      {
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
      return String(&name.at(0), name_length);
    }

    String RegistryRecord::formatLastWriteTime() const
    {
      // Open the key.
      lib::RegistryKey key(
        path_.getMachineName(),
        path_.getKey(),
        path_.getSubKeyName(),
        0,
        KEY_READ,
        path_.get(),
        ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);

      // Get the last write time.
      ::FILETIME time;
      ::SecureZeroMemory(&time, sizeof(time));
      const ::LONG error
        = ::RegQueryInfoKey(key.get(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &time);
      if (error)
      {
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Format.
      return lib::Field::format(time);
    }

    String RegistryRecord::formatType() const
    {
      // Open the key.
      lib::RegistryKey key(
        path_.getMachineName(),
        path_.getKey(),
        path_.getSubKeyName(),
        0,
        KEY_READ,
        path_.get(),
        ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);

      // Get the value type.
      ::DWORD type = 0;
      const ::LONG error = ::RegQueryValueEx(
        key.get(), path_.getValueName().c_str(), 0, &type, 0, 0);
      if (error)
      {
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);
        e.setWin32(error);
        throw e;
      }

      // Format.
      switch (type)
      {
      case REG_BINARY:
        return _T("BINARY");
      case REG_DWORD: // REG_DWORD_LITTLE_ENDIAN:
        return _T("DWORD");
      case REG_DWORD_BIG_ENDIAN:
        return _T("DWORD_BIG_ENDIAN");
      case REG_EXPAND_SZ:
        return _T("EXPAND_SZ");
      case REG_FULL_RESOURCE_DESCRIPTOR:
        return _T("FULL_RESOURCE_DESCRIPTOR");
      case REG_LINK:
        return _T("LINK");
      case REG_MULTI_SZ:
        return _T("MULTI_SZ");
      case REG_NONE:
        return _T("NONE");
      case REG_QWORD: // REG_QWORD_LITTLE_ENDIAN:
        return _T("QWORD");
      case REG_RESOURCE_LIST:
        return _T("RESOURCE_LIST");
      case REG_RESOURCE_REQUIREMENTS_LIST:
        return _T("RESOURCE_REQUIREMENTS_LIST");
      case REG_SZ:
        return _T("SZ");
      default:
        return lib::Field::formatToHex(type);
      }
    }

    String RegistryRecord::formatSize() const
    {
      // Open the key.
      lib::RegistryKey key(
        path_.getMachineName(),
        path_.getKey(),
        path_.getSubKeyName(),
        0,
        KEY_READ,
        path_.get(),
        ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);

      // Get the value size.
      ::DWORD size = 0;
      const ::LONG error = ::RegQueryValueEx(
        key.get(), path_.getValueName().c_str(), 0, 0, 0, &size);
      if (error)
      {
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Format.
      return lib::Field::format(size);
    }

    String RegistryRecord::formatSecurityInfo(
      ::SECURITY_INFORMATION si, ::UINT issue) const
    {
      // Open the key.
      ::REGSAM rights = READ_CONTROL;
      if (SACL_SECURITY_INFORMATION == si)
      {
        rights |= ACCESS_SYSTEM_SECURITY;
      }
      lib::RegistryKey key(
        path_.getMachineName(),
        path_.getKey(),
        path_.getSubKeyName(),
        0,
        rights,
        path_.get(),
        issue);

      // Get the size of the security descriptor.
      ::DWORD size = 0;
      ::LONG error = ::RegGetKeySecurity(key.get(), si, 0, &size);
      if (error && ERROR_INSUFFICIENT_BUFFER != error)
      {
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(issue);
        e.setWin32(error);
        throw e;
      }
      if (!size)
      {
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(issue);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Get the security descriptor.
      std::vector<::BYTE> buffer(size, 0);
      ::SECURITY_DESCRIPTOR* sd
        = reinterpret_cast<::SECURITY_DESCRIPTOR*>(&buffer.at(0));
      error = ::RegGetKeySecurity(key.get(), si, sd, &size);
      if (error)
      {
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(issue);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Format.
      return lib::Field::format(sd, si, issue);
    }

    String RegistryRecord::formatHash(lib::Hash::Algorithm algo) const
    {
      // Open the key.
      lib::RegistryKey key(
        path_.getMachineName(),
        path_.getKey(),
        path_.getSubKeyName(),
        0,
        KEY_READ,
        path_.get(),
        ELEPHEYE_RESOURCE_IDS_ISSUE_COMPUTE_HASH);

      // Get the value size.
      ::DWORD size = 0;
      const ::LONG error = ::RegQueryValueEx(
        key.get(), path_.getValueName().c_str(), 0, 0, 0, &size);
      if (error)
      {
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_COMPUTE_HASH);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Read the value.
      std::vector<::BYTE> value(size, 0);
      if (size)
      {
        const ::LONG error = ::RegQueryValueEx(
          key.get(), path_.getValueName().c_str(), 0, 0, &value.at(0), &size);
        if (error)
        {
          Exceptions::System e;
          e.setContext(path_.get());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_COMPUTE_HASH);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
      }

      // Compute the hash.
      lib::Hash hash(algo);
      if (!value.empty())
      {
        hash.update(&value.at(0), value.size());
      }

      // Format.
      return hash.format();
    }
  }
}
