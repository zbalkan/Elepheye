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

#include <cui/CommandLine.hpp>
#include <cui/Console.hpp>
#include <cui/RecordDiffLogger.hpp>
#include <cui/RecordViewer.hpp>
#include <cui/resource.h>
#include <elepheye/ExceptionProxy.hpp>
#include <elepheye/Exceptions.hpp>

namespace cui
{
  CommandLine::CommandLine(int argc, const ::TCHAR* const* argv)
    : token_(
        elepheye::Shared<CommandLineTokenizer>(
          new CommandLineTokenizer(argc, argv)))
  {
    // No arguments.
    if (token_end_ == token_)
    {
      elepheye::Exception e;
      e.setIssue(CUI_RESOURCE_IDS_ISSUE_PARSE_ARGUMENTS);
      e.setReason(CUI_RESOURCE_IDS_REASON_MISSING_SOURCE_TYPE);
      elepheye::ExceptionProxy::log(e);
      Console::err
        << std::endl
        << elepheye::string_load(CUI_RESOURCE_IDS_LABEL_HELP)
        << std::endl;
      return;
    }

    // Parse options.
    while (token_end_ != token_ && token_->isSwitch())
    {
      if (_T("V") == token_->get())
      {
        // Print the version.
        Console::out
          << elepheye::string_load(CUI_RESOURCE_IDS_LABEL_VERSION) << std::endl;
        return;
      }
      else if (_T("h") == token_->get())
      {
        // Print the help.
        Console::out
          << elepheye::string_load(CUI_RESOURCE_IDS_LABEL_HELP) << std::endl;
        return;
      }
      else if (_T("w") == token_->get())
      {
        // Enable warnings.
        elepheye::ExceptionProxy::setVerboseLevel(
          elepheye::ExceptionLogger::LEVEL_WARNING);
        ++token_;
      }
      else
      {
        elepheye::Exceptions::User e;
        e.setContext(token_->get());
        e.setIssue(CUI_RESOURCE_IDS_ISSUE_PARSE_ARGUMENTS);
        e.setReason(CUI_RESOURCE_IDS_REASON_UNKNOWN_OPTION);
        throw e;
      }
    }

    // Parse the first source.
    parseSource();

    // Parse commands.
    while (token_end_ != token_ && token_->isSwitch())
    {
      if (_T("c") == token_->get())
      {
        // Parse the second source to compare.
        ++token_;
        parseSource();
      }
      else if (_T("f") == token_->get())
      {
        // Parse the filter.
        ++token_;
        parseFilter();
      }
      else
      {
        elepheye::Exceptions::User e;
        e.setContext(token_->get());
        e.setIssue(CUI_RESOURCE_IDS_ISSUE_PARSE_ARGUMENTS);
        e.setReason(CUI_RESOURCE_IDS_REASON_UNKNOWN_COMMAND);
        throw e;
      }
    }

    // Run the command.
    if (input_to_compare_)
    {
      RecordDiffLogger logger(input_to_compare_->getName());
      elepheye::Command::compare(input_to_compare_, input_, logger);
    }
    else
    {
      elepheye::Command::input(input_);
    }
  }

  void CommandLine::parseSource()
  {
    // Check whether the second source is already created.
    if (input_to_compare_)
    {
      elepheye::Exceptions::User e;
      e.setIssue(CUI_RESOURCE_IDS_ISSUE_PARSE_ARGUMENTS);
      e.setReason(CUI_RESOURCE_IDS_REASON_TOO_MANY_SOURCES);
      throw e;
    }

    // No more arguments.
    if (token_end_ == token_)
    {
      elepheye::Exceptions::User e;
      e.setIssue(CUI_RESOURCE_IDS_ISSUE_PARSE_ARGUMENTS);
      if (input_)
      {
        e.setReason(CUI_RESOURCE_IDS_REASON_MISSING_SOURCE_TYPE_TO_COMPARE);
      }
      else
      {
        e.setReason(CUI_RESOURCE_IDS_REASON_MISSING_SOURCE_TYPE);
      }
      throw e;
    }

    // Get the type.
    elepheye::String type(token_->get());

    // Get arguments.
    std::vector<elepheye::String> args;
    while (token_end_ != ++token_ && !token_->isSwitch())
    {
      args.push_back(token_->get());
    }

    // Create the input.
    if (input_)
    {
      input_to_compare_ = elepheye::Command::createSource(type, args);
    }
    else
    {
      input_ = elepheye::Command::createSource(type, args);
    }
  }

  void CommandLine::parseFilter()
  {
    // No more arguments.
    if (token_end_ == token_)
    {
      elepheye::Exceptions::User e;
      e.setIssue(CUI_RESOURCE_IDS_ISSUE_PARSE_ARGUMENTS);
      e.setReason(CUI_RESOURCE_IDS_REASON_MISSING_FILTER_TYPE);
      throw e;
    }

    // Get the type.
    elepheye::String type(token_->get());

    // Get arguments.
    std::vector<elepheye::String> args;
    while (token_end_ != ++token_ && !token_->isSwitch())
    {
      args.push_back(token_->get());
    }

    // Select the source.
    elepheye::Shared<elepheye::Command::RecordInput> src
      = input_to_compare_ ? input_to_compare_ : input_;

    // Create the filter.
    elepheye::Shared<elepheye::Command::RecordInput> filter;
    if (_T("console") == type)
    {
      if (!args.empty())
      {
        elepheye::Exceptions::User e;
        e.setContext(type);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_FILTER);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_TOO_MANY_ARGUMENTS);
        throw e;
      }
      filter = elepheye::Shared<
        elepheye::Command::RecordInput>(new RecordViewer(src));
    }
    else
    {
      filter = elepheye::Command::createFilter(type, args, src);
    }

    // Save the filter.
    if (input_to_compare_)
    {
      input_to_compare_ = filter;
    }
    else
    {
      input_ = filter;
    }
  }
}
