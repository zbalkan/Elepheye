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
#include <elepheye/source/Filesystem.hpp>
#include <elepheye/source/FilesystemScanner.hpp>

namespace elepheye
{
  namespace source
  {
    Filesystem::Filesystem(const std::vector<String>& paths)
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
        values_.push_back(FilesystemRecord(*i));
      }

      // Sort records.
      values_.sort(lib::RecordComparator());
    }

    bool Filesystem::input()
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

      // Find streams of the last record.
      std::list<FilesystemRecord> streams;
      try
      {
        FilesystemScanner::findStreams(values_.front(), streams);
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
      if (!streams.empty())
      {
        streams.sort(lib::RecordComparator());
        values_.merge(streams, lib::RecordComparator());
      }

      // Find contents of the last record.
      std::list<FilesystemRecord> contents;
      try
      {
        FilesystemScanner::findContents(values_.front(), contents);
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
      if (!contents.empty())
      {
        contents.sort(lib::RecordComparator());
        values_.merge(contents, lib::RecordComparator());
      }

      // Remove the last record.
      values_.pop_front();
      return !values_.empty();
    }

    const Filesystem::ValueType& Filesystem::get() const
    {
      return values_.front();
    }

    Filesystem::ValueType& Filesystem::get()
    {
      return values_.front();
    }

    const Filesystem::NameType& Filesystem::getName() const
    {
      return name_;
    }

    void Filesystem::createName()
    {
      // Set field names.
      name_.getFieldNames().resize(FilesystemRecord::FIELD_INDEX_COUNT);
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_PATH)
        =_T("path");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_ATTRIBUTES)
        =_T("attributes");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_CREATION_TIME)
        =_T("creation_time");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_LAST_ACCESS_TIME)
        =_T("last_access_time");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_LAST_WRITE_TIME)
        =_T("last_write_time");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_SIZE)
        =_T("size");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_REPARSE_TAG)
        =_T("reparse_tag");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_REPARSE_PATH)
        =_T("reparse_path");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_OWNER)
        =_T("owner");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_GROUP)
        =_T("group");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_DACL)
        =_T("dacl");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_SACL)
        =_T("sacl");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_MD5)
        =_T("md5");
      name_.getFieldNames().at(FilesystemRecord::FIELD_INDEX_SHA1)
        =_T("sha1");

      // Set option names.
      name_.getOptionNames().resize(FilesystemRecord::OPTION_INDEX_COUNT);
      name_.getOptionNames().at(FilesystemRecord::OPTION_INDEX_IGNORE_CHILDREN)
        =_T("ignore_children");
      name_.getOptionNames().at(
        FilesystemRecord::OPTION_INDEX_ENABLE_LAST_ACCESS_TIME)
        =_T("enable_last_access_time");
    }
  }
}
