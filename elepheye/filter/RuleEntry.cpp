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

#include <elepheye/filter/RuleEntry.hpp>

namespace elepheye
{
  namespace filter
  {
    const RuleCommands& RuleEntry::getCommands() const
    {
      return commands_;
    }

    RuleCommands& RuleEntry::getCommands()
    {
      return commands_;
    }

    void RuleEntry::addMatch(const RuleMatch& match)
    {
      matches_.push_back(match);
    }

    bool RuleEntry::match(const Record& record) const
    {
      // Match all.
      for (
        std::list<RuleMatch>::const_iterator i = matches_.begin();
        matches_.end() != i;
        ++i)
      {
        if (!i->match(record))
        {
          return false;
        }
      }
      return true;
    }
  }
}
