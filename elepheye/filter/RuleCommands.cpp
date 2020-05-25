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

#include <elepheye/filter/RuleCommands.hpp>

namespace elepheye
{
  namespace filter
  {
    RuleCommands::RuleCommands() : exits_(false), drops_(false)
    {
    }

    bool RuleCommands::exits() const
    {
      return exits_;
    }

    void RuleCommands::setExits(bool exits)
    {
      exits_ = exits;
    }

    bool RuleCommands::drops() const
    {
      return drops_;
    }

    void RuleCommands::setDrops(bool drops)
    {
      drops_ = drops;
    }

    const std::list<std::size_t>& RuleCommands::getFieldsToDisable() const
    {
      return fields_to_disable_;
    }

    std::list<std::size_t>& RuleCommands::getFieldsToDisable()
    {
      return fields_to_disable_;
    }

    const std::list<std::size_t>& RuleCommands::getFieldsToIgnoreCase() const
    {
      return fields_to_ignore_case_;
    }

    std::list<std::size_t>& RuleCommands::getFieldsToIgnoreCase()
    {
      return fields_to_ignore_case_;
    }

    const std::list<std::size_t>& RuleCommands::getOptionsToSet() const
    {
      return options_to_set_;
    }

    std::list<std::size_t>& RuleCommands::getOptionsToSet()
    {
      return options_to_set_;
    }
  }
}
