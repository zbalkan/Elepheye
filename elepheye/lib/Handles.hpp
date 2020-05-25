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

#ifndef ELEPHEYE_LIB_HANDLES_HPP
#define ELEPHEYE_LIB_HANDLES_HPP

#include <elepheye/lib/Handle.hpp>
#include <oleauto.h>
#include <wincrypt.h>

namespace elepheye
{
  namespace lib
  {
    /**
     * Implementations of Handle.
     */
    class Handles
    {
    public:

      /**
       * Implementation of Handle for ::BackupRead.
       */
      class BackupRead : public Handle<::LPVOID>
      {
      public:
        BackupRead() throw();
        ~BackupRead() throw();
        const ::HANDLE& getFile() const throw();
        ::HANDLE& getFile() throw();
      private:
        ::HANDLE file_;
      };

      /**
       * Implementation of Handle for ::CloseHandle.
       */
      class CloseHandle : public Handle<::HANDLE>
      {
      public:
        CloseHandle() throw();
        ~CloseHandle() throw();
      };

      /**
       * Implementation of Handle for ::CryptDestroyHash.
       */
      class CryptDestroyHash : public Handle<::HCRYPTHASH>
      {
      public:
        CryptDestroyHash() throw();
        ~CryptDestroyHash() throw();
      };

      /**
       * Implementation of Handle for ::CryptReleaseContext.
       */
      class CryptReleaseContext : public Handle<::HCRYPTPROV>
      {
      public:
        CryptReleaseContext() throw();
        ~CryptReleaseContext() throw();
      };

      /**
       * Implementation of Handle for ::FindClose.
       */
      class FindClose : public Handle<::HANDLE>
      {
      public:
        FindClose() throw();
        ~FindClose() throw();
      };

      /**
       * Implementation of Handle for ::LocalFree.
       */
      class LocalFree : public Handle<::HLOCAL>
      {
      public:
        LocalFree() throw();
        ~LocalFree() throw();
      };

      /**
       * Implementation of Handle for ::RegCloseKey.
       */
      class RegCloseKey : public Handle<::HKEY>
      {
      public:
        RegCloseKey() throw();
        ~RegCloseKey() throw();
      };

      /**
       * Implementation of Handle for ::SafeArrayDestroy.
       */
      class SafeArrayDestroy : public Handle<::SAFEARRAY*>
      {
      public:
        SafeArrayDestroy() throw();
        ~SafeArrayDestroy() throw();
      };

      /**
       * Implementation of Handle for ::SafeArrayUnaccessData.
       */
      class SafeArrayUnaccessData : public Handle<::SAFEARRAY*>
      {
      public:
        SafeArrayUnaccessData() throw();
        ~SafeArrayUnaccessData() throw();
      };
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_HANDLES_HPP
