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

#ifndef ELEPHEYE_SOURCE_REGISTRY_RECORD_HPP
#define ELEPHEYE_SOURCE_REGISTRY_RECORD_HPP

#include <elepheye/Record.hpp>
#include <elepheye/lib/Hash.hpp>
#include <elepheye/source/RegistryPath.hpp>
#include <vector>

namespace elepheye
{
  namespace source
  {
    /**
     * Implementation of Record for Registry.
     */
    class RegistryRecord : public Record
    {
    public:

      /**
       * Index values for the field.
       */
      enum FieldIndex
      {
        FIELD_INDEX_PATH,
        FIELD_INDEX_CLASS,
        FIELD_INDEX_LAST_WRITE_TIME,
        FIELD_INDEX_TYPE,
        FIELD_INDEX_SIZE,
        FIELD_INDEX_OWNER,
        FIELD_INDEX_GROUP,
        FIELD_INDEX_DACL,
        FIELD_INDEX_SACL,
        FIELD_INDEX_MD5,
        FIELD_INDEX_SHA1,
        FIELD_INDEX_COUNT // Must be last entry.
      };

      /**
       * Index values for the option.
       */
      enum OptionIndex
      {
        OPTION_INDEX_IGNORE_CHILDREN,
        OPTION_INDEX_COUNT // Must be the last entry.
      };

      /**
       * Constructor.
       * @param path - The path.
       */
      explicit RegistryRecord(const String& path);

      /**
       * Constructor.
       * @param parent - The parent record.
       * @param child - The child name to append.
       * @param is_key - True if the child is a key.
       */
      RegistryRecord(
        const RegistryRecord& parent, const String& child, bool is_key);

      const String& getField(std::size_t index) const;

      bool getOption(std::size_t index) const;

      void setOption(std::size_t index);

      /**
       * Get the path.
       * @return The path.
       */
      const RegistryPath& getPath() const;

    private:
      RegistryRecord(); // Not implemented.
      void setField(std::size_t index) const;
      String formatClass() const;
      String formatLastWriteTime() const;
      String formatType() const;
      String formatSize() const;
      String formatSecurityInfo(::SECURITY_INFORMATION si, ::UINT issue) const;
      String formatHash(lib::Hash::Algorithm algo) const;
      RegistryPath path_;
      mutable std::vector<String> fields_; // Mutable for getField optimization.
      std::vector<bool> options_;
    };
  }
}

#endif // #ifndef ELEPHEYE_SOURCE_REGISTRY_RECORD_HPP
