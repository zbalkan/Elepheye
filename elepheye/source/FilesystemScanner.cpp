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

#include <cwchar>
#include <elepheye/ExceptionProxy.hpp>
#include <elepheye/Exceptions.hpp>
#include <elepheye/lib/File.hpp>
#include <elepheye/lib/Handles.hpp>
#include <elepheye/source/FilesystemScanner.hpp>
#include <vector>

namespace elepheye
{
  namespace source
  {
    namespace
    {
      bool fine_next_stream(
        const FilesystemRecord& parent,
        std::list<FilesystemRecord>& streams,
        ::HANDLE file,
        ::LPVOID& context)
      {
        // Read the stream header.
        ::WIN32_STREAM_ID id;
        ::SecureZeroMemory(&id, sizeof(id));
        const ::DWORD header_size = offsetof(::WIN32_STREAM_ID, cStreamName);
        ::DWORD read_size = 0;
        if (!::BackupRead(
              file,
              reinterpret_cast<::LPBYTE>(&id),
              header_size,
              &read_size,
              FALSE,
              FALSE,
              &context))
        {
          const ::DWORD error = ::GetLastError();
          Exceptions::System e;
          e.setContext(parent.getField(0));
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_FILE_STREAMS);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }

        // No more streams.
        if (!read_size)
        {
          return false;
        }

        // Read the stream name.
        if (id.dwStreamNameSize)
        {
          if (id.dwStreamNameSize % sizeof(::WCHAR))
          {
            Exceptions::System e;
            e.setContext(parent.getField(0));
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_FILE_STREAMS);
            e.setSource(_T(__FILE__), __LINE__);
            throw e;
          }
          std::vector<::WCHAR> name(id.dwStreamNameSize / sizeof(::WCHAR), 0);
          read_size = 0;
          if (!::BackupRead(
                file,
                reinterpret_cast<::LPBYTE>(&name.at(0)),
                id.dwStreamNameSize,
                &read_size,
                FALSE,
                FALSE,
                &context))
          {
            const ::DWORD error = ::GetLastError();
            Exceptions::System e;
            e.setContext(parent.getField(0));
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_FILE_STREAMS);
            e.setWin32(error);
            e.setSource(_T(__FILE__), __LINE__);
            throw e;
          }
          if (id.dwStreamNameSize != read_size)
          {
            Exceptions::System e;
            e.setContext(parent.getField(0));
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_FILE_STREAMS);
            e.setSource(_T(__FILE__), __LINE__);
            throw e;
          }
          if (!name.at(0))
          {
            Exceptions::System e;
            e.setContext(parent.getField(0));
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_FILE_STREAMS);
            e.setSource(_T(__FILE__), __LINE__);
            throw e;
          }
          // Add the new record.
          try
          {
            streams.push_back(
              FilesystemRecord(parent, String(&name.at(0), name.size())));
          }
          catch (const Exceptions::System& e)
          {
            // Log system errors.
            ExceptionProxy::log(e);
          }
        }

        // Skip the stream body.
        if (id.Size.LowPart || id.Size.HighPart)
        {
          ::DWORD seeked_size_low = 0;
          ::DWORD seeked_size_high = 0;
          if (!::BackupSeek(
                file,
                id.Size.LowPart,
                id.Size.HighPart,
                &seeked_size_low,
                &seeked_size_high,
                &context))
          {
            const ::DWORD error = ::GetLastError();
            Exceptions::System e;
            e.setContext(parent.getField(0));
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_FILE_STREAMS);
            e.setWin32(error);
            e.setSource(_T(__FILE__), __LINE__);
            throw e;
          }
          if (id.Size.LowPart != seeked_size_low
              || id.Size.HighPart != static_cast<::LONG>(seeked_size_high))
          {
            Exceptions::System e;
            e.setContext(parent.getField(0));
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_FILE_STREAMS);
            e.setSource(_T(__FILE__), __LINE__);
            throw e;
          }
        }
        return true;
      }
    }

    void FilesystemScanner::findStreams(
      const FilesystemRecord& parent, std::list<FilesystemRecord>& streams)
    {
      // Clear records.
      streams.clear();

      // Do nothing if the parent is a stream.
      if (parent.isStream())
      {
        return;
      }

      // Do nothing if ignored.
      if (parent.getOption(FilesystemRecord::OPTION_INDEX_IGNORE_CHILDREN))
      {
        return;
      }

      // Open the file.
      // ACCESS_SYSTEM_SECURITY is not required because BackupRead skips ACLs.
      lib::File file(
        parent.getField(0),
        GENERIC_READ,
        FILE_SHARE_READ,
        0,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        0,
        ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_FILE_STREAMS);

      // Find streams.
      lib::Handles::BackupRead context;
      context.getFile() = file.get();
      while (fine_next_stream(parent, streams, file.get(), context.get()))
      {
      }
    }

    void FilesystemScanner::findContents(
      const FilesystemRecord& parent, std::list<FilesystemRecord>& contents)
    {
      // Clear records.
      contents.clear();

      // Do nothing if the parent is not a directory.
      if (!parent.isDirectory())
      {
        return;
      }

      // Do nothing if the parent is a reparse point.
      if (parent.isReparsePoint())
      {
        return;
      }

      // Do nothing if ignored.
      if (parent.getOption(FilesystemRecord::OPTION_INDEX_IGNORE_CHILDREN))
      {
        return;
      }

      // Make the pattern to find contents.
      String pattern(parent.getField(0));
      const ::TCHAR sep = _T('\\');
      if (pattern.at(pattern.length() - 1) != sep)
      {
        pattern += sep;
      }
      pattern += _T('*');

      // Open the find handle.
      ::WIN32_FIND_DATA data;
      ::SecureZeroMemory(&data, sizeof(data));
      lib::Handles::FindClose find;
      find.get() = ::FindFirstFile(pattern.c_str(), &data);
      if (INVALID_HANDLE_VALUE == find.get())
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setContext(pattern);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_DIRECTORY_CONTENTS);
        e.setWin32(error);
        throw e;
      }
      find.setName(pattern);

      // Find contents.
      do
      {
        if (!data.cFileName[0])
        {
          Exceptions::System e;
          e.setContext(pattern);
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_DIRECTORY_CONTENTS);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
        if (::_tcscmp(_T("."), data.cFileName)
            && ::_tcscmp(_T(".."), data.cFileName))
        {
          // Add the new record.
          try
          {
            contents.push_back(FilesystemRecord(parent, data));
          }
          catch (const Exceptions::System& e)
          {
            // Log system errors.
            ExceptionProxy::log(e);
          }
        }
        ::SecureZeroMemory(&data, sizeof(data));
      }
      while (::FindNextFile(find.get(), &data));
      const ::DWORD error = ::GetLastError();
      if (ERROR_NO_MORE_FILES != error)
      {
        Exceptions::System e;
        e.setContext(pattern);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_FIND_DIRECTORY_CONTENTS);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
    }
  }
}
