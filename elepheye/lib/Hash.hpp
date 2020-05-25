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

#ifndef ELEPHEYE_LIB_HASH_HPP
#define ELEPHEYE_LIB_HASH_HPP

#include <elepheye/String.hpp>
#include <elepheye/lib/Handles.hpp>

namespace elepheye
{
  namespace lib
  {
    /**
     * Class to compute the hash.
     */
    class Hash
    {
    public:

      /**
       * Algorithms.
       */
      enum Algorithm
      {
        ALGORITHM_MD5,
        ALGORITHM_SHA1,
      };

      /**
       * Constructor.
       * @param algo - The algorithm.
       */
      explicit Hash(Algorithm algo);

      /**
       * Add the new data to update the hash.
       * @param data - The data.
       * @param size - The size of data in bytes.
       */
      void update(const unsigned char* data, std::size_t size);

      /**
       * Format the current hash.
       * @return The hash.
       */
      String format() const;

    private:
      Handles::CryptReleaseContext prov_;
      Handles::CryptDestroyHash hash_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_HASH_HPP
