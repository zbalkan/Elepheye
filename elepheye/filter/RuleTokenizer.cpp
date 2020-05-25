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
#include <elepheye/filter/RuleTokenizer.hpp>

namespace elepheye
{
  namespace filter
  {
    RuleTokenizer::RuleTokenizer(const Shared<SourceType>& src)
      : InputFilter(src), src_(src)
    {
    }

    bool RuleTokenizer::input()
    {
      // No more tokens.
      if (src_end_ == src_)
      {
        return false;
      }

      // Initialize.
      value_.clear();
      token_ = RuleToken();

      // Parse tokens until the end of the line.
      do
      {
        switch (src_->get())
        {
        case _T('\t'):
        case _T(' '):
          flush();
          ++src_;
          break;
        case _T('\n'):
        case _T('\r'):
          flush();
          ++src_;
          if (!value_.empty())
          {
            return true;
          }
          break;
        case _T('"'):
          flush();
          parseDoubleQuoted();
          flush();
          break;
        case _T('#'):
          parseComment();
          break;
        default:
          append(src_->get());
          ++src_;
          break;
        }
      }
      while (src_end_ != src_);
      flush();
      return !value_.empty();
    }

    const RuleTokenizer::ValueType& RuleTokenizer::get() const
    {
      return value_;
    }

    RuleTokenizer::ValueType& RuleTokenizer::get()
    {
      return value_;
    }

    void RuleTokenizer::append(::TCHAR c)
    {
      // Save the position if new token.
      if (!token_.isQuoted() && token_.get().empty())
      {
        token_.setPosition(src_->getPosition());
      }

      // Append the character to the token.
      token_.get() += c;
    }

    void RuleTokenizer::flush()
    {
      // Add and clear the token if exists.
      if (token_.isQuoted() || !token_.get().empty())
      {
        // Parse the long switch.
        if (!token_.isQuoted() && !token_.get().compare(0, 2, _T("--")))
        {
          token_.get().erase(0, 2);
          token_.setIsSwitch(true);
        }
        value_.push_back(token_);
        token_ = RuleToken();
      }
    }

    void RuleTokenizer::parseDoubleQuoted()
    {
      // Start the token.
      token_.setIsQuoted(true);
      token_.setPosition(src_->getPosition());

      // Parse the token.
      while (src_end_ != ++src_)
      {
        if (_T('"') == src_->get())
        {
          if (src_end_ != ++src_ && _T('"') == src_->get())
          {
            // Unescape the couple of double quotes.
            append(src_->get());
          }
          else
          {
            return;
          }
        }
        else
        {
          append(src_->get());
        }
      }

      // Missing the closing double quote.
      Exceptions::User e;
      e.setContext(token_.getPosition().format(getName()));
      e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_RULE);
      e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_CLOSING_DOUBLE_QUOTE);
      throw e;
    }

    void RuleTokenizer::parseComment()
    {
      // Skip until the end of the file or line.
      while (
        src_end_ != ++src_
        && _T('\n') != src_->get()
        && _T('\r') != src_->get())
      {
      }
    }
  }
}
