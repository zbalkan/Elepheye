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

#ifndef ELEPHEYE_RECORD_NAME_HPP
#define ELEPHEYE_RECORD_NAME_HPP

#include <elepheye/String.hpp>
#include <vector>

namespace elepheye
{
  /**
   * Class to define names of Record.
   */
  class RecordName
  {
  public:

    /**
     * Get field names.
     * @return The names.
     */
    const std::vector<String>& getFieldNames() const;

    /**
     * Get field names.
     * @return The names.
     */
    std::vector<String>& getFieldNames();

    /**
     * Get option names.
     * @return The names.
     */
    const std::vector<String>& getOptionNames() const;

    /**
     * Get option names.
     * @return The names.
     */
    std::vector<String>& getOptionNames();

  private:
    std::vector<String> field_names_;
    std::vector<String> option_names_;
  };
}

#endif // #ifndef ELEPHEYE_RECORD_NAME_HPP
