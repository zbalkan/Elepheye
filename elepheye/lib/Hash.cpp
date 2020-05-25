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
#include <elepheye/lib/Hash.hpp>
#include <iomanip>
#include <sstream>
#include <vector>

namespace elepheye
{
  namespace lib
  {
    Hash::Hash(Algorithm algo)
    {
      // Select the algorithm ID for the implementation.
      ::ALG_ID algo_impl = 0;
      switch (algo)
      {
      case ALGORITHM_MD5:
        algo_impl = CALG_MD5;
        break;
      case ALGORITHM_SHA1:
        algo_impl = CALG_SHA1;
        break;
      default:
        {
          Exceptions::Application e;
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_COMPUTE_HASH);
          e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_INVALID_ARGUMENT);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
      }

      // Get the provider.
      if (!::CryptAcquireContext(
            &prov_.get(), 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_COMPUTE_HASH);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Create the hash.
      if (!::CryptCreateHash(prov_.get(), algo_impl, 0, 0, &hash_.get()))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_COMPUTE_HASH);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
    }

    void Hash::update(const unsigned char* data, std::size_t size)
    {
      // Add the data.
      if (size)
      {
        if (!::CryptHashData(hash_.get(), data, static_cast<::DWORD>(size), 0))
        {
          const ::DWORD error = ::GetLastError();
          Exceptions::System e;
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_COMPUTE_HASH);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
      }
    }

    String Hash::format() const
    {
      // Get the hash size.
      ::DWORD size = 0;
      if (!::CryptGetHashParam(hash_.get(), HP_HASHVAL, 0, &size, 0))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_COMPUTE_HASH);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Get the hash value.
      std::vector<::BYTE> value(size);
      if (!::CryptGetHashParam(hash_.get(), HP_HASHVAL, &value.at(0), &size, 0))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_COMPUTE_HASH);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Format the hash to the string.
      std::basic_ostringstream<::TCHAR> s;
      for (
        std::vector<::BYTE>::const_iterator i = value.begin();
        value.end() != i;
        ++i)
      {
        s << std::hex
          << std::right
          << std::setfill(_T('0'))
          << std::setw(2)
          << *i;
      }
      return s.str();
    }
  }
}
