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

#ifndef ELEPHEYE_LIB_FILE_OUTPUT_HPP
#define ELEPHEYE_LIB_FILE_OUTPUT_HPP

#include <elepheye/Exceptions.hpp>
#include <elepheye/Output.hpp>
#include <elepheye/Shared.hpp>
#include <elepheye/lib/File.hpp>
#include <vector>

namespace elepheye
{
  namespace lib
  {
    /**
     * Implementation of Output to write a file.
     * @param Value - The type of the value.
     */
    template<class Value>
    class FileOutput : public Output<Value, String>
    {
    public:

      /**
       * Constructor.
       * @param file - The file.
       */
      explicit FileOutput(const Shared<File>& file)
        : file_(file), name_(file_->getPath())
      {
      }

      void output(const Value& value)
      {
        write(value);
      }

      const NameType& getName() const
      {
        return name_;
      }

    private:

      // Write a primitive value.
      template<class Any>
      void write(const Any& value)
      {
        // Write the value.
        ::DWORD written_size = 0;
        if (!::WriteFile(file_->get(), &value, sizeof(Any), &written_size, 0))
        {
          const ::DWORD error = ::GetLastError();
          Exceptions::System e;
          e.setContext(file_->getPath());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_WRITE_FILE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }

        // Check the written size.
        if (sizeof(Any) != written_size)
        {
          Exceptions::System e;
          e.setContext(file_->getPath());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_WRITE_FILE);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
      }

      // Write a std::vector.
      template<class Any>
      void write(const std::vector<Any>& value)
      {
        // Write the value.
        const ::DWORD write_size
          = static_cast<::DWORD>(value.size() * sizeof(Any));
        ::DWORD written_size = 0;
        if (!::WriteFile(
              file_->get(), &value.at(0), write_size, &written_size, 0))
        {
          const ::DWORD error = ::GetLastError();
          Exceptions::System e;
          e.setContext(file_->getPath());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_WRITE_FILE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }

        // Check the written size.
        if (write_size != written_size)
        {
          Exceptions::System e;
          e.setContext(file_->getPath());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_WRITE_FILE);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
      }

      // Write a std::basic_string.
      template<class Any>
      void write(const std::basic_string<Any>& value)
      {
        // Write the value.
        const ::DWORD write_size
          = static_cast<::DWORD>(value.length() * sizeof(Any));
        ::DWORD written_size = 0;
        if (!::WriteFile(
              file_->get(), value.c_str(), write_size, &written_size, 0))
        {
          const ::DWORD error = ::GetLastError();
          Exceptions::System e;
          e.setContext(file_->getPath());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_WRITE_FILE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }

        // Check the written size.
        if (write_size != written_size)
        {
          Exceptions::System e;
          e.setContext(file_->getPath());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_WRITE_FILE);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }
      }

      Shared<File> file_;

      NameType name_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_FILE_OUTPUT_HPP
