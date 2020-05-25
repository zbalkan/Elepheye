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

#ifndef ELEPHEYE_FORWARD_ITERATOR_HPP
#define ELEPHEYE_FORWARD_ITERATOR_HPP

#include <elepheye/Shared.hpp>
#include <iterator>

namespace elepheye
{
  /**
   * ForwardIterator adapter for Input.
   * @param Adaptee - The type of the adaptee.
   */
  template<class Adaptee>
  class ForwardIterator
    : public std::iterator<
        std::forward_iterator_tag, typename Adaptee::ValueType>
  {
  public:

    /**
     * Constructor.
     * This instance represents the end marker.
     */
    ForwardIterator() throw()
    {
    }

    /**
     * Constructor.
     * @param adaptee - The adaptee instance.
     */
    explicit ForwardIterator(const Shared<Adaptee>& adaptee) throw()
      : adaptee_(adaptee)
    {
      forward();
    }

    bool operator==(const ForwardIterator& rhs) const throw()
    {
      return adaptee_ == rhs.adaptee_;
    }

    bool operator!=(const ForwardIterator& rhs) const throw()
    {
      return adaptee_ != rhs.adaptee_;
    }

    typename Adaptee::ValueType& operator*() const
    {
      return adaptee_->get();
    }

    typename Adaptee::ValueType* operator->() const
    {
      return &adaptee_->get();
    }

    ForwardIterator& operator++()
    {
      forward();
      return *this;
    }

    /**
     * Get the name.
     * @return The name.
     */
    const typename Adaptee::NameType& getName() const
    {
      return adaptee_->getName();
    }

  private:

    void forward()
    {
      if (adaptee_)
      {
        if (!adaptee_->input())
        {
          adaptee_ = Shared<Adaptee>();
        }
      }
    }

    Shared<Adaptee> adaptee_;
  };
}

#endif // #ifndef ELEPHEYE_FORWARD_ITERATOR_HPP
