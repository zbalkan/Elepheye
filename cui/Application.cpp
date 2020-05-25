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

#include <cui/Application.hpp>
#include <cui/Com.hpp>
#include <cui/CommandLine.hpp>
#include <cui/Console.hpp>
#include <cui/ExceptionLogger.hpp>
#include <elepheye/ExceptionProxy.hpp>
#include <locale>
#include <new>

namespace cui
{
  namespace
  {
    void initialize_locale()
    {
      // Set the global locale to C.
      const std::locale c(std::locale::classic());
      std::locale::global(c);

      // Need to print non-ascii characters.
      const std::locale l(c, std::locale(""), std::locale::ctype);
      Console::out.imbue(l);
      Console::err.imbue(l);
    }
  }

  int Application::start(int argc, const ::TCHAR* const* argv) throw()
  {
    // Set the exception logger.
    ExceptionLogger logger;
    elepheye::ExceptionProxy::setLogger(&logger);

    // Start.
    try
    {
      initialize_locale();
      cui::Com com(::COINIT_MULTITHREADED);
      CommandLine command_line(argc, argv);
    }
    catch (const elepheye::Exception& e)
    {
      elepheye::ExceptionProxy::log(e);
    }
    catch (const std::bad_alloc&)
    {
      elepheye::Exception e;
      e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_ALLOCATE_MEMORY);
      elepheye::ExceptionProxy::log(e);
    }
    catch (const std::exception& se)
    {
      elepheye::Exception e;
      e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_UNEXPECTED);
      e.setMessage(se.what());
      e.setSource(_T(__FILE__), __LINE__);
      elepheye::ExceptionProxy::log(e);
    }
    catch (...)
    {
      elepheye::Exception e;
      e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_UNEXPECTED);
      e.setSource(_T(__FILE__), __LINE__);
      elepheye::ExceptionProxy::log(e);
    }

    // Check the error.
    if (elepheye::ExceptionProxy::getMaxLevel()
        < elepheye::ExceptionLogger::LEVEL_ERROR)
    {
      return 0;
    }
    return 1;
  }
}
