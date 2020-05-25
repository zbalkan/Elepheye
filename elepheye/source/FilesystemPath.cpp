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
#include <elepheye/source/FilesystemPath.hpp>

namespace elepheye
{
  namespace source
  {
    namespace
    {
      // Separators for the path.
      const ::TCHAR sep_ = _T('\\');
      const ::TCHAR stream_sep_ = _T(':');
    }

    FilesystemPath::FilesystemPath(const String& path)
    {
      // Check the path.
      if (path.empty())
      {
        Exceptions::User e;
        e.setContext(path_);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILESYSTEM);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_INVALID_PATH);
        throw e;
      }

      // Convert to the extended-length path.
      toExtended(path);
    }

    FilesystemPath::FilesystemPath(
      const FilesystemPath& parent, const String& child) : path_(parent.path_)
    {
      // Check the child name.
      if (child.empty())
      {
        Exceptions::Application e;
        e.setContext(path_);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILESYSTEM);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_INVALID_PATH);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }

      // Append the child name.
      if (child.at(0) != stream_sep_ && path_.at(path_.length() - 1) != sep_)
      {
        path_ += sep_;
      }
      path_ += child;

      // Check the relation.
      if (parent.isStream())
      {
        Exceptions::Application e;
        e.setContext(path_);
        e.setIssue(ELEPHEYE_RESOURCE_IDS_ISSUE_READ_FILESYSTEM);
        e.setReason(ELEPHEYE_RESOURCE_IDS_REASON_INVALID_PATH);
        e.setSource(_T(__FILE__), __LINE__);
        throw e;
      }
    }

    const String& FilesystemPath::get() const
    {
      return path_;
    }

    bool FilesystemPath::isStream() const
    {
      return path_.find(stream_sep_, path_.rfind(sep_)) != String::npos;
    }

    void FilesystemPath::toExtended(const String& path)
    {
      const String prefix_extended(_T("\\\\?\\"));
      const String prefix_extended_unc(_T("\\\\?\\UNC\\"));
      const String prefix_unc(_T("\\\\"));

      // Prepend the extended-length path prefix.
      if (!path.compare(0, prefix_extended.length(), prefix_extended))
      {
        // Already prefixed.
        path_ = path;
      }
      else if (!path.compare(0, prefix_unc.length(), prefix_unc))
      {
        // The path is UNC.
        path_ = prefix_extended_unc;
        path_ += path.substr(prefix_unc.length());
      }
      else
      {
        path_ = prefix_extended;
        path_ += path;
      }

      // Remove last separators.
      String::size_type last = path_.find_last_not_of(sep_);
      if (String::npos != last)
      {
        path_.erase(last + 1);
      }

      // Append the separator to the root directory.
      if (path_.find(sep_, prefix_extended.length()) == String::npos)
      {
        path_ += sep_;
      }
    }
  }
}
