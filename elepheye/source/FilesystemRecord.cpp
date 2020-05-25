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
#include <elepheye/lib/File.hpp>
#include <elepheye/lib/FileInput.hpp>
#include <elepheye/lib/Handles.hpp>
#include <elepheye/source/FilesystemRecord.hpp>

// Confirm recent definitions.
#ifndef FILE_ATTRIBUTE_VIRTUAL
#define FILE_ATTRIBUTE_VIRTUAL (0x00010000)
#endif
#ifndef IO_REPARSE_TAG_DFSR
#define IO_REPARSE_TAG_DFSR (0x80000012)
#endif
#ifndef IO_REPARSE_TAG_DRIVER_EXTENDER
#define IO_REPARSE_TAG_DRIVER_EXTENDER (0x80000005)
#endif
#ifndef IO_REPARSE_TAG_FILTER_MANAGER
#define IO_REPARSE_TAG_FILTER_MANAGER (0x8000000B)
#endif
#ifndef IO_REPARSE_TAG_HSM2
#define IO_REPARSE_TAG_HSM2 (0x80000006)
#endif
#ifndef IO_REPARSE_TAG_SYMLINK
#define IO_REPARSE_TAG_SYMLINK (0xA000000C)
#endif

namespace elepheye
{
  namespace source
  {
    namespace
    {
      // Structures to read the reparse data.
      struct ReparseDataBufferHeader
      {
        ::ULONG tag;
        ::USHORT size;
        ::USHORT reserved;
      };
      struct MountPointReparseBuffer
      {
        ReparseDataBufferHeader header;
        ::USHORT substitute_name_offset;
        ::USHORT substitute_name_size;
        ::USHORT print_name_offset;
        ::USHORT print_name_size;
        ::WCHAR path[1];
      };
      struct SymbolicLinkReparseBuffer
      {
        ReparseDataBufferHeader header;
        ::USHORT substitute_name_offset;
        ::USHORT substitute_name_size;
        ::USHORT print_name_offset;
        ::USHORT print_name_size;
        ::ULONG flags;
        ::WCHAR path[1];
      };
    }

    FilesystemRecord::FilesystemRecord(const String& path)
      : path_(path),
        reparse_tag_(0),
        fields_(FIELD_INDEX_COUNT),
        options_(OPTION_INDEX_COUNT, false)
    {
      // Initialize the data.
      ::SecureZeroMemory(&data_, sizeof(data_));
      try
      {
        initializeByFindFirstFile();
      }
      catch (const Exceptions::System&)
      {
        // Ignore system errors.
      }
      try
      {
        initializeByGetFileInformationByHandle();
        return;
      }
      catch (const Exceptions::System&)
      {
        // Ignore system errors.
      }
      initializeByGetFileAttributesEx();
    }

    FilesystemRecord::FilesystemRecord(
      const FilesystemRecord& parent, const String& child)
      : path_(parent.path_, child),
        reparse_tag_(0),
        fields_(FIELD_INDEX_COUNT),
        options_(OPTION_INDEX_COUNT, false)
    {
      // Initialize the data.
      ::SecureZeroMemory(&data_, sizeof(data_));
      try
      {
        initializeByFindFirstFile();
      }
      catch (const Exceptions::System&)
      {
        // Ignore system errors.
      }
      try
      {
        initializeByGetFileInformationByHandle();
        return;
      }
      catch (const Exceptions::System&)
      {
        // Ignore system errors.
      }
      initializeByGetFileAttributesEx();
    }

    FilesystemRecord::FilesystemRecord(
      const FilesystemRecord& parent, const ::WIN32_FIND_DATA& child)
      : path_(parent.path_, child.cFileName),
        reparse_tag_(0),
        fields_(FIELD_INDEX_COUNT),
        options_(OPTION_INDEX_COUNT, false)
    {
      // Initialize the data.
      ::SecureZeroMemory(&data_, sizeof(data_));
      initialize(child);
      try
      {
        initializeByGetFileInformationByHandle();
        return;
      }
      catch (const Exceptions::System&)
      {
        // Ignore system errors.
      }
    }

    const String& FilesystemRecord::getField(std::size_t index) const
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

    bool FilesystemRecord::getOption(std::size_t index) const
    {
      return options_.at(index);
    }

    void FilesystemRecord::setOption(std::size_t index)
    {
      options_.at(index) = true;
    }

