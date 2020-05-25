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

#ifndef ELEPHEYE_LIB_INPUT_BUFFER_HPP
#define ELEPHEYE_LIB_INPUT_BUFFER_HPP

#include <elepheye/InputFilter.hpp>
#include <vector>

namespace elepheye
{
  namespace lib
  {
    /**
     * Implementation of InputFilter for buffering.
     * @param Value - The type of the value.
     * @param Name - The type of the name.
     */
    template<class Value, class Name>
    class InputBuffer : public InputFilter<Value, Name, std::vector<Value> >
    {
    public:

      /**
       * Constructor.
       * @param count - The count of values in the buffer.
       * @param src - The source instance.
       */
      InputBuffer(
        typename SourceType::ValueType::size_type count,
        const Shared<SourceType>& src)
        : InputFilter(src), index_(0)
      {
        getSource()->get().resize(count);
        getSource()->input();
      }

      bool input()
      {
        if (getSource()->get().size() < ++index_)
        {
          index_ = 1;
          return getSource()->input();
        }
        return true;
      }

      const Value& get() const
      {
        return getSource()->get().at(index_ - 1);
      }

      Value& get()
      {
        return getSource()->get().at(index_ - 1);
      }

    private:

      typename SourceType::ValueType::size_type index_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_INPUT_BUFFER_HPP
