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

#ifndef ELEPHEYE_SHARED_HPP
#define ELEPHEYE_SHARED_HPP

#include <elepheye/lib/ReferenceCounterImpl.hpp>

namespace elepheye
{
  /**
   * Class for the shared pointer.
   * @param Value - The type of the value.
   */
  template<class Value>
  class Shared
  {
  public:

    /**
     * Constructor.
     * This instance represents the null pointer.
     */
    Shared() throw() : value_(0), counter_(0)
    {
    }

    /**
     * Constructor.
     * @param value - The value.
     */
    template<class Any>
    explicit Shared(Any* value) : value_(value), counter_(0)
    {
      if (value)
      {
        try
        {
          counter_ = new lib::ReferenceCounterImpl<Any>(value);
        }
        catch (...)
        {
          // For the exception-safety.
          delete value;
          throw;
        }
      }
    }

    Shared(const Shared& rhs) throw()
      : value_(rhs.value_), counter_(rhs.counter_)
    {
      if (counter_)
      {
        counter_->add();
      }
    }

    // Allow the implicit cast only.
    template<class Any>
    Shared(const Shared<Any>& rhs) throw()
      : value_(rhs.value_), counter_(rhs.counter_)
    {
      if (counter_)
      {
        counter_->add();
      }
    }

    ~Shared() throw()
    {
      if (counter_)
      {
        counter_->release();
      }
    }

    Shared& operator=(const Shared& rhs) throw()
    {
      return assign(rhs);
    }

    // Allow the implicit cast only.
    template<class Any>
    Shared& operator=(const Shared<Any>& rhs) throw()
    {
      return assign(rhs);
    }

    template<class Any>
    bool operator==(const Shared<Any>& rhs) const throw()
    {
      return value_ == rhs.value_;
    }

    template<class Any>
    bool operator!=(const Shared<Any>& rhs) const throw()
    {
      return value_ != rhs.value_;
    }

    Value& operator*() const throw()
    {
      return *value_;
    }

    Value* operator->() const throw()
    {
      return value_;
    }

    operator bool() const throw()
    {
      return value_ ? true : false;
    }

  private:

    template<class Any> friend class Shared;

    template<class Any>
    Shared& assign(const Shared<Any>& rhs) throw()
    {
      if (value_ != rhs.value_)
      {
        value_ = rhs.value_;
        if (counter_)
        {
          counter_->release();
        }
        counter_ = rhs.counter_;
        if (counter_)
        {
          counter_->add();
        }
      }
      return *this;
    }

    Value* value_;

    lib::ReferenceCounter* counter_;
  };
}

#endif // #ifndef ELEPHEYE_SHARED_HPP
