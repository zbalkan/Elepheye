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

#ifndef ELEPHEYE_FILTER_RULE_HPP
#define ELEPHEYE_FILTER_RULE_HPP

#include <elepheye/InputFilter.hpp>
#include <elepheye/Record.hpp>
#include <elepheye/RecordName.hpp>
#include <elepheye/filter/RuleEntry.hpp>
#include <list>

namespace elepheye
{
  namespace filter
  {
    /**
     * Implementation of InputFilter and Record for a rule text file.
     */
    class Rule : public InputFilter<Record, RecordName>, public Record
    {
    public:

      /**
       * Constructor.
       * @param path - The path to the rule text.
       * @param src - The source instance.
       */
      Rule(const String& path, const Shared<SourceType>& src);

      bool input();

      const ValueType& get() const;

      ValueType& get();

      const String& getField(std::size_t index) const;

      bool getOption(std::size_t index) const;

      void setOption(std::size_t index);

    private:
      void doCommands(const RuleCommands& cmds);
      std::list<RuleEntry> entries_;
      mutable std::vector<String> fields_; // Mutable for getField optimization.
      std::vector<bool> disables_;
      std::vector<bool> ignores_case_;
    };
  }
}

#endif // #ifndef ELEPHEYE_FILTER_RULE_HPP
