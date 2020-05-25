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

#ifndef ELEPHEYE_OUTPUT_FILTER_HPP
#define ELEPHEYE_OUTPUT_FILTER_HPP

#include <elepheye/Output.hpp>
#include <elepheye/Shared.hpp>

namespace elepheye
{
  /**
   * Implementation of Output for filtering.
   * @param Value - The type of the value.
   * @param Name - The type of the name.
   * @param DestinationValue - The type of the destination value.
   * @param DestinationName - The type of the destination name.
   */
  template<
    class Value,
    class Name,
    class DestinationValue = Value,
    class DestinationName = Name>
  class OutputFilter : public Output<Value, Name>
  {
  protected:

    /**
     * Type of the destination.
     */
    typedef Output<DestinationValue, DestinationName> DestinationType;

    /**
     * Constructor.
     * @param dst - The destination instance.
     */
    explicit OutputFilter(const Shared<DestinationType>& dst) throw()
      : dst_(dst)
    {
    }

    /**
     * Get the destination instance.
     * @return The instance.
     */
    const Shared<DestinationType>& getDestination() const throw()
    {
      return dst_;
    }

  private:

    Shared<DestinationType> dst_;
  };

  template<class Value, class Name, class DestinationValue>
  class OutputFilter<Value, Name, DestinationValue> : public Output<Value, Name>
  {
  public:

    const Name& getName() const
    {
      return getDestination()->getName();
    }

  protected:

    typedef Output<DestinationValue, Name> DestinationType;

    explicit OutputFilter(const Shared<DestinationType>& dst) throw()
      : dst_(dst)
    {
    }

    const Shared<DestinationType>& getDestination() const throw()
    {
      return dst_;
    }

  private:

    Shared<DestinationType> dst_;
  };

  template<class Value, class Name>
  class OutputFilter<Value, Name> : public Output<Value, Name>
  {
  public:

    void output(const Value& value)
    {
      getDestination()->output(value);
    }

    const Name& getName() const
    {
      return getDestination()->getName();
    }

  protected:

    typedef Output<Value, Name> DestinationType;

    explicit OutputFilter(const Shared<DestinationType>& dst) throw()
      : dst_(dst)
    {
    }

    const Shared<DestinationType>& getDestination() const throw()
    {
      return dst_;
    }

  private:

    Shared<DestinationType> dst_;
  };
}

#endif // #ifndef ELEPHEYE_OUTPUT_FILTER_HPP
