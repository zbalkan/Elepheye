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

#include <cui/CommandLineTokenizer.hpp>

namespace cui
{
  CommandLineTokenizer::CommandLineTokenizer(
    int argc, const ::TCHAR* const* argv)
    : args_(&argv[1], &argv[argc]), name_(_T("Command-line"))
  {
  }

  bool CommandLineTokenizer::input()
  {
    // No more tokens.
    if (args_.empty())
    {
      return false;
    }

    // Initialize.
    value_ = CommandLineToken();

    // Parse the switch.
    if (!args_.front().compare(0, 2, _T("--")))
    {
      // Parse the long switch.
      args_.front().erase(0, 2);
      value_.setIsSwitch(true);
    }
    else if (!args_.front().compare(0, 1, _T("-")))
    {
      // Parse the short switch.
      switch (args_.front().length())
      {
      case 1:
        args_.front().clear();
        break;
      case 2:
        args_.front().erase(0, 1);
        break;
      default:
        {
          // Split short switches.
          elepheye::String token(args_.front().substr(1, 1));
          if (!args_.front().compare(2, 1, _T("-")))
          {
            args_.front().erase(0, 2);
          }
          else
          {
            args_.front().erase(1, 1);
          }
          args_.push_front(token);
        }
        break;
      }
      value_.setIsSwitch(true);
    }

    // Set the text.
    value_.set(args_.front());
    args_.pop_front();
    return true;
  }

  const CommandLineTokenizer::ValueType& CommandLineTokenizer::get() const
  {
    return value_;
  }

  CommandLineTokenizer::ValueType& CommandLineTokenizer::get()
  {
    return value_;
  }

  const CommandLineTokenizer::NameType& CommandLineTokenizer::getName() const
  {
    return name_;
  }
}
