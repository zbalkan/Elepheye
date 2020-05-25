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

#ifndef ELEPHEYE_RECORD_HPP
#define ELEPHEYE_RECORD_HPP

#include <elepheye/String.hpp>

namespace elepheye
{
  /**
   * Interface for a record.
   * A record must have one key field, and zero or more non-key fields.
   * The index number of the key field must be zero.
   * The value of the key field must be unique in records.
   * Records must be sorted by lib::Field::compareKey.
   */
  class Record
  {
  public:

    /**
     * Destructor.
     */
    virtual ~Record() throw()
    {
    }

    /**
     * Get a field value.
     * @param index - The zero-based index number for the field.
     * @return The value.
     */
    virtual const String& getField(std::size_t index) const = 0;

    /**
     * Get an option.
     * @param index - The zero-based index number for the option.
     * @return True if the option is set.
     */
    virtual bool getOption(std::size_t index) const = 0;

    /**
     * Set an option.
     * @param index - The zero-based index number for the option.
     */
    virtual void setOption(std::size_t index) = 0;
  };
}

#endif // #ifndef ELEPHEYE_RECORD_HPP
