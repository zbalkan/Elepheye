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

#ifndef ELEPHEYE_LIB_FIELD_HPP
#define ELEPHEYE_LIB_FIELD_HPP

#include <elepheye/Exceptions.hpp>
#include <elepheye/String.hpp>
#include <iomanip>
#include <sstream>

namespace elepheye
{
  namespace lib
  {
    /**
     * Static class for field operations.
     */
    class Field
    {
    public:

      /**
       * Compare key fields to sort.
       * @param key1 - The key.
       * @param key2 - Another key.
       * @return The result in the style of strcmp.
       */
      static int compareKey(const String& key1, const String& key2);

      /**
       * Convert a string to uppercase to ignore case.
       * @param s - The string.
       * @return The uppercase string.
       */
      static String toUpper(const String& s);

      /**
       * Format a primitive value to the string.
       * @param value - The value.
       * @return The string.
       */
      template<class Value>
      static String format(const Value& value)
      {
        std::basic_ostringstream<::TCHAR> s;
        s << value;
        return s.str();
      }

      /**
       * Format a primitive value to the hexadecimal string.
       * @param value - The value.
       * @return The string.
       */
      template<class Value>
      static String formatToHex(const Value& value)
      {
        std::basic_ostringstream<::TCHAR> s;
        s << _T("0x")
          << std::hex
          << std::right
          << std::setfill(_T('0'))
          << std::setw(sizeof(value) * 2)
          << std::uppercase
          << value;
        return s.str();
      }

      /**
       * Format a ::FILETIME to the ISO 8601 string in UTC.
       * @param value - The value.
       * @return The string.
       */
      template<>
      static String format(const ::FILETIME& value)
      {
        // Convert to the system time.
        ::SYSTEMTIME st;
        ::SecureZeroMemory(&st, sizeof(st));
        if (!::FileTimeToSystemTime(&value, &st))
        {
          const ::DWORD error = ::GetLastError();
          Exceptions::System e;
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_UNEXPECTED);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }

        // Format.
        return format(st);
      }

      /**
       * Format a ::SYSTEMTIME to the ISO 8601 string in UTC.
       * @param value - The value.
       * @return The string.
       */
      template<>
      static String format(const ::SYSTEMTIME& value)
      {
        std::basic_ostringstream<::TCHAR> s;
        s << value.wYear
          << _T('-')
          << std::right
          << std::setfill(_T('0'))
          << std::setw(2)
          << value.wMonth
          << _T('-')
          << std::setw(2)
          << value.wDay
          << _T('T')
          << std::setw(2)
          << value.wHour
          << _T(':')
          << std::setw(2)
          << value.wMinute
          << _T(':')
          << std::setw(2)
          << value.wSecond
          << _T('.')
          << std::setw(3)
          << value.wMilliseconds
          << _T('Z');
        return s.str();
      }

      /**
       * Format a ::VARIANT to the string.
       * @param value - The value.
       * @return The string.
       */
      template<>
      static String format(const ::VARIANT& value)
      {
        const String type(formatToHex(value.vt) + _T(':'));
        if (::VT_ARRAY & value.vt && ::VT_ILLEGAL != value.vt)
        {
          return type + formatVariantArray(value);
        }
        else
        {
          return type + formatVariant(value);
        }
      }

      /**
       * Format split numbers to the string.
       * @param high - The high-order value.
       * @param low - The low-order value.
       * @return The string.
       */
      template<class Value, class High, class Low>
      static String format(const High& high, const Low& low)
      {
        const Value value
          = static_cast<Value>(high) << (sizeof(Low) * 8)
            | static_cast<Value>(low);
        std::basic_ostringstream<::TCHAR> s;
        s << value;
        return s.str();
      }

      /**
       * Format a security descriptor to the SDDL string.
       * @param sd - The security descriptor.
       * @param si - The security information.
       * @param issue - The issue for the exception.
       * @return The string.
       */
      static String format(
        ::SECURITY_DESCRIPTOR* sd, ::SECURITY_INFORMATION si, ::UINT issue);

    private:
      static String formatVariant(const ::VARIANT& value);
      static String formatVariantArray(const ::VARIANT& value);
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_FIELD_HPP
