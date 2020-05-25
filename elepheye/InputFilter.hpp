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

#ifndef ELEPHEYE_INPUT_FILTER_HPP
#define ELEPHEYE_INPUT_FILTER_HPP

#include <elepheye/Input.hpp>
#include <elepheye/Shared.hpp>

namespace elepheye
{
  /**
   * Implementation of Input for filtering.
   * @param Value - The type of the value.
   * @param Name - The type of the name.
   * @param SourceValue - The type of the source value.
   * @param SourceName - The type of the source name.
   */
  template<
    class Value, class Name, class SourceValue = Value, class SourceName = Name>
  class InputFilter : public Input<Value, Name>
  {
  public:

    bool input()
    {
      return getSource()->input();
    }

  protected:

    /**
     * Type of the source.
     */
    typedef Input<SourceValue, SourceName> SourceType;

    /**
     * Constructor.
     * @param src - The source instance.
     */
    explicit InputFilter(const Shared<SourceType>& src) throw() : src_(src)
    {
    }

    /**
     * Get the source instance.
     * @return The instance.
     */
    const Shared<SourceType>& getSource() const throw()
    {
      return src_;
    }

  private:

    Shared<SourceType> src_;
  };

  template<class Value, class Name, class SourceValue>
  class InputFilter<Value, Name, SourceValue> : public Input<Value, Name>
  {
  public:

    bool input()
    {
      return getSource()->input();
    }

    const Name& getName() const
    {
      return getSource()->getName();
    }

  protected:

    typedef Input<SourceValue, Name> SourceType;

    explicit InputFilter(const Shared<SourceType>& src) throw() : src_(src)
    {
    }

    const Shared<SourceType>& getSource() const throw()
    {
      return src_;
    }

  private:

    Shared<SourceType> src_;
  };

  template<class Value, class Name>
  class InputFilter<Value, Name> : public Input<Value, Name>
  {
  public:

    bool input()
    {
      return getSource()->input();
    }

    const Value& get() const
    {
      return getSource()->get();
    }

    Value& get()
    {
      return getSource()->get();
    }

    const Name& getName() const
    {
      return getSource()->getName();
    }

  protected:

    typedef Input<Value, Name> SourceType;

    explicit InputFilter(const Shared<SourceType>& src) throw() : src_(src)
    {
    }

    const Shared<SourceType>& getSource() const throw()
    {
      return src_;
    }

  private:

    Shared<SourceType> src_;
  };
}

#endif // #ifndef ELEPHEYE_INPUT_FILTER_HPP
