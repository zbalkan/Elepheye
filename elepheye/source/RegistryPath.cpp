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
#include <elepheye/source/RegistryPath.hpp>

namespace elepheye
{
  namespace source
  {
    namespace
    {
      // Separators for the path.
      const ::TCHAR sep_ = _T('\\');
      const ::TCHAR* value_sep_ = _T("\\\\");
    }

    RegistryPath::RegistryPath(const String& path) : key_(0), is_key_(false)
    {
      // Check the path.
      if (path.empty())
      {
        Exceptions::User e;
        e.setContext(path_);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_INVALID_PATH);
        throw e;
      }

      // Parse the path.
      split(path);
      join();
      setKey();
    }

    RegistryPath::RegistryPath(
      const RegistryPath& parent, const String& child, bool is_key)
      : path_(parent.path_),
        machine_name_(parent.machine_name_),
        key_name_(parent.key_name_),
        sub_key_name_(parent.sub_key_name_),
        value_name_(parent.value_name_),
        key_(parent.key_),
        is_key_(is_key)
    {
      // Check the child name.
      if (child.empty() && is_key)
      {
        Exceptions::Application e;
        e.setContext(path_);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_INVALID_PATH);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Append the child name.
      if (is_key_)
      {
        if (!sub_key_name_.empty())
        {
          sub_key_name_ += sep_;
        }
        sub_key_name_ += child;
        path_ += sep_;
        path_ += child;
      }
      else
      {
        value_name_ = child;
        path_ += value_sep_;
        path_ += child;
      }

      // Check the relation.
      if (!parent.is_key_)
      {
        Exceptions::Application e;
        e.setContext(path_);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_INVALID_PATH);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
    }

    const String& RegistryPath::get() const
    {
      return path_;
    }

    const String& RegistryPath::getMachineName() const
    {
      return machine_name_;
    }

    const String& RegistryPath::getKeyName() const
    {
      return key_name_;
    }

    const String& RegistryPath::getSubKeyName() const
    {
      return sub_key_name_;
    }

    const String& RegistryPath::getValueName() const
    {
      return value_name_;
    }

    ::HKEY RegistryPath::getKey() const
    {
      return key_;
    }

    bool RegistryPath::isKey() const
    {
      return is_key_;
    }

    void RegistryPath::split(const String& path)
    {
      // Split the path to the machine name and local path.
      String local_path;
      String::size_type found = path.find(_T("\\\\"));
      if (0 != found)
      {
        local_path = path;
      }
      else
      {
        found = path.find(sep_, 2);
        if (String::npos != found)
        {
          machine_name_ = path.substr(0, found);
          local_path = path.substr(found + 1);
        }
        else
        {
          machine_name_ = path;
        }
      }

      // Split the local path to the key name and value name.
      String key_name;
      found = local_path.find(value_sep_);
      if (String::npos == found)
      {
        key_name = local_path;
        is_key_ = true;
      }
      else
      {
        key_name = local_path.substr(0, found);
        value_name_ = local_path.substr(found + 2);
      }

      // Split the key name to the root key name and sub key name.
      found = key_name.find(sep_);
      if (String::npos == found)
      {
        key_name_ = key_name;
      }
      else
      {
        key_name_ = key_name.substr(0, found);
        sub_key_name_ = key_name.substr(found + 1);
        // Remove last separator.
        String::size_type length = sub_key_name_.length();
        if (length && sub_key_name_.at(length - 1) == sep_)
        {
          sub_key_name_.erase(length - 1, 1);
        }
      }
    }

    void RegistryPath::join()
    {
      // Add the machine name.
      path_.clear();
      if (!machine_name_.empty())
      {
        path_ += machine_name_;
      }

      // Add the key name.
      if (!path_.empty())
      {
        path_ += sep_;
      }
      path_ += key_name_;

      // Add the sub key name.
      if (!sub_key_name_.empty())
      {
        path_ += sep_;
        path_ += sub_key_name_;
      }

      // Add the value name.
      if (!is_key_)
      {
        path_ += value_sep_;
        path_ += value_name_;
      }
    }

    void RegistryPath::setKey()
    {
      if (_T("HKCR") == key_name_
          || _T("HKEY_CLASSES_ROOT") == key_name_)
      {
        key_ = HKEY_CLASSES_ROOT;
      }
      else if (_T("HKCC") == key_name_
               || _T("HKEY_CURRENT_CONFIG") == key_name_)
      {
        key_ = HKEY_CURRENT_CONFIG;
      }
      else if (_T("HKCU") == key_name_
               || _T("HKEY_CURRENT_USER") == key_name_)
      {
        key_ = HKEY_CURRENT_USER;
      }
      else if (_T("HKDD") == key_name_
               || _T("HKEY_DYN_DATA") == key_name_)
      {
        key_ = HKEY_DYN_DATA;
      }
      else if (_T("HKLM") == key_name_
               || _T("HKEY_LOCAL_MACHINE") == key_name_)
      {
        key_ = HKEY_LOCAL_MACHINE;
      }
      else if (_T("HKPD") == key_name_
               || _T("HKEY_PERFORMANCE_DATA") == key_name_)
      {
        key_ = HKEY_PERFORMANCE_DATA;
      }
      else if (_T("HKPN") == key_name_
               || _T("HKEY_PERFORMANCE_NLSTEXT") == key_name_)
      {
        key_ = HKEY_PERFORMANCE_NLSTEXT;
      }
      else if (_T("HKPT") == key_name_
               || _T("HKEY_PERFORMANCE_TEXT") == key_name_)
      {
        key_ = HKEY_PERFORMANCE_TEXT;
      }
      else if (_T("HKU") == key_name_
               || _T("HKEY_USERS") == key_name_)
      {
        key_ = HKEY_USERS;
      }
      else
      {
        Exceptions::User e;
        if (key_name_.empty())
        {
          e.setContext(path_);
        }
        else
        {
          e.setContext(key_name_);
        }
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_REGISTRY);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_INVALID_PATH);
        throw e;
      }
    }
  }
}
