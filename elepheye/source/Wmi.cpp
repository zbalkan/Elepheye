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

#include <elepheye/ExceptionProxy.hpp>
#include <elepheye/Exceptions.hpp>
#include <elepheye/lib/Field.hpp>
#include <elepheye/lib/Handles.hpp>
#include <elepheye/lib/RecordComparator.hpp>
#include <elepheye/source/Wmi.hpp>

namespace elepheye
{
  namespace source
  {
    Wmi::Wmi(const String& path) : is_first_input_(true)
    {
      // Split the path to the namespace and class name.
      String path_namespace;
      String path_class;
      const String::size_type end_of_namespace = path.find(_T(':'));
      if (String::npos == end_of_namespace)
      {
        path_namespace = path;
      }
      else
      {
        path_namespace = path.substr(0, end_of_namespace);
        path_class = path.substr(end_of_namespace + 1);
      }

      // Check the path.
      if (path_namespace.empty() || path_class.empty())
      {
        Exceptions::User e;
        e.setContext(path);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_INVALID_PATH);
        throw e;
      }

      // Generate all records.
      IWbemServicesPtr services = createServices(path_namespace);
      saveFieldNames(services, path_class);
      saveRecords(services, path_class);
      values_.sort(lib::RecordComparator());
    }

    bool Wmi::input()
    {
      // No more records.
      if (values_.empty())
      {
        return false;
      }

      // For the first input.
      if (is_first_input_)
      {
        is_first_input_ = false;
        return true;
      }

      // Remove the last record.
      values_.pop_front();
      return !values_.empty();
    }

    const Wmi::ValueType& Wmi::get() const
    {
      return values_.front();
    }

    Wmi::ValueType& Wmi::get()
    {
      return values_.front();
    }

    const Wmi::NameType& Wmi::getName() const
    {
      return name_;
    }

    const ::TCHAR* Wmi::getHresultModule(::HRESULT hr) const throw()
    {
      // Do nothing if not a WMI code.
      if (0x80041000 > hr || 0x800440ff < hr)
      {
        return 0;
      }

      // Return the path if already set.
      static ::TCHAR path[MAX_PATH + 1] = _T("");
      if (path[0])
      {
        return path;
      }

      // Get the system directory.
      ::SecureZeroMemory(path, sizeof(path));
      const ::UINT length = sizeof(path) / sizeof(path[0]);
      const ::UINT copied = ::GetSystemDirectory(path, length - 1);
      if (!copied || copied > length)
      {
        path[0] = 0;
        return 0;
      }

      // Select the module name.
      ::OSVERSIONINFO ovi;
      ::SecureZeroMemory(&ovi, sizeof(ovi));
      ovi.dwOSVersionInfoSize = sizeof(ovi);
      if (!::GetVersionEx(&ovi))
      {
        path[0] = 0;
        return 0;
      }
      if ((5 == ovi.dwMajorVersion && 0 < ovi.dwMinorVersion)
          || 5 < ovi.dwMajorVersion)
      {
        // Windows XP or later.
        ::_tcscat(path, _T("\\wbem\\wmiutils.dll"));
      }
      else
      {
        ::_tcscat(path, _T("\\wbem\\wbemcomn.dll"));
      }
      return path;
    }

