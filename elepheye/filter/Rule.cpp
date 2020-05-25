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

#include <algorithm>
#include <elepheye/filter/Rule.hpp>
#include <elepheye/filter/RuleParser.hpp>
#include <elepheye/filter/RuleTokenizer.hpp>
#include <elepheye/lib/Field.hpp>
#include <elepheye/lib/FileInput.hpp>
#include <elepheye/lib/InputBuffer.hpp>
#include <elepheye/lib/TextInput.hpp>

namespace elepheye
{
  namespace filter
  {
    Rule::Rule(const String& path, const Shared<SourceType>& src)
      : InputFilter(src),
        fields_(getSource()->getName().getFieldNames().size()),
        disables_(fields_.size(), false),
        ignores_case_(fields_.size(), false)
    {
      // Parse rule entries.
      RuleParser parser(
        getName(),
        Shared<RuleTokenizer>(
          new RuleTokenizer(
            Shared<lib::TextInput>(
              new lib::TextInput(
                Shared<lib::FileInput<::TCHAR> >(
                  new lib::FileInput<::TCHAR>(
                    Shared<lib::File>(
                      new lib::File(
                        path,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        0,
                        OPEN_EXISTING,
                        FILE_FLAG_SEQUENTIAL_SCAN,
                        0)))))))));
      while (parser.input())
      {
        entries_.push_back(parser.get());
      }
    }

    bool Rule::input()
    {
      // Clear cache fields.
      for (
        std::vector<String>::iterator i = fields_.begin();
        fields_.end() != i;
        ++i)
      {
        i->clear();
      }

      // Match the record and do commands.
      while (getSource()->input())
      {
        bool drops = false;
        std::fill(disables_.begin(), disables_.end(), false);
        std::fill(ignores_case_.begin(), ignores_case_.end(), false);
        for (
          std::list<RuleEntry>::const_iterator i = entries_.begin();
          entries_.end() != i;
          ++i)
        {
          if (i->match(getSource()->get()))
          {
            doCommands(i->getCommands());
            if (i->getCommands().drops())
            {
              drops = true;
            }
            if (i->getCommands().exits())
            {
              break;
            }
          }
        }
        if (!drops)
        {
          return true;
        }
      }
      return false;
    }

    const Rule::ValueType& Rule::get() const
    {
      return *this;
    }

    Rule::ValueType& Rule::get()
    {
      return *this;
    }

    const String& Rule::getField(std::size_t index) const
    {
      // Return the empty field if disables.
      if (disables_.at(index))
      {
        return fields_.at(index);
      }

      // Convert the field to uppercase if ignores case.
      if (ignores_case_.at(index))
      {
        if (fields_.at(index).empty())
        {
          fields_.at(index)
            = lib::Field::toUpper(getSource()->get().getField(index));
        }
        return fields_.at(index);
      }

      // Do nothing.
      return getSource()->get().getField(index);
    }

    bool Rule::getOption(std::size_t index) const
    {
      return getSource()->get().getOption(index);
    }

    void Rule::setOption(std::size_t index)
    {
      getSource()->get().setOption(index);
    }

    void Rule::doCommands(const RuleCommands& commands)
    {
      // Disable fields.
      for (
        std::list<std::size_t>::const_iterator i
          = commands.getFieldsToDisable().begin();
        commands.getFieldsToDisable().end() != i;
        ++i)
      {
        disables_.at(*i) = true;
      }

      // Ignore case fields.
      for (
        std::list<std::size_t>::const_iterator i
          = commands.getFieldsToIgnoreCase().begin();
        commands.getFieldsToIgnoreCase().end() != i;
        ++i)
      {
        ignores_case_.at(*i) = true;
      }

      // Set options.
      for (
        std::list<std::size_t>::const_iterator i
          = commands.getOptionsToSet().begin();
        commands.getOptionsToSet().end() != i;
        ++i)
      {
        getSource()->get().setOption(*i);
      }
    }
  }
}
