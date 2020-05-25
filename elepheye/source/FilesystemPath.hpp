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

#ifndef ELEPHEYE_SOURCE_FILESYSTEM_PATH_HPP
#define ELEPHEYE_SOURCE_FILESYSTEM_PATH_HPP

#include <elepheye/String.hpp>

namespace elepheye
{
  namespace source
  {
    /**
     * Class for the filesystem path.
     * The path is converted to the extended-length path with the `\\?\' prefix.
     */
    class FilesystemPath
    {
    public:

      /**
       * Constructor.
       * @param path - The path.
       */
      explicit FilesystemPath(const String& path);

      /**
       * Constructor.
       * @param parent - The parent path.
       * @param child - The child name to append.
       */
      FilesystemPath(const FilesystemPath& parent, const String& child);

      /**
       * Get the path.
       * @return The path.
       */
      const String& get() const;

      /**
       * Get whether the path is a stream.
       * @return True if stream.
       */
      bool isStream() const;

    private:
      void toExtended(const String& path);
      String path_;
    };
  }
}

#endif // #ifndef ELEPHEYE_SOURCE_FILESYSTEM_PATH_HPP
