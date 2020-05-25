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

#include <elepheye/Command.hpp>
#include <elepheye/Exceptions.hpp>
#include <elepheye/ForwardIterator.hpp>
#include <elepheye/filter/Csv.hpp>
#include <elepheye/filter/Rule.hpp>
#include <elepheye/filter/Validator.hpp>
#include <elepheye/lib/Field.hpp>
#include <elepheye/source/Csv.hpp>
#include <elepheye/source/Filesystem.hpp>
#include <elepheye/source/Registry.hpp>
#include <elepheye/source/Wmi.hpp>

namespace elepheye
{
  Shared<Command::RecordInput> Command::createSource(
    const String& type, const std::vector<String>& args)
  {
    // Csv.
    if (_T("csv") == type)
    {
      if (args.empty())
      {
        Exceptions::User e;
        e.setContext(type);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_SOURCE);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_ARGUMENTS);
        throw e;
      }
      if (1 < args.size())
      {
        Exceptions::User e;
        e.setContext(type);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_SOURCE);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_TOO_MANY_ARGUMENTS);
        throw e;
      }
      return Shared<RecordInput>(new source::Csv(args.at(0)));
    }

    // Filesystem.
    if (_T("filesystem") == type)
    {
      if (args.empty())
      {
        Exceptions::User e;
        e.setContext(type);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_SOURCE);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_ARGUMENTS);
        throw e;
      }
      return Shared<RecordInput>(new source::Filesystem(args));
    }

    // Registry.
    if (_T("registry") == type)
    {
      if (args.empty())
      {
        Exceptions::User e;
        e.setContext(type);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_SOURCE);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_ARGUMENTS);
        throw e;
      }
      return Shared<RecordInput>(new source::Registry(args));
    }

    // Wmi.
    if (_T("wmi") == type)
    {
      if (args.empty())
      {
        Exceptions::User e;
        e.setContext(type);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_SOURCE);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_ARGUMENTS);
        throw e;
      }
      if (1 < args.size())
      {
        Exceptions::User e;
        e.setContext(type);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_SOURCE);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_TOO_MANY_ARGUMENTS);
        throw e;
      }
      return Shared<RecordInput>(new source::Wmi(args.at(0)));
    }

    // Unknown.
    Exceptions::User e;
    e.setContext(type);
    e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_SOURCE);
    e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_UNKNOWN_SOURCE_TYPE);
    throw e;
  }

  Shared<Command::RecordInput> Command::createFilter(
    const String& type,
    const std::vector<String>& args,
    const Shared<RecordInput>& src)
  {
    // Csv.
    if (_T("csv") == type)
    {
      if (args.empty())
      {
        Exceptions::User e;
        e.setContext(type);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_FILTER);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_ARGUMENTS);
        throw e;
      }
      if (1 < args.size())
      {
        Exceptions::User e;
        e.setContext(type);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_FILTER);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_TOO_MANY_ARGUMENTS);
        throw e;
      }
      return Shared<RecordInput>(new filter::Csv(args.at(0), src));
    }

    // Rule.
    if (_T("rule") == type)
    {
      if (args.empty())
      {
        Exceptions::User e;
        e.setContext(type);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_FILTER);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISSING_ARGUMENTS);
        throw e;
      }
      if (1 < args.size())
      {
        Exceptions::User e;
        e.setContext(type);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_FILTER);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_TOO_MANY_ARGUMENTS);
        throw e;
      }
      return Shared<RecordInput>(new filter::Rule(args.at(0), src));
    }

    // Unknown.
    Exceptions::User e;
    e.setContext(type);
    e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_CREATE_FILTER);
    e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_UNKNOWN_FILTER_TYPE);
    throw e;
  }

  void Command::input(const Shared<RecordInput>& input)
  {
    // Validate and input all records.
    filter::Validator valid(input);
    while (valid.input())
    {
    }
  }

  void Command::compare(
    const Shared<RecordInput>& from,
    const Shared<RecordInput>& to,
    RecordDiffLogger& logger)
  {
    // Compare field counts.
    if (from->getName().getFieldNames().size()
        != to->getName().getFieldNames().size())
    {
      Exceptions::User e;
      e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_COMPARE_RECORDS);
      e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISMATCHED_FIELD_COUNT);
      throw e;
    }

    // Compare field names.
    for (std::size_t i = 0; from->getName().getFieldNames().size() > i; ++i)
    {
      if (from->getName().getFieldNames().at(i)
          != to->getName().getFieldNames().at(i))
      {
        Exceptions::User e;
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_COMPARE_RECORDS);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_MISMATCHED_FIELD_NAME);
        throw e;
      }
    }

    // Validate and compare all records.
    ForwardIterator<RecordInput> i_from(
      Shared<RecordInput>(new filter::Validator(from)));
    ForwardIterator<RecordInput> i_to(
      Shared<RecordInput>(new filter::Validator(to)));
    const ForwardIterator<RecordInput> end;
    std::list<std::size_t> changed_fields;
    const std::size_t field_count = from->getName().getFieldNames().size();
    while (i_from != end && i_to != end)
    {
      // Compare key fields.
      const int comparison
        = lib::Field::compareKey(i_from->getField(0), i_to->getField(0));
      if (!comparison)
      {
        // Compare non-key fields.
        changed_fields.clear();
        for (std::size_t i = 1; field_count > i; ++i)
        {
          if (i_from->getField(i) != i_to->getField(i))
          {
            changed_fields.push_back(i);
          }
        }
        if (!changed_fields.empty())
        {
          logger.logChanged(*i_from, *i_to, changed_fields);
        }
        ++i_from;
        ++i_to;
      }
      else if (0 > comparison)
      {
        logger.logRemoved(*i_from);
        ++i_from;
      }
      else
      {
        logger.logAdded(*i_to);
        ++i_to;
      }
    }
    while (i_from != end)
    {
      logger.logRemoved(*i_from);
      ++i_from;
    }
    while (i_to != end)
    {
      logger.logAdded(*i_to);
      ++i_to;
    }
  }
}
