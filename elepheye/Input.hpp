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

#ifndef ELEPHEYE_INPUT_HPP
#define ELEPHEYE_INPUT_HPP

namespace elepheye
{
  /**
   * Interface to input values.
   * @param Value - The type of the value.
   * @param Name - The type of the name.
   */
  template<class Value, class Name>
  class Input
  {
  public:

    /**
     * Type of the value.
     */
    typedef Value ValueType;

    /**
     * Type of the name.
     */
    typedef Name NameType;

    /**
     * Destructor.
     */
    virtual ~Input() throw()
    {
    }

    /**
     * Input a value.
     * @return False if no more values.
     */
    virtual bool input() = 0;

    /**
     * Get the value.
     * @return The value.
     */
    virtual const Value& get() const = 0;

    /**
     * Get the value.
     * @return The value.
     */
    virtual Value& get() = 0;

    /**
     * Get the name.
     * @return The name.
     */
    virtual const Name& getName() const = 0;
  };
}

#endif // #ifndef ELEPHEYE_INPUT_HPP
