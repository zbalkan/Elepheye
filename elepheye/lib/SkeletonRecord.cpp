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

#include <elepheye/lib/SkeletonRecord.hpp>

namespace elepheye
{
  namespace lib
  {
    const String& SkeletonRecord::getField(std::size_t index) const
    {
      return fields_.at(index);
    }

    bool SkeletonRecord::getOption(std::size_t index) const
    {
      return options_.at(index);
    }

    void SkeletonRecord::setOption(std::size_t index)
    {
      options_.at(index) = true;
    }

    const std::vector<String>& SkeletonRecord::getFields() const
    {
      return fields_;
    }

    std::vector<String>& SkeletonRecord::getFields()
    {
      return fields_;
    }

    const std::vector<bool>& SkeletonRecord::getOptions() const
    {
      return options_;
    }

    std::vector<bool>& SkeletonRecord::getOptions()
    {
      return options_;
    }
  }
}
