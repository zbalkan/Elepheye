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

#ifndef ELEPHEYE_LIB_REGISTRY_KEY_HPP
#define ELEPHEYE_LIB_REGISTRY_KEY_HPP

#include <elepheye/lib/Handles.hpp>
#include <elepheye/resource.h>

namespace elepheye
{
  namespace lib
  {
    /**
     * Class for the registry key handle.
     */
    class RegistryKey
    {
    public:

      /**
       * Constructor.
       * @param machine_name - For ::RegConnectRegistry.
       * @param key - For ::RegConnectRegistry or ::RegOpenKeyEx.
       * @param sub_key - For ::RegOpenKeyEx.
       * @param options - For ::RegOpenKeyEx.
       * @param sam_desired - For ::RegOpenKeyEx.
       * @param path - The path for the name.
       * @param issue - The issue for the exception.
       */
      RegistryKey(
        const String& machine_name,
        ::HKEY key,
        const String& sub_key,
        ::DWORD options,
        ::REGSAM sam_desired,
        const String& path,
        ::UINT issue = ELEPHEYE_RESOURCE_IDS_ISSUE_OPEN_REGISTRY_KEY);

      /**
       * Get the handle.
       * @return The handle.
       */
      const ::HKEY& get() const throw();

      /**
       * Get the handle.
       * @return The handle.
       */
      ::HKEY& get() throw();

      /**
       * Get the path.
       * @return The path.
       */
      const ::TCHAR* getPath() const throw();

    private:
      Handles::RegCloseKey key_remote_;
      Handles::RegCloseKey key_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_REGISTRY_KEY_HPP
