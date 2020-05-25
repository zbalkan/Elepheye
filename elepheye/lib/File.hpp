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

#ifndef ELEPHEYE_LIB_FILE_HPP
#define ELEPHEYE_LIB_FILE_HPP

#include <elepheye/lib/Handles.hpp>
#include <elepheye/resource.h>

namespace elepheye
{
  namespace lib
  {
    /**
     * Class for the file handle.
     */
    class File
    {
    public:

      /**
       * Constructor.
       * @param path - The path.
       * @param desired_access - For ::CreateFile.
       * @param share_mode - For ::CreateFile.
       * @param security_attributes - For ::CreateFile.
       * @param creation_disposition - For ::CreateFile.
       * @param template_file - For ::CreateFile.
       * @param issue - The issue for the exception.
       */
      File(
        const String& path,
        ::DWORD desired_access,
        ::DWORD share_mode,
        ::LPSECURITY_ATTRIBUTES security_attributes,
        ::DWORD creation_disposition,
        ::DWORD flags_and_attributes,
        ::HANDLE template_file,
        ::UINT issue = ELEPHEYE_RESOURCE_IDS_ISSUE_OPEN_FILE);

      /**
       * Get the handle.
       * @return The handle.
       */
      const ::HANDLE& get() const throw();

      /**
       * Get the handle.
       * @return The handle.
       */
      ::HANDLE& get() throw();

      /**
       * Get the path.
       * @return The path.
       */
      const ::TCHAR* getPath() const throw();

    private:
      Handles::CloseHandle file_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_FILE_HPP
