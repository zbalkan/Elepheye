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

#ifndef ELEPHEYE_LIB_FILE_INPUT_HPP
#define ELEPHEYE_LIB_FILE_INPUT_HPP

#include <elepheye/Exceptions.hpp>
#include <elepheye/Input.hpp>
#include <elepheye/Shared.hpp>
#include <elepheye/lib/File.hpp>
#include <vector>

namespace elepheye
{
  namespace lib
  {
    /**
     * Implementation of Input to read a file.
     * @param Value - The type of the value.
     */
    template<class Value>
    class FileInput : public Input<Value, String>
    {
    public:

      /**
       * Constructor.
       * @param file - The file.
       */
      explicit FileInput(const Shared<File>& file)
        : file_(file), name_(file_->getPath())
      {
      }

      bool input()
      {
        // Initialize.
        ::SecureZeroMemory(&value_, sizeof(value_));

        // Read the value.
        ::DWORD read_size = 0;
        if (!::ReadFile(file_->get(), &value_, sizeof(Value), &read_size, 0))
        {
          const ::DWORD error = ::GetLastError();
          Exceptions::System e;
          e.setContext(file_->getPath());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }

        // End of the file.
        if (!read_size)
        {
          return false;
        }

        // Unexpected end of the file.
        if (sizeof(Value) != read_size)
        {
          Exceptions::User e;
          e.setContext(file_->getPath());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILE);
          e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_UNEXPECTED_EOF);
          throw e;
        }
        return true;
      }

      const Value& get() const
      {
        return value_;
      }

      Value& get()
      {
        return value_;
      }

      const NameType& getName() const
      {
        return name_;
      }

    private:

      Shared<File> file_;

      Value value_;

      NameType name_;
    };

    template<class Value>
    class FileInput<std::vector<Value> >
      : public Input<std::vector<Value>, String>
    {
    public:

      explicit FileInput(const Shared<File>& file)
        : file_(file), name_(file_->getPath())
      {
      }

      // The empty vector is not allowed.
      bool input()
      {
        // Initialize.
        ::SecureZeroMemory(&value_.at(0), value_.size() * sizeof(value_.at(0)));

        // Read the value.
        ::DWORD read_size = 0;
        if (!::ReadFile(
              file_->get(),
              &value_.at(0),
              static_cast<::DWORD>(value_.size() * sizeof(Value)),
              &read_size,
              0))
        {
          const ::DWORD error = ::GetLastError();
          Exceptions::System e;
          e.setContext(file_->getPath());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILE);
          e.setWin32(error);
          e.setSource(_T(__FILE__), __LINE__);
          throw e;
        }

        // End of the file.
        if (!read_size)
        {
          return false;
        }

        // Unexpected end of the file.
        if (read_size % sizeof(Value))
        {
          Exceptions::User e;
          e.setContext(file_->getPath());
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILE);
          e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_UNEXPECTED_EOF);
          throw e;
        }

        // Resize the vector to the read size.
        value_.resize(read_size / sizeof(Value));
        return true;
      }

      const ValueType& get() const
      {
        return value_;
      }

      ValueType& get()
      {
        return value_;
      }

      const NameType& getName() const
      {
        return name_;
      }

    private:

      Shared<File> file_;

      ValueType value_;

      NameType name_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_FILE_INPUT_HPP
