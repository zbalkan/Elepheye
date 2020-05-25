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

#include <cui/Console.hpp>
#include <cui/ExceptionLogger.hpp>
#include <cui/resource.h>
#include <iomanip>

namespace cui
{
  void ExceptionLogger::logException(
    const elepheye::Exception& e,
    elepheye::ExceptionLogger::Level level) throw()
  {
    // Get the separator.
    ::TCHAR sep[0x10];
    elepheye::string_load(CUI_RESOURCE_IDS_LABEL_SEPARATOR, sep);

    // Print the label.
    ::TCHAR s[0x100];
    switch (level)
    {
    case elepheye::ExceptionLogger::LEVEL_WARNING:
      Console::err << elepheye::string_load(CUI_RESOURCE_IDS_LABEL_WARNING, s);
      break;
    case elepheye::ExceptionLogger::LEVEL_ERROR:
      Console::err << elepheye::string_load(CUI_RESOURCE_IDS_LABEL_ERROR, s);
      break;
    }

    // Print the context.
    if (e.getContext())
    {
      Console::err << sep << e.getContext();
    }

    // Print the issue.
    if (e.getIssue())
    {
      Console::err << sep << elepheye::string_load(e.getIssue(), s);
    }

    // Print the reason.
    if (e.getReason())
    {
      Console::err << sep << elepheye::string_load(e.getReason(), s);
    }

    // Print the HRESULT code.
    if (e.getHresult())
    {
      Console::err << sep;
      ::DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER
                      | FORMAT_MESSAGE_FROM_SYSTEM
                      | FORMAT_MESSAGE_IGNORE_INSERTS;
      // Load the module if required.
      ::HMODULE module = 0;
      const ::TCHAR* module_name = e.getHresultModule();
      if (module_name)
      {
        module = ::LoadLibrary(module_name);
        if (module)
        {
          flags |= FORMAT_MESSAGE_FROM_HMODULE;
        }
      }
      // Format the message.
      ::TCHAR* msg = 0;
      if (::FormatMessage(
            flags,
            module,
            e.getHresult(),
            LANG_USER_DEFAULT,
            reinterpret_cast<::TCHAR*>(&msg),
            0,
            0)
          && msg)
      {
        // Remove last newlines.
        std::size_t i = ::_tcslen(msg);
        while (i-- && ::_tcschr(_T("\r\n"), msg[i]))
        {
          msg[i] = _T('\0');
        }
        // Print the message.
        Console::err << msg;
        ::LocalFree(msg);
      }
      // Unload the module.
      if (module)
      {
        ::FreeLibrary(module);
      }
      // Append the number.
      Console::err
        << _T('(')
        << _T("0x")
        << std::hex
        << std::right
        << std::setfill(_T('0'))
        << std::setw(sizeof(::HRESULT) * 2)
        << std::uppercase
        << e.getHresult()
        << _T(')');
    }

    // Print the miscellaneous message.
    if (e.getMessage())
    {
      Console::err << sep;
      std::cerr << e.getMessage();
    }

    // Print the location of the source code.
    if (e.getFile())
    {
      Console::err
        << sep << e.getFile() << _T('(') << std::dec << e.getLine() << _T(')');
    }

    // Print the newline.
    Console::err << std::endl;
  }
}
