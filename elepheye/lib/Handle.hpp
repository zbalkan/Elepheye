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

#ifndef ELEPHEYE_LIB_HANDLE_HPP
#define ELEPHEYE_LIB_HANDLE_HPP

#include <elepheye/String.hpp>
#include <elepheye/lib/NoThrowString.hpp>
#include <elepheye/lib/NonCopyable.hpp>

namespace elepheye
{
  namespace lib
  {
    /**
     * Class for a handle.
     * @param Value - The type of the value.
     */
    template<class Value>
    class Handle : private NonCopyable
    {
    public:

      /**
       * Destructor.
       */
      virtual ~Handle() throw()
      {
      }

      /**
       * Get the value.
       * @return The value.
       */
      const Value& get() const throw()
      {
        return value_;
      }

      /**
       * Get the value.
       * @return The value.
       */
      Value& get() throw()
      {
        return value_;
      }

      /**
       * Get the name.
       * @return The name.
       */
      const ::TCHAR* getName() const throw()
      {
        return name_.get(_T("(ERROR: Failed to get the name)"));
      }

      /**
       * Set the name.
       * @param name - The name.
       */
      void setName(const ::TCHAR* name) throw()
      {
        name_.set(name);
      }

      /**
       * Set the name.
       * @param name - The name.
       */
      void setName(const String& name) throw()
      {
        name_.set(name);
      }

    protected:

      /**
       * Constructor.
       * @param value - The initial value.
       */
      explicit Handle(const Value& value) throw() : value_(value)
      {
      }

    private:

      Value value_;

      NoThrowString<::TCHAR> name_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_HANDLE_HPP
