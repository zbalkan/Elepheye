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
#include <elepheye/filter/RuleParser.hpp>

namespace elepheye
{
  namespace filter
  {
    RuleParser::RuleParser(
      const RecordName& name, const Shared<SourceType>& src)
      : InputFilter(src), name_(name)
    {
    }

    bool RuleParser::input()
    {
      // No more tokens.
      if (!getSource()->input())
      {
        return false;
      }

      // Initialize.
      value_ = RuleEntry();
      token_ = getSource()->get().begin();
      token_end_ = getSource()->get().end();

      // Parse matches.
      do
      {
        RuleMatch match;
        // Parse the field name.
        match.setField(findField());
        // Parse match options.
        bool ignores_case = false;
        while (token_end_ != ++token_ && token_->isSwitch())
        {
          if (_T("ignore-case") == token_->get())
          {
            ignores_case = true;
          }
          else if (_T("not") == token_->get())
          {
            match.invert();
          }
          else
          {
            Exceptions::User e;
            e.setContext(token_->getPosition().format(getName()));
            e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_RULE);
            e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_UNKNOWN_MATCH_OPTION);
            throw e;
          }
        }
        // No more tokens.
        if (token_end_ == token_)
        {
          Exceptions::User e;
          e.setContext(
            getSource()->get().front().getPosition().format(getName(), false));
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_RULE);
          e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_MATCH_PATTERN);
          throw e;
        }
        // Parse the pattern.
        match.setPattern(*token_, ignores_case);
        // Add a match.
        value_.addMatch(match);
      }
      while (token_end_ != ++token_ && hasSwitch(_T("and")));

      // No more tokens.
      if (token_end_ == token_)
      {
        Exceptions::User e;
        e.setContext(
          getSource()->get().front().getPosition().format(getName(), false));
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_RULE);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_COMMANDS);
        throw e;
      }

      // Parse commands.
      do
      {
        if (!token_->isSwitch())
        {
          Exceptions::User e;
          e.setContext(token_->getPosition().format(getName()));
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_RULE);
          e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_INVALID_TOKEN);
          throw e;
        }
        else if (_T("exit") == token_->get())
        {
          value_.getCommands().setExits();
          ++token_;
        }
        else if (_T("drop") == token_->get())
        {
          value_.getCommands().setDrops();
          ++token_;
        }
        else if (_T("disable") == token_->get())
        {
          // Parse field names to disable.
          ++token_;
          do
          {
            const std::size_t field = findField();
            if (field)
            {
              value_.getCommands().getFieldsToDisable().push_back(field);
            }
            else
            {
              // Drops the record if the key field is disabled.
              value_.getCommands().setDrops();
            }
          }
          while (token_end_ != ++token_ && !token_->isSwitch());
        }
        else if (_T("ignore-case") == token_->get())
        {
          // Parse field names to ignore case.
          ++token_;
          do
          {
            value_.getCommands().getFieldsToIgnoreCase().push_back(findField());
          }
          while (token_end_ != ++token_ && !token_->isSwitch());
        }
        else if (_T("set") == token_->get())
        {
          // Parse option names to set.
          ++token_;
          do
          {
            value_.getCommands().getOptionsToSet().push_back(findOption());
          }
          while (token_end_ != ++token_ && !token_->isSwitch());
        }
        else
        {
          Exceptions::User e;
          e.setContext(token_->getPosition().format(getName()));
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_RULE);
          e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_UNKNOWN_COMMAND);
          throw e;
        }
      }
      while (token_end_ != token_);
      return true;
    }

    const RuleParser::ValueType& RuleParser::get() const
    {
      return value_;
    }

    RuleParser::ValueType& RuleParser::get()
    {
      return value_;
    }

    bool RuleParser::hasSwitch(const String& text)
    {
      if (token_->isSwitch() && token_->get() == text)
      {
        ++token_;
        return true;
      }
      return false;
    }

    std::size_t RuleParser::findField() const
    {
      // No more tokens.
      if (token_end_ == token_)
      {
        Exceptions::User e;
        e.setContext(
          getSource()->get().front().getPosition().format(getName(), false));
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_RULE);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_FIELD_NAME);
        throw e;
      }

      // Find the field name.
      for (std::size_t i = 0; name_.getFieldNames().size() > i; ++i)
      {
        if (token_->get() == name_.getFieldNames().at(i))
        {
          return i;
        }
      }

      // Field name not found.
      Exceptions::User e;
      e.setContext(token_->getPosition().format(getName()));
      e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_RULE);
      e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_UNKNOWN_FIELD_NAME);
      throw e;
    }

    std::size_t RuleParser::findOption() const
    {
      // No more tokens.
      if (token_end_ == token_)
      {
        Exceptions::User e;
        e.setContext(
          getSource()->get().front().getPosition().format(getName(), false));
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_RULE);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_OPTION_NAME);
        throw e;
      }

      // Find the option name.
      for (std::size_t i = 0; name_.getOptionNames().size() > i; ++i)
      {
        if (token_->get() == name_.getOptionNames().at(i))
        {
          return i;
        }
      }

      // Option name not found.
      Exceptions::User e;
      e.setContext(token_->getPosition().format(getName()));
      e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_RULE);
      e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_UNKNOWN_OPTION_NAME);
      throw e;
    }
  }
}
