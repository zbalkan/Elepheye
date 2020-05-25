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

#ifndef ELEPHEYE_EXCEPTION_HPP
#define ELEPHEYE_EXCEPTION_HPP

#include <elepheye/String.hpp>
#include <elepheye/lib/NoThrowString.hpp>
#include <elepheye/resource.h>

namespace elepheye
{
  /**
   * Class for the exception.
   */
  class Exception
  {
  public:

    /**
     * Constructor.
     */
    Exception() throw();

    /**
     * Destructor.
     */
    virtual ~Exception() throw();

    /**
     * Get the context.
     * @return The context.
     */
    const ::TCHAR* getContext() const throw();

    /**
     * Set the context.
     * @param context - The context.
     */
    void setContext(const ::TCHAR* context) throw();

    /**
     * Set the context.
     * @param context - The context.
     */
    void setContext(const String& context) throw();

    /**
     * Get the issue.
     * @return The resource ID for the issue.
     */
    ::UINT getIssue() const throw();

    /**
     * Set the issue.
     * @param issue - The resource ID for the issue.
     */
    void setIssue(::UINT issue) throw();

    /**
     * Get the reason.
     * @return The resource ID for the reason.
     */
    ::UINT getReason() const throw();

    /**
     * Set the reason.
     * @param reason - The resource ID for the reason.
     */
    void setReason(::UINT reason) throw();

    /**
     * Get the HRESULT code.
     * @return The code.
     */
    ::HRESULT getHresult() const throw();

    /**
     * Set the HRESULT code.
     * @param hr - The code.
     */
    void setHresult(::HRESULT hr) throw();

    /**
     * Set the Win32 error code.
     * This code is converted to the ::HRESULT code.
     * @param error - The code.
     */
    void setWin32(::DWORD error) throw();

    /**
     * Get the module name which contains the message for the ::HRESULT code.
     * @return The module name.
     */
    const ::TCHAR* getHresultModule() const throw();

    /**
     * Set the module name which contains the message for the ::HRESULT code.
     * @param module - The module name.
     */
    void setHresultModule(const ::TCHAR* module) throw();

    /**
     * Get the miscellaneous message.
     * @return The message.
     */
    const char* getMessage() const throw();

    /**
     * Set the miscellaneous message.
     * @param msg - The message.
     */
    void setMessage(const char* msg) throw();

    /**
     * Get the file name of the source code.
     * @return The file name.
     */
    const ::TCHAR* getFile() const throw();

    /**
     * Get the line number of the source code.
     * @return The line number.
     */
    int getLine() const throw();

    /**
     * Set the location of the source code.
     * @param file - The file name.
     * @param line - The line number.
     */
    void setSource(const ::TCHAR* file, int line) throw();

  private:
    lib::NoThrowString<::TCHAR> context_;
    ::UINT issue_;
    ::UINT reason_;
    ::HRESULT hr_;
    lib::NoThrowString<::TCHAR> hr_module_;
    lib::NoThrowString<char> msg_;
    lib::NoThrowString<::TCHAR> file_;
    int line_;
  };
}

#endif // #ifndef ELEPHEYE_EXCEPTION_HPP
