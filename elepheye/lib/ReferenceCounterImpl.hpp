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

#ifndef ELEPHEYE_LIB_REFERENCE_COUNTER_IMPL_HPP
#define ELEPHEYE_LIB_REFERENCE_COUNTER_IMPL_HPP

#include <elepheye/lib/ReferenceCounter.hpp>

namespace elepheye
{
  namespace lib
  {
    /**
     * Implementation of ReferenceCounter with the dynamic deleter.
     * @param Value - The type of the value.
     */
    template<class Value>
    class ReferenceCounterImpl : public ReferenceCounter
    {
    public:

      /**
       * Constructor.
       * @param value - The value.
       */
      explicit ReferenceCounterImpl(Value* value) throw() : value_(value)
      {
      }

      /**
       * Destructor.
       */
      ~ReferenceCounterImpl() throw()
      {
        delete value_;
      }

    private:

      Value* value_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_REFERENCE_COUNTER_IMPL_HPP
