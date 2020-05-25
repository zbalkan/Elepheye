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

#include <elepheye/Exception.hpp>

namespace elepheye
{
  Exception::Exception() throw() : issue_(0), reason_(0), hr_(S_OK), line_(0)
  {
  }

  Exception::~Exception() throw()
  {
  }

  const ::TCHAR* Exception::getContext() const throw()
  {
    return context_.get(_T("(ERROR: Failed to get the context)"));
  }

  void Exception::setContext(const ::TCHAR* context) throw()
  {
    context_.set(context);
  }

  void Exception::setContext(const String& context) throw()
  {
    context_.set(context);
  }

  ::UINT Exception::getIssue() const throw()
  {
    return issue_;
  }

  void Exception::setIssue(::UINT issue) throw()
  {
    issue_ = issue;
  }

  ::UINT Exception::getReason() const throw()
  {
    return reason_;
  }

  void Exception::setReason(::UINT reason) throw()
  {
    reason_ = reason;
  }

  ::HRESULT Exception::getHresult() const throw()
  {
    return hr_;
  }

  void Exception::setHresult(::HRESULT hr) throw()
  {
    hr_ = hr;
  }

  void Exception::setWin32(::DWORD error) throw()
  {
    hr_ = HRESULT_FROM_WIN32(error);
  }

  const ::TCHAR* Exception::getHresultModule() const throw()
  {
    return hr_module_.get(0);
  }

  void Exception::setHresultModule(const ::TCHAR* module) throw()
  {
    hr_module_.set(module);
  }

  const char* Exception::getMessage() const throw()
  {
    return msg_.get("(ERROR: Failed to get the message)");
  }

  void Exception::setMessage(const char* msg) throw()
  {
    msg_.set(msg);
  }

  const ::TCHAR* Exception::getFile() const throw()
  {
    return file_.get(_T("(ERROR: Failed to get the file name)"));
  }

  int Exception::getLine() const throw()
  {
    return line_;
  }

  void Exception::setSource(const ::TCHAR* file, int line) throw()
  {
    file_.set(file);
    line_ = line;
  }
}
