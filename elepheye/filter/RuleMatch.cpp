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

#include <elepheye/Exceptions.hpp>
#include <elepheye/filter/RuleMatch.hpp>

namespace elepheye
{
  namespace filter
  {
    RuleMatch::RuleMatch()
      : field_(0), inverts_(false), pattern_(_T("*"), false)
    {
    }

    void RuleMatch::setField(std::size_t index)
    {
      field_ = index;
    }

    void RuleMatch::invert()
    {
      inverts_ = !inverts_;
    }

    void RuleMatch::setPattern(const RuleToken& pattern, bool ignores_case)
    {
      // Make the pattern.
      pattern_ = lib::Wildcard(pattern.get(), ignores_case);
    }

    bool RuleMatch::match(const Record& record) const
    {
      // Evaluate the pattern.
      return pattern_.match(record.getField(field_)) != inverts_;
    }
  }
}
