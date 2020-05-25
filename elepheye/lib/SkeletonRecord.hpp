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

#ifndef ELEPHEYE_LIB_SKELETON_RECORD_HPP
#define ELEPHEYE_LIB_SKELETON_RECORD_HPP

#include <elepheye/Record.hpp>
#include <vector>

namespace elepheye
{
  namespace lib
  {
    /**
     * Skeleton implementation of Record.
     */
    class SkeletonRecord : public Record
    {
    public:

      const String& getField(std::size_t index) const;

      bool getOption(std::size_t index) const;

      void setOption(std::size_t index);

      /**
       * Get field values.
       * @return The values.
       */
      const std::vector<String>& getFields() const;

      /**
       * Get field values.
       * @return The values.
       */
      std::vector<String>& getFields();

      /**
       * Get option values.
       * @return The values.
       */
      const std::vector<bool>& getOptions() const;

      /**
       * Get option values.
       * @return The values.
       */
      std::vector<bool>& getOptions();

    private:
      std::vector<String> fields_;
      std::vector<bool> options_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_SKELETON_RECORD_HPP
