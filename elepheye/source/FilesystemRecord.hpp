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

#ifndef ELEPHEYE_SOURCE_FILESYSTEM_RECORD_HPP
#define ELEPHEYE_SOURCE_FILESYSTEM_RECORD_HPP

#include <elepheye/Record.hpp>
#include <elepheye/lib/Hash.hpp>
#include <elepheye/source/FilesystemPath.hpp>
#include <vector>

namespace elepheye
{
  namespace source
  {
    /**
     * Implementation of Record for Filesystem.
     */
    class FilesystemRecord : public Record
    {
    public:

      /**
       * Index values for the field.
       */
      enum FieldIndex
      {
        FIELD_INDEX_PATH,
        FIELD_INDEX_ATTRIBUTES,
        FIELD_INDEX_CREATION_TIME,
        FIELD_INDEX_LAST_ACCESS_TIME,
        FIELD_INDEX_LAST_WRITE_TIME,
        FIELD_INDEX_SIZE,
        FIELD_INDEX_REPARSE_TAG,
        FIELD_INDEX_REPARSE_PATH,
        FIELD_INDEX_OWNER,
        FIELD_INDEX_GROUP,
        FIELD_INDEX_DACL,
        FIELD_INDEX_SACL,
        FIELD_INDEX_MD5,
        FIELD_INDEX_SHA1,
        FIELD_INDEX_COUNT // Must be the last entry.
      };

      /**
       * Index values for the option.
       */
      enum OptionIndex
      {
        OPTION_INDEX_IGNORE_CHILDREN,
        OPTION_INDEX_ENABLE_LAST_ACCESS_TIME,
        OPTION_INDEX_COUNT // Must be the last entry.
      };

      /**
       * Constructor.
       * @param path - The path.
       */
      explicit FilesystemRecord(const String& path);

      /**
       * Constructor.
       * @param parent - The parent record.
       * @param child - The child name.
       */
      FilesystemRecord(const FilesystemRecord& parent, const String& child);

      /**
       * Constructor.
       * @param parent - The parent record.
       * @param child - The child data.
       */
      FilesystemRecord(
        const FilesystemRecord& parent, const ::WIN32_FIND_DATA& child);

      const String& getField(std::size_t index) const;

      bool getOption(std::size_t index) const;

      void setOption(std::size_t index);

      /**
       * Get whether the record is a directory.
       * @return True if directory.
       */
      bool isDirectory() const;

      /**
       * Get whether the record is a stream.
       * @return True if stream.
       */
      bool isStream() const;

      /**
       * Get whether the record is a reparse point.
       * @return True if reparse point.
       */
      bool isReparsePoint() const;

    private:
      FilesystemRecord(); // Not implemented.
      void initialize(const ::WIN32_FIND_DATA& data);
      void initializeByFindFirstFile();
      void initializeByGetFileAttributesEx();
      void initializeByGetFileInformationByHandle();
      void setField(std::size_t index) const;
      String formatAttributes(::DWORD attributes) const;
      String formatReparseTag(::DWORD tag) const;
      String formatReparsePath(::DWORD tag) const;
      String formatSecurityInfo(::SECURITY_INFORMATION si, ::UINT issue) const;
      String formatHash(lib::Hash::Algorithm algo) const;
      FilesystemPath path_;
      ::WIN32_FILE_ATTRIBUTE_DATA data_;
      ::DWORD reparse_tag_;
      mutable std::vector<String> fields_; // Mutable for getField optimization.
      std::vector<bool> options_;
    };
  }
}

#endif // #ifndef ELEPHEYE_SOURCE_FILESYSTEM_RECORD_HPP
