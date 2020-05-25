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

#ifndef CUI_RECORD_DIFF_LOGGER_HPP
#define CUI_RECORD_DIFF_LOGGER_HPP

#include <elepheye/RecordDiffLogger.hpp>
#include <elepheye/RecordName.hpp>

namespace cui
{
  /**
   * Implementation of elepheye::RecordDiffLogger for the console.
   */
  class RecordDiffLogger : public elepheye::RecordDiffLogger
  {
  public:

    /**
     * Constructor.
     * @param name - The name of records.
     */
    explicit RecordDiffLogger(const elepheye::RecordName& name);

    void logRemoved(const elepheye::Record& removed);

    void logAdded(const elepheye::Record& added);

    void logChanged(
      const elepheye::Record& from,
      const elepheye::Record& to,
      const std::list<std::size_t>& fields);

  private:
    std::vector<elepheye::String> field_names_;
    elepheye::String label_removed_;
    elepheye::String label_added_;
    elepheye::String label_changed_;
    elepheye::String label_changed_field_;
    elepheye::String label_changed_from_;
    elepheye::String label_changed_to_;
  };
}

#endif // #ifndef CUI_RECORD_DIFF_LOGGER_HPP