    Wmi::IWbemServicesPtr Wmi::createServices(
      const String& path_namespace) const
    {
      // Create the locator.
      _COM_SMARTPTR_TYPEDEF(IWbemLocator, __uuidof(IWbemLocator));
      IWbemLocatorPtr locator;
      ::HRESULT hr = ::CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        __uuidof(IWbemLocator),
        reinterpret_cast<::LPVOID*>(&locator));
      if (FAILED(hr))
      {
        elepheye::Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setHresult(hr);
        e.setHresultModule(getHresultModule(hr));
        throw e;
      }
      if (!locator)
      {
        elepheye::Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Connect to the server.
      // Try to get locale-independent information,
      // although almost all information is already localized.
      IWbemServicesPtr services;
      hr = locator->ConnectServer(
        ::_bstr_t(path_namespace.c_str()),
        0,
        0,
        ::_bstr_t(_T("MS_409")),
        0,
        0,
        0,
        &services);
      if (FAILED(hr))
      {
        elepheye::Exceptions::System e;
        e.setContext(path_namespace);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setHresult(hr);
        e.setHresultModule(getHresultModule(hr));
        throw e;
      }
      if (!services)
      {
        elepheye::Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Set the security level.
      // The remote connection from Windows 2000 is not implemented.
      hr = ::CoSetProxyBlanket(
        services,
        RPC_C_AUTHN_DEFAULT,
        RPC_C_AUTHZ_DEFAULT,
        COLE_DEFAULT_PRINCIPAL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        COLE_DEFAULT_AUTHINFO,
        ::EOAC_DEFAULT);
      if (FAILED(hr))
      {
        elepheye::Exceptions::System e;
        e.setContext(path_namespace);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setHresult(hr);
        throw e;
      }
      return services;
    }

    void Wmi::saveFieldNames(
      IWbemServicesPtr services, const String& path_class)
    {
      // Get the class definition.
      _COM_SMARTPTR_TYPEDEF(IWbemClassObject, __uuidof(IWbemClassObject));
      IWbemClassObjectPtr class_def;
      ::HRESULT hr = services->GetObject(
        ::_bstr_t(path_class.c_str()),
        ::WBEM_FLAG_RETURN_WBEM_COMPLETE,
        0,
        &class_def,
        0);
      if (FAILED(hr))
      {
        elepheye::Exceptions::System e;
        e.setContext(path_class);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setHresult(hr);
        e.setHresultModule(getHresultModule(hr));
        throw e;
      }
      if (!class_def)
      {
        elepheye::Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Get property names.
      lib::Handles::SafeArrayDestroy prop_names;
      hr = class_def->GetNames(0, ::WBEM_FLAG_ALWAYS, 0, &prop_names.get());
      if (FAILED(hr))
      {
        elepheye::Exceptions::System e;
        e.setContext(path_class);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setHresult(hr);
        e.setHresultModule(getHresultModule(hr));
        throw e;
      }
      if (!prop_names.get())
      {
        elepheye::Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Lock property names.
      ::BSTR* locked = 0;
      hr = ::SafeArrayAccessData(
        prop_names.get(), reinterpret_cast<void**>(&locked));
      if (FAILED(hr))
      {
        Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setHresult(hr);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
      if (!locked)
      {
        elepheye::Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
      lib::Handles::SafeArrayUnaccessData auto_locked;
      auto_locked.get() = prop_names.get();

      // Check the type of property names.
      // Must be a single-dimensional ::BSTR array.
      if (1 != prop_names.get()->cDims)
      {
        elepheye::Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
      if (!(FADF_BSTR & prop_names.get()->fFeatures))
      {
        elepheye::Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Save property names as field names.
      // The property name `__PATH' is the key field.
      const String key_field_name(_T("__PATH"));
      bool is_key_field_name_found = false;
      name_.getFieldNames().push_back(key_field_name);
      for (::ULONG i = 0; i < prop_names.get()->rgsabound[0].cElements; ++i)
      {
        if (!locked[i])
        {
          elepheye::Exceptions::System e;
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
        if (!::_tcscmp(locked[i], key_field_name.c_str()))
        {
          is_key_field_name_found = true;
        }
        else
        {
          name_.getFieldNames().push_back(locked[i]);
        }
      }

      // Check the key field.
      if (!is_key_field_name_found)
      {
        elepheye::Exceptions::System e;
        e.setContext(path_class);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_WMI_PATH);
        throw e;
      }
    }

    void Wmi::saveRecords(IWbemServicesPtr services, const String& path_class)
    {
      // Create the enumerator for objects.
      IEnumWbemClassObjectPtr objects;
      const ::HRESULT hr = services->CreateInstanceEnum(
        ::_bstr_t(path_class.c_str()), ::WBEM_FLAG_FORWARD_ONLY, 0, &objects);
      if (FAILED(hr))
      {
        elepheye::Exceptions::System e;
        e.setContext(path_class);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setHresult(hr);
        e.setHresultModule(getHresultModule(hr));
        throw e;
      }
      if (!objects)
      {
        elepheye::Exceptions::System e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Save all objects as records.
      while (saveNextRecord(objects, path_class)) ;
      {
      }
    }

    bool Wmi::saveNextRecord(
      IEnumWbemClassObjectPtr objects, const String& path_class)
    {
      // Get the next object.
      _COM_SMARTPTR_TYPEDEF(IWbemClassObject, __uuidof(IWbemClassObject));
      ::ULONG returned = 0;
      IWbemClassObjectPtr object;
      const ::HRESULT hr = objects->Next(90000, 1, &object, &returned);
      switch (hr)
      {
      case WBEM_S_FALSE:
        // No more objects.
        return false;
      case WBEM_S_NO_ERROR:
        if (1 != returned)
        {
          elepheye::Exceptions::System e;
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
        if (!object)
        {
          elepheye::Exceptions::System e;
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
        break;
      default:
        {
          elepheye::Exceptions::System e;
          e.setContext(path_class);
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
          e.setHresult(hr);
          e.setHresultModule(getHresultModule(hr));
          throw e;
        }
      }

      // Save the object as a record.
      lib::SkeletonRecord record;
      for (
        std::vector<String>::const_iterator i = name_.getFieldNames().begin();
        name_.getFieldNames().end() != i;
        ++i)
      {
        // Save the property value as a field value.
        String field;
        try
        {
          ::_variant_t prop;
          const ::HRESULT hr
            = object->Get(i->c_str(), 0, &prop.GetVARIANT(), 0, 0);
          if (FAILED(hr))
          {
            elepheye::Exceptions::System e;
            e.setContext(path_class);
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_WMI);
            e.setHresult(hr);
            e.setHresultModule(getHresultModule(hr));
            throw e;
          }
          field = lib::Field::format(prop.GetVARIANT());
        }
        catch (const Exceptions::System& e)
        {
          // Log system errors.
          ExceptionProxy::log(e);
        }
        record.getFields().push_back(field);
      }
      values_.push_back(record);
      return true;
    }
  }
}