    bool FilesystemRecord::isDirectory() const
    {
      return FILE_ATTRIBUTE_DIRECTORY & data_.dwFileAttributes ? true : false;
    }

    bool FilesystemRecord::isStream() const
    {
      return path_.isStream();
    }

    bool FilesystemRecord::isReparsePoint() const
    {
      return
        FILE_ATTRIBUTE_REPARSE_POINT & data_.dwFileAttributes ? true : false;
    }

    void FilesystemRecord::initialize(const ::WIN32_FIND_DATA& data)
    {
      // Save the data.
      ::CopyMemory(&data_, &data, sizeof(data_));
      if (isReparsePoint())
      {
        reparse_tag_ = data.dwReserved0;
      }
    }

    void FilesystemRecord::initializeByFindFirstFile()
    {
      // Get the data.
      ::WIN32_FIND_DATA data;
      ::SecureZeroMemory(&data, sizeof(data));
      lib::Handles::FindClose find;
      find.get() = ::FindFirstFile(path_.get().c_str(), &data);
      if (INVALID_HANDLE_VALUE == find.get())
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILESYSTEM);
        e.setWin32(error);
        throw e;
      }
      find.setName(path_.get());

      // Save the data.
      initialize(data);
    }

    void FilesystemRecord::initializeByGetFileAttributesEx()
    {
      // Save the data.
      if (!::GetFileAttributesEx(
            path_.get().c_str(), ::GetFileExInfoStandard, &data_))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILESYSTEM);
        e.setWin32(error);
        throw e;
      }
    }

    void FilesystemRecord::initializeByGetFileInformationByHandle()
    {
      // Open the file.
      lib::File file(
        path_.get(),
        GENERIC_READ,
        FILE_SHARE_READ,
        0,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        0,
        ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILESYSTEM);

      // Get the data.
      ::BY_HANDLE_FILE_INFORMATION info;
      ::SecureZeroMemory(&info, sizeof(info));
      if (!::GetFileInformationByHandle(file.get(), &info))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILESYSTEM);
        e.setWin32(error);
        throw e;
      }

      // Save the data.
      data_.dwFileAttributes = info.dwFileAttributes;
      data_.ftCreationTime = info.ftCreationTime;
      data_.ftLastAccessTime = info.ftLastAccessTime;
      data_.ftLastWriteTime = info.ftLastWriteTime;
      data_.nFileSizeHigh = info.nFileSizeHigh;
      data_.nFileSizeLow = info.nFileSizeLow;
    }

    void FilesystemRecord::setField(std::size_t index) const
    {
      switch (index)
      {
      case FIELD_INDEX_ATTRIBUTES:
        fields_.at(index) = formatAttributes(data_.dwFileAttributes);
        break;
      case FIELD_INDEX_CREATION_TIME:
        if (!isStream())
        {
          fields_.at(index) = lib::Field::format(data_.ftCreationTime);
        }
        break;
      case FIELD_INDEX_LAST_ACCESS_TIME:
        if (!isStream() && getOption(OPTION_INDEX_ENABLE_LAST_ACCESS_TIME))
        {
          fields_.at(index) = lib::Field::format(data_.ftLastAccessTime);
        }
        break;
      case FIELD_INDEX_LAST_WRITE_TIME:
        if (!isStream())
        {
          fields_.at(index) = lib::Field::format(data_.ftLastWriteTime);
        }
        break;
      case FIELD_INDEX_SIZE:
        if (!isDirectory())
        {
          fields_.at(index) = lib::Field::format<::ULONGLONG>(
            data_.nFileSizeHigh, data_.nFileSizeLow);
        }
        break;
      case FIELD_INDEX_REPARSE_TAG:
        if (!isStream() && isReparsePoint())
        {
          fields_.at(index) = formatReparseTag(reparse_tag_);
        }
        break;
      case FIELD_INDEX_REPARSE_PATH:
        if (!isStream() && isReparsePoint())
        {
          fields_.at(index) = formatReparsePath(reparse_tag_);
        }
        break;
      case FIELD_INDEX_OWNER:
        if (!isStream())
        {
          fields_.at(index) = formatSecurityInfo(
            OWNER_SECURITY_INFORMATION, ELEPHEYE_RESOURCE_IDS_ISSUE_READ_OWNER);
        }
        break;
      case FIELD_INDEX_GROUP:
        if (!isStream())
        {
          fields_.at(index) = formatSecurityInfo(
            GROUP_SECURITY_INFORMATION, ELEPHEYE_RESOURCE_IDS_ISSUE_READ_GROUP);
        }
        break;
      case FIELD_INDEX_DACL:
        if (!isStream())
        {
          fields_.at(index) = formatSecurityInfo(
            DACL_SECURITY_INFORMATION, ELEPHEYE_RESOURCE_IDS_ISSUE_READ_DACL);
        }
        break;
      case FIELD_INDEX_SACL:
        if (!isStream())
        {
          fields_.at(index) = formatSecurityInfo(
            SACL_SECURITY_INFORMATION, ELEPHEYE_RESOURCE_IDS_ISSUE_READ_SACL);
        }
        break;
      case FIELD_INDEX_MD5:
        if (!isDirectory())
        {
          fields_.at(index) = formatHash(lib::Hash::ALGORITHM_MD5);
        }
        break;
      case FIELD_INDEX_SHA1:
        if (!isDirectory())
        {
          fields_.at(index) = formatHash(lib::Hash::ALGORITHM_SHA1);
        }
        break;
      }
    }

    String FilesystemRecord::formatAttributes(::DWORD attributes) const
    {
      String result = lib::Field::formatToHex(attributes);
      if (FILE_ATTRIBUTE_ARCHIVE & attributes)
      {
        result += _T(":ARCHIVE");
      }
      if (FILE_ATTRIBUTE_COMPRESSED & attributes)
      {
        result += _T(":COMPRESSED");
      }
      if (FILE_ATTRIBUTE_DEVICE & attributes)
      {
        result += _T(":DEVICE");
      }
      if (FILE_ATTRIBUTE_DIRECTORY & attributes)
      {
        result += _T(":DIRECTORY");
      }
      if (FILE_ATTRIBUTE_ENCRYPTED & attributes)
      {
        result += _T(":ENCRYPTED");
      }
      if (FILE_ATTRIBUTE_HIDDEN & attributes)
      {
        result += _T(":HIDDEN");
      }
      if (FILE_ATTRIBUTE_NORMAL & attributes)
      {
        result += _T(":NORMAL");
      }
      if (FILE_ATTRIBUTE_NOT_CONTENT_INDEXED & attributes)
      {
        result += _T(":NOT_CONTENT_INDEXED");
      }
      if (FILE_ATTRIBUTE_OFFLINE & attributes)
      {
        result += _T(":OFFLINE");
      }
      if (FILE_ATTRIBUTE_READONLY & attributes)
      {
        result += _T(":READONLY");
      }
      if (FILE_ATTRIBUTE_REPARSE_POINT & attributes)
      {
        result += _T(":REPARSE_POINT");
      }
      if (FILE_ATTRIBUTE_SPARSE_FILE & attributes)
      {
        result += _T(":SPARSE_FILE");
      }
      if (FILE_ATTRIBUTE_SYSTEM & attributes)
      {
        result += _T(":SYSTEM");
      }
      if (FILE_ATTRIBUTE_TEMPORARY & attributes)
      {
        result += _T(":TEMPORARY");
      }
      if (FILE_ATTRIBUTE_VIRTUAL & attributes)
      {
        result += _T(":VIRTUAL");
      }
      return result;
    }

    String FilesystemRecord::formatReparseTag(::DWORD tag) const
    {
      switch (tag)
      {
      case IO_REPARSE_TAG_DFS:
        return _T("DFS");
      case IO_REPARSE_TAG_DFSR:
        return _T("DFSR");
      case IO_REPARSE_TAG_DRIVER_EXTENDER:
        return _T("DRIVER_EXTENDER");
      case IO_REPARSE_TAG_FILTER_MANAGER:
        return _T("FILTER_MANAGER");
      case IO_REPARSE_TAG_HSM:
        return _T("HSM");
      case IO_REPARSE_TAG_HSM2:
        return _T("HSM2");
      case IO_REPARSE_TAG_MOUNT_POINT:
        return _T("MOUNT_POINT");
      case IO_REPARSE_TAG_SIS:
        return _T("SIS");
      case IO_REPARSE_TAG_SYMLINK:
        return _T("SYMLINK");
      default:
        return lib::Field::formatToHex(tag);
      }
    }

    String FilesystemRecord::formatReparsePath(::DWORD tag) const
    {
      // Do nothing if unknown tag.
      String result;
      if (IO_REPARSE_TAG_MOUNT_POINT != tag && IO_REPARSE_TAG_SYMLINK != tag)
      {
        return result;
      }

      // Open the reparse point.
      lib::File file(
        path_.get(),
        GENERIC_READ,
        FILE_SHARE_READ,
        0,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        0,
        ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILESYSTEM);

      // Read the data.
      ::BYTE data[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
      ::SecureZeroMemory(data, sizeof(data));
      ::DWORD returned_size = 0;
      if (!::DeviceIoControl(
            file.get(),
            FSCTL_GET_REPARSE_POINT,
            0,
            0,
            data,
            sizeof(data),
            &returned_size,
            0))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILESYSTEM);
        e.setWin32(error);
        throw e;
      }
      if (sizeof(data) < returned_size)
      {
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILESYSTEM);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Retrieve the path.
      switch (tag)
      {
      case IO_REPARSE_TAG_MOUNT_POINT:
        {
          const MountPointReparseBuffer* buffer
            = reinterpret_cast<const MountPointReparseBuffer*>(data);
          if (
            sizeof(MountPointReparseBuffer) > returned_size
            || offsetof(MountPointReparseBuffer, path)
            + buffer->substitute_name_offset
            + buffer->substitute_name_size > returned_size)
          {
            Exceptions::System e;
            e.setContext(path_.get());
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILESYSTEM);
            e.setSource(_T(__FILE__), __LINE__);
            throw e;
          }
          result = String(
            &buffer->path[buffer->substitute_name_offset / sizeof(::WCHAR)],
            buffer->substitute_name_size / sizeof(::WCHAR));
          break;
        }
      case IO_REPARSE_TAG_SYMLINK:
        {
          const SymbolicLinkReparseBuffer* buffer
            = reinterpret_cast<const SymbolicLinkReparseBuffer*>(data);
          if (
            sizeof(SymbolicLinkReparseBuffer) > returned_size
            || offsetof(SymbolicLinkReparseBuffer, path)
            + buffer->substitute_name_offset
            + buffer->substitute_name_size > returned_size)
          {
            Exceptions::System e;
            e.setContext(path_.get());
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILESYSTEM);
            e.setSource(_T(__FILE__), __LINE__);
            throw e;
          }
          result = String(
            &buffer->path[buffer->substitute_name_offset / sizeof(::WCHAR)],
            buffer->substitute_name_size / sizeof(::WCHAR));
          break;
        }
      }
      return result;
    }

    String FilesystemRecord::formatSecurityInfo(
      ::SECURITY_INFORMATION si, ::UINT issue) const
    {
      // Get the size of the security descriptor.
      ::DWORD needed_size = 0;
      if (!::GetFileSecurity(path_.get().c_str(), si, 0, 0, &needed_size))
      {
        const ::DWORD error = ::GetLastError();
        if (ERROR_INSUFFICIENT_BUFFER != error)
        {
          Exceptions::System e;
          e.setContext(path_.get());
          e.setIssue(issue);
          e.setWin32(error);
          throw e;
        }
      }
      if (!needed_size)
      {
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(issue);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Get the security descriptor.
      std::vector<::BYTE> buffer(needed_size, 0);
      ::SECURITY_DESCRIPTOR* sd
        = reinterpret_cast<::SECURITY_DESCRIPTOR*>(&buffer.at(0));
      if (!::GetFileSecurity(
            path_.get().c_str(),
            si,
            sd,
            static_cast<::DWORD>(buffer.size()),
            &needed_size))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(issue);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
      if (buffer.size() < needed_size)
      {
        Exceptions::System e;
        e.setContext(path_.get());
        e.setIssue(issue);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Format.
      return lib::Field::format(sd, si, issue);
    }

    String FilesystemRecord::formatHash(lib::Hash::Algorithm algo) const
    {
      // Open the file.
      lib::FileInput<std::vector<::BYTE> > file(
        Shared<lib::File>(
          new lib::File(
            path_.get(),
            GENERIC_READ,
            FILE_SHARE_READ,
            0,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS
            | FILE_FLAG_OPEN_REPARSE_POINT
            | FILE_FLAG_SEQUENTIAL_SCAN,
            0,
            ELEPHEYE_RESOURCE_IDS_ISSUE_COMPUTE_HASH)));

      // Read the file and compute the hash.
      file.get().resize(0x1000);
      lib::Hash hash(algo);
      while (file.input())
      {
        hash.update(&file.get().at(0), file.get().size());
      }

      // Format.
      return hash.format();
    }
  }
}
