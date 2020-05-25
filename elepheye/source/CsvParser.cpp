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
#include <elepheye/source/CsvParser.hpp>

namespace elepheye
{
  namespace source
  {
    CsvParser::CsvParser(const Shared<SourceType>& src)
      : InputFilter(src), src_(src), line_(0), index_(0)
    {
    }

    bool CsvParser::input()
    {
      // No more records.
      if (src_end_ == src_)
      {
        return false;
      }

      // Initialize.
      line_ = src_->getPosition().getLine();
      index_ = 0;
      prepareField();

      // Parse fields until the end of the record.
      do
      {
        switch (src_->get())
        {
        case _T(','):
          ++src_;
          ++index_;
          prepareField();
          break;
        case _T('\n'):
          if (src_end_ != ++src_ && _T('\r') == src_->get())
          {
            ++src_;
          }
          terminateRecord();
          return true;
        case _T('\r'):
          if (src_end_ != ++src_ && _T('\n') == src_->get())
          {
            ++src_;
          }
          terminateRecord();
          return true;
        case _T('"'):
          parseEscapedField();
          break;
        default:
          value_.at(index_) += src_->get();
          ++src_;
          break;
        }
      }
      while (src_end_ != src_);
      terminateRecord();
      return true;
    }

    const CsvParser::ValueType& CsvParser::get() const
    {
      return value_;
    }

    CsvParser::ValueType& CsvParser::get()
    {
      return value_;
    }

    void CsvParser::prepareField()
    {
      // Create or clear the field for the current index.
      if (1 == line_)
      {
        if (value_.size() > index_)
        {
          value_.at(index_).clear();
        }
        else
        {
          value_.push_back(String());
        }
      }
      else
      {
        if (value_.size() > index_)
        {
          value_.at(index_).clear();
        }
        else
        {
          lib::TextPosition position;
          position.setLine(line_);
          Exceptions::User e;
          e.setContext(position.format(getName(), false));
          e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_CSV);
          e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISMATCHED_FIELD_COUNT);
          throw e;
        }
      }
    }

    void CsvParser::terminateRecord()
    {
      // Check the field count.
      if (value_.size() != index_ + 1)
      {
        lib::TextPosition position;
        position.setLine(line_);
        Exceptions::User e;
        e.setContext(position.format(getName(), false));
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_CSV);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISMATCHED_FIELD_COUNT);
        throw e;
      }
    }

    void CsvParser::parseEscapedField()
    {
      // Check the field status.
      if (!value_.at(index_).empty())
      {
        Exceptions::User e;
        e.setContext(src_->getPosition().format(getName()));
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_CSV);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_INVALID_DOUBLE_QUOTE);
        throw e;
      }

      // Save the position of the opening double quote.
      lib::TextPosition opening = src_->getPosition();

      // Parse the field.
      while (src_end_ != ++src_)
      {
        if (_T('"') == src_->get())
        {
          if (src_end_ == ++src_)
          {
            return;
          }
          switch (src_->get())
          {
          case _T(','):
          case _T('\n'):
          case _T('\r'):
            return;
          case _T('"'):
            // Unescape the couple of double quotes.
            value_.at(index_) += src_->get();
            break;
          default:
            {
              Exceptions::User e;
              e.setContext(src_->getPosition().format(getName()));
              e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_CSV);
              e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_INVALID_CHARACTER);
              throw e;
            }
          }
        }
        else
        {
          value_.at(index_) += src_->get();
        }
      }

      // Missing the closing double quote.
      Exceptions::User e;
      e.setContext(opening.format(getName()));
      e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_PARSE_CSV);
      e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_CLOSING_DOUBLE_QUOTE);
      throw e;
    }
  }
}
