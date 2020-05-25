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

#ifndef ELEPHEYE_FILTER_RULE_MATCH_HPP
#define ELEPHEYE_FILTER_RULE_MATCH_HPP

#include <elepheye/Record.hpp>
#include <elepheye/filter/RuleToken.hpp>
#include <elepheye/lib/Wildcard.hpp>

namespace elepheye
{
  namespace filter
  {
    /**
     * Class for a match of the rule text.
     */
    class RuleMatch
    {
    public:

      /**
       * Constructor.
       */
      RuleMatch();

      /**
       * Set the field to match.
       * @param index - The zero-based index number for the field.
       */
      void setField(std::size_t index);

      /**
       * Invert the result.
       */
      void invert();

      /**
       * Set the pattern.
       * @param pattern - The pattern.
       * @param ignores_case - True if ignores case.
       */
      void setPattern(const RuleToken& pattern, bool ignores_case);

      /**
       * Match a record.
       * @param record - The record.
       * @return True if matches.
       */
      bool match(const Record& record) const;

    private:
      std::size_t field_;
      bool inverts_;
      lib::Wildcard pattern_;
    };
  }
}

#endif // #ifndef ELEPHEYE_FILTER_RULE_MATCH_HPP
