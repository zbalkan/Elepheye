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

#include <comutil.h>
#include <elepheye/lib/Field.hpp>
#include <elepheye/lib/Handles.hpp>
#include <sddl.h>
#include <vector>

namespace elepheye
{
  namespace lib
  {
    namespace
    {
      bool increment_indices(
        std::vector<::LONG>& indices,
        const std::vector<::LONG>& l_bounds,
        const std::vector<::LONG>& u_bounds)
      {
        for (std::vector<::LONG>::size_type i = 0; indices.size() > i; ++i)
        {
          if (indices.at(i) < u_bounds.at(i))
          {
            ++indices.at(i);
            for (std::vector<::LONG>::size_type j = 0; j < i; ++j)
            {
              indices.at(j) = l_bounds.at(j);
            }
            return true;
          }
        }
        return false;
      }
    }

    int Field::compareKey(const String& key1, const String& key2)
    {
      // Compare case-sensitively.
      const int result = ::_tcscmp(key1.c_str(), key2.c_str());
      if (!result)
      {
        return result;
      }

      // Compare case-insensitively.
      const String upper1(toUpper(key1));
      const String upper2(toUpper(key2));
      const int upper_result = ::_tcscmp(upper1.c_str(), upper2.c_str());
      if (!upper_result)
      {
        return result;
      }
      return upper_result;
    }

    String Field::toUpper(const String& s)
    {
      // Do nothing if empty.
      if (s.empty())
      {
        return s;
      }

      // Convert to uppercase locale-independently.
      std::vector<::TCHAR> upper(s.length() + 1, 0);
      if (!::LCMapString(
            MAKELCID(
              MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
              SORT_DEFAULT), // LOCALE_INVARIANT.
            LCMAP_UPPERCASE,
            s.c_str(),
            static_cast<int>(s.length()),
            &upper.at(0),
            static_cast<int>(upper.size())))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setContext(s);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_UNEXPECTED);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
      return &upper.at(0);
    }

