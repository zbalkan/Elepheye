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
#include <elepheye/lib/TextInput.hpp>

namespace elepheye
{
  namespace lib
  {
    TextInput::TextInput(const Shared<SourceType>& src)
      : InputFilter(src), newline_(NEWLINE_NONE)
    {
#ifdef UNICODE
      // Check the BOM.
      if (!getSource()->input())
      {
        Exceptions::User e;
        e.setContext(getName());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_TEXT);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_BOM);
        throw e;
      }
      if (0xfffe == getSource()->get())
      {
        Exceptions::User e;
        e.setContext(getName());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_TEXT);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_BOM);
        throw e;
      }
      if (0xfeff != getSource()->get())
      {
        Exceptions::User e;
        e.setContext(getName());
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_TEXT);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_BOM);
        throw e;
      }
#endif // #ifdef UNICODE
    }

    bool TextInput::input()
    {
      // Input the source.
      if (!getSource()->input())
      {
        return false;
      }

      // Save the character.
      value_.set(getSource()->get());

      // Update the position.
      if (_T('\r') == value_.get())
      {
        if (NEWLINE_NONE == newline_)
        {
          newline_ = NEWLINE_CR;
        }
        else if (NEWLINE_LF == newline_)
        {
          newline_ = NEWLINE_LFCR;
        }
        else
        {
          newline_ = NEWLINE_CR;
          value_.getPosition().setLine(value_.getPosition().getLine() + 1);
          value_.getPosition().setColumn(0);
        }
      }
      else if (_T('\n') == value_.get())
      {
        if (NEWLINE_NONE == newline_)
        {
          newline_ = NEWLINE_LF;
        }
        else if (NEWLINE_CR == newline_)
        {
          newline_ = NEWLINE_CRLF;
        }
        else
        {
          newline_ = NEWLINE_LF;
          value_.getPosition().setLine(value_.getPosition().getLine() + 1);
          value_.getPosition().setColumn(0);
        }
      }
      else if (newline_)
      {
        newline_ = NEWLINE_NONE;
        value_.getPosition().setLine(value_.getPosition().getLine() + 1);
        value_.getPosition().setColumn(0);
      }
      value_.getPosition().setColumn(value_.getPosition().getColumn() + 1);
      return true;
    }

    const TextInput::ValueType& TextInput::get() const
    {
      return value_;
    }

    TextInput::ValueType& TextInput::get()
    {
      return value_;
    }
  }
}
