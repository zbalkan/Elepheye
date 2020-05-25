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

#ifndef ELEPHEYE_SOURCE_WMI_HPP
#define ELEPHEYE_SOURCE_WMI_HPP

#include <comdef.h>
#include <elepheye/Input.hpp>
#include <elepheye/Record.hpp>
#include <elepheye/RecordName.hpp>
#include <elepheye/lib/SkeletonRecord.hpp>
#include <list>
#include <wbemidl.h>

namespace elepheye
{
  namespace source
  {
    /**
     * Implementation of Input for the WMI.
     */
    class Wmi : public Input<Record, RecordName>
    {
    public:

      /**
       * Constructor.
       * @param path - The path to the WMI class.
       */
      explicit Wmi(const String& path);

      bool input();

      const ValueType& get() const;

      ValueType& get();

      const NameType& getName() const;

    private:
      _COM_SMARTPTR_TYPEDEF(IWbemServices, __uuidof(IWbemServices));
      _COM_SMARTPTR_TYPEDEF(
        IEnumWbemClassObject, __uuidof(IEnumWbemClassObject));
      const ::TCHAR* getHresultModule(::HRESULT hr) const throw();
      IWbemServicesPtr createServices(const String& path_namespace) const;
      void saveFieldNames(IWbemServicesPtr services, const String& path_class);
      void saveRecords(IWbemServicesPtr services, const String& path_class);
      bool saveNextRecord(
        IEnumWbemClassObjectPtr objects, const String& path_class);
      std::list<lib::SkeletonRecord> values_;
      NameType name_;
      bool is_first_input_;
    };
  }
}

#endif // #ifndef ELEPHEYE_SOURCE_WMI_HPP