    String Field::format(
      ::SECURITY_DESCRIPTOR* sd, ::SECURITY_INFORMATION si, ::UINT issue)
    {
      // Check ACL not to get ERROR_INVALID_ACL from
      // ::ConvertSecurityDescriptorToStringSecurityDescriptor.
      ::BOOL is_present = FALSE;
      ::BOOL is_defaulted = FALSE;
      ::ACL* acl = 0;
      switch (si)
      {
      case DACL_SECURITY_INFORMATION:
        if (!::GetSecurityDescriptorDacl(sd, &is_present, &acl, &is_defaulted))
        {
          const ::DWORD error = ::GetLastError();
          Exceptions::System e;
          e.setIssue(issue);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
        if (!is_present)
        {
          // No ACL.
          return _T("");
        }
        if (!acl)
        {
          // Null ACL.
          return _T("");
        }
        break;
      case SACL_SECURITY_INFORMATION:
        if (!::GetSecurityDescriptorSacl(sd, &is_present, &acl, &is_defaulted))
        {
          const ::DWORD error = ::GetLastError();
          Exceptions::System e;
          e.setIssue(issue);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
        if (!is_present)
        {
          // No ACL.
          return _T("");
        }
        if (!acl)
        {
          // Null ACL.
          return _T("");
        }
        break;
      }

      // Convert the security descriptor to the string.
      ::TCHAR* converted = 0;
      if (!::ConvertSecurityDescriptorToStringSecurityDescriptor(
            sd, SDDL_REVISION_1, si, &converted, 0))
      {
        const ::DWORD error = ::GetLastError();
        Exceptions::System e;
        e.setIssue(issue);
        e.setWin32(error);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
      if (!converted)
      {
        Exceptions::System e;
        e.setIssue(issue);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
      Handles::LocalFree auto_converted;
      auto_converted.get() = converted;
      return converted;
    }

    String Field::formatVariant(const ::VARIANT& value)
    {
      // Convert some types in the custom way.
      switch (value.vt)
      {
      case ::VT_BSTR | ::VT_BYREF:
        return value.pbstrVal && *value.pbstrVal ? *value.pbstrVal : _T("");
      case ::VT_BSTR:
        return value.bstrVal ? value.bstrVal : _T("");
      case ::VT_DATE | ::VT_BYREF:
        {
          ::SYSTEMTIME st;
          ::SecureZeroMemory(&st, sizeof(st));
          if (value.pdate)
          {
            if (!::VariantTimeToSystemTime(*value.pdate, &st))
            {
              const ::DWORD error = ::GetLastError();
              Exceptions::System e;
              e.setContext(formatToHex(value.vt));
              e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_UNEXPECTED);
              e.setWin32(error);
              e.setSource(_T(__FILE__), __LINE__);
              throw e;
            }
            return format(st);
          }
          return _T("");
        }
      case ::VT_DATE:
        {
          ::SYSTEMTIME st;
          ::SecureZeroMemory(&st, sizeof(st));
          if (!::VariantTimeToSystemTime(value.date, &st))
          {
            const ::DWORD error = ::GetLastError();
            Exceptions::System e;
            e.setContext(formatToHex(value.vt));
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_UNEXPECTED);
            e.setWin32(error);
            e.setSource(_T(__FILE__), __LINE__);
            throw e;
          }
          return format(st);
        }
      case ::VT_EMPTY | ::VT_BYREF:
        // Invalid type.
        return _T("");
      case ::VT_EMPTY:
        return _T("");
      case ::VT_ERROR | ::VT_BYREF:
        return value.pscode ? formatToHex(*value.pscode) : _T("");
      case ::VT_ERROR:
        return formatToHex(value.scode);
      case ::VT_I8 | ::VT_BYREF:
        // Not supported by ::VariantChangeTypeEx under Windows 2000.
        return value.pllVal ? format(*value.pllVal) : _T("");
      case ::VT_I8:
        // Not supported by ::VariantChangeTypeEx under Windows 2000.
        return format(value.llVal);
      case ::VT_ILLEGAL:
        return _T("");
      case ::VT_NULL | ::VT_BYREF:
        // Invalid type.
        return _T("");
      case ::VT_NULL:
        return _T("");
      case ::VT_UI8 | ::VT_BYREF:
        // Not supported by ::VariantChangeTypeEx under Windows 2000.
        return value.pullVal ? format(*value.pullVal) : _T("");
      case ::VT_UI8:
        // Not supported by ::VariantChangeTypeEx under Windows 2000.
        return format(value.ullVal);
      case ::VT_VARIANT | ::VT_BYREF:
        if (value.pvarVal)
        {
          if (value.pvarVal->vt == value.vt)
          {
            // Invalid type.
            return formatToHex(value.vt) + _T(':');
          }
          return format(*value.pvarVal);
        }
        return _T("");
      case ::VT_VARIANT:
        // Invalid type.
        return _T("");
      }

      // Convert other types in the default way.
      ::_variant_t src(value);
      ::_variant_t dst;
      const ::HRESULT hr = ::VariantChangeTypeEx(
        &dst.GetVARIANT(),
        &src.GetVARIANT(),
        MAKELCID(
          MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
          SORT_DEFAULT), // LOCALE_INVARIANT.
        VARIANT_ALPHABOOL,
        ::VT_BSTR);
      if (FAILED(hr))
      {
        Exceptions::System e;
        e.setContext(formatToHex(value.vt));
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_UNEXPECTED);
        e.setHresult(hr);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
      return dst.bstrVal ? dst.bstrVal : _T("");
    }

    String Field::formatVariantArray(const ::VARIANT& value)
    {
      // Get the array.
      ::SAFEARRAY* array = 0;
      if (::VT_BYREF & value.vt)
      {
        if (value.pparray)
        {
          array = *value.pparray;
        }
      }
      else
      {
        array = value.parray;
      }
      if (!array)
      {
        // Null array.
        return _T("");
      }

      // Get dimensions.
      const ::UINT dims = ::SafeArrayGetDim(array);
      if (!dims)
      {
        // Empty array.
        return _T("");
      }

      // Get lower and upper bounds.
      std::vector<::LONG> l_bounds(dims);
      std::vector<::LONG> u_bounds(dims);
      for (::UINT i = 0; i < dims; ++i)
      {
        ::HRESULT hr = ::SafeArrayGetLBound(array, i + 1, &l_bounds.at(i));
        if (FAILED(hr))
        {
          Exceptions::System e;
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_UNEXPECTED);
          e.setHresult(hr);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
        hr = ::SafeArrayGetUBound(array, i + 1, &u_bounds.at(i));
        if (FAILED(hr))
        {
          Exceptions::System e;
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_UNEXPECTED);
          e.setHresult(hr);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
        if (l_bounds.at(i) > u_bounds.at(i))
        {
          // No elements.
          return _T("");
        }
      }

      // Initialize indices.
      std::vector<::LONG> indices(l_bounds);

      // Format all elements.
      std::basic_ostringstream<::TCHAR> elements;
      do
      {
        // Get an element.
        ::_variant_t v;
        String element;
        switch (::VT_TYPEMASK & value.vt)
        {
        case ::VT_DECIMAL:
          {
            const ::HRESULT hr
              = ::SafeArrayGetElement(array, &indices.at(0), &v.decVal);
            if (FAILED(hr))
            {
              Exceptions::System e;
              e.setContext(formatToHex(value.vt));
              e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_UNEXPECTED);
              e.setHresult(hr);
              e.setSource(_T(__FILE__), __LINE__);
              throw e;
            }
            v.vt = ::VT_DECIMAL;
            element = formatVariant(v.GetVARIANT());
          }
          break;
        case ::VT_VARIANT:
          {
            const ::HRESULT hr
              = ::SafeArrayGetElement(array, &indices.at(0), &v.GetVARIANT());
            if (FAILED(hr))
            {
              Exceptions::System e;
              e.setContext(formatToHex(value.vt));
              e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_UNEXPECTED);
              e.setHresult(hr);
              e.setSource(_T(__FILE__), __LINE__);
              throw e;
            }
            element = format(v.GetVARIANT());
          }
          break;
        default:
          {
            const ::HRESULT hr
              = ::SafeArrayGetElement(array, &indices.at(0), &v.byref);
            if (FAILED(hr))
            {
              Exceptions::System e;
              e.setContext(formatToHex(value.vt));
              e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_UNEXPECTED);
              e.setHresult(hr);
              e.setSource(_T(__FILE__), __LINE__);
              throw e;
            }
            v.vt = static_cast<::VARTYPE>(::VT_TYPEMASK & value.vt);
            element = formatVariant(v.GetVARIANT());
          }
          break;
        }
        // Format an element.
        elements << _T('[');
        for (std::vector<::LONG>::size_type i = 0; indices.size() > i; ++i)
        {
          if (i)
          {
            elements << _T(',');
          }
          elements << indices.at(i);
        }
        elements << _T(':') << element.length() << _T(']') << element;
      }
      while (increment_indices(indices, l_bounds, u_bounds));
      return elements.str();
    }
  }
}
