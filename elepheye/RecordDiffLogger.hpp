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

#ifndef ELEPHEYE_RECORD_DIFF_LOGGER_HPP
#define ELEPHEYE_RECORD_DIFF_LOGGER_HPP

#include <elepheye/Record.hpp>
#include <list>

namespace elepheye
{
  /**
   * Interface to log differentials of records.
   */
  class RecordDiffLogger
  {
  public:

    /**
     * Destructor.
     */
    virtual ~RecordDiffLogger() throw()
    {
    }

    /**
     * Log a removed record.
     * @param removed - The record.
     */
    virtual void logRemoved(const Record& removed) = 0;

    /**
     * Log an added record.
     * @param added - The record.
     */
    virtual void logAdded(const Record& added) = 0;

    /**
     * Log a changed record.
     * @param from - The record before changed.
     * @param to - The record after changed.
     * @param fields - Index numbers of changed fields.
     */
    virtual void logChanged(
      const Record& from,
      const Record& to,
      const std::list<std::size_t>& fields) = 0;
  };
}

#endif // #ifndef ELEPHEYE_RECORD_DIFF_LOGGER_HPP
