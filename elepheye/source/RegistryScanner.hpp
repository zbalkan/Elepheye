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

#ifndef ELEPHEYE_SOURCE_REGISTRY_SCANNER_HPP
#define ELEPHEYE_SOURCE_REGISTRY_SCANNER_HPP

#include <elepheye/source/RegistryRecord.hpp>
#include <list>

namespace elepheye
{
  namespace source
  {
    /**
     * Static class to scan the registry.
     */
    class RegistryScanner
    {
    public:

      /**
       * Find sub keys of the parent record.
       * @param parent - The parent record.
       * @param sub_keys - The sub keys.
       */
      static void findSubKeys(
        const RegistryRecord& parent, std::list<RegistryRecord>& sub_keys);

      /**
       * Find values of the parent record.
       * @param parent - The parent record.
       * @param values - The values.
       */
      static void findValues(
        const RegistryRecord& parent, std::list<RegistryRecord>& values);
    };
  }
}

#endif // #ifndef ELEPHEYE_SOURCE_REGISTRY_SCANNER_HPP
