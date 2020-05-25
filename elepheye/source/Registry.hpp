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

#ifndef ELEPHEYE_SOURCE_REGISTRY_HPP
#define ELEPHEYE_SOURCE_REGISTRY_HPP

#include <elepheye/Input.hpp>
#include <elepheye/Record.hpp>
#include <elepheye/RecordName.hpp>
#include <elepheye/lib/Privilege.hpp>
#include <elepheye/source/RegistryRecord.hpp>
#include <list>

namespace elepheye
{
  namespace source
  {
    /**
     * Implementation of Input for the registry.
     */
    class Registry : public Input<Record, RecordName>
    {
    public:

      /**
       * Constructor.
       * @param paths - The paths to the registry.
       */
      explicit Registry(const std::vector<String>& paths);

      bool input();

      const ValueType& get() const;

      ValueType& get();

      const NameType& getName() const;

    private:
      void createName();
      std::list<RegistryRecord> values_;
      NameType name_;
      bool is_first_input_;
      lib::Privilege backup_privilege;
      lib::Privilege security_privilege;
    };
  }
}

#endif // #ifndef ELEPHEYE_SOURCE_REGISTRY_HPP
