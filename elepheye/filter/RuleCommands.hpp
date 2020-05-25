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

#ifndef ELEPHEYE_FILTER_RULE_COMMANDS_HPP
#define ELEPHEYE_FILTER_RULE_COMMANDS_HPP

#include <list>

namespace elepheye
{
  namespace filter
  {
    /**
     * Class for commands of the rule text.
     */
    class RuleCommands
    {
    public:

      /**
       * Constructor.
       */
      RuleCommands();

      /**
       * Get whether exits rule entries.
       * @return True if exits.
       */
      bool exits() const;

      /**
       * Set whether exits rule entries.
       * @param exits - True if exits.
       */
      void setExits(bool exits = true);

      /**
       * Get whether drops the record.
       * @return True if drops.
       */
      bool drops() const;

      /**
       * Set whether drops the record.
       * @param drops - True if drops.
       */
      void setDrops(bool drops = true);

      /**
       * Get fields to disable.
       * @return Zero-based index numbers for fields.
       */
      const std::list<std::size_t>& getFieldsToDisable() const;

      /**
       * Get fields to disable.
       * @return Zero-based index numbers for fields.
       */
      std::list<std::size_t>& getFieldsToDisable();

      /**
       * Get fields to ignore case.
       * @return Zero-based index numbers for fields.
       */
      const std::list<std::size_t>& getFieldsToIgnoreCase() const;

      /**
       * Get fields to ignore case.
       * @return Zero-based index numbers for fields.
       */
      std::list<std::size_t>& getFieldsToIgnoreCase();

      /**
       * Get options to set.
       * @return Zero-based index numbers for options.
       */
      const std::list<std::size_t>& getOptionsToSet() const;

      /**
       * Get options to set.
       * @return Zero-based index numbers for options.
       */
      std::list<std::size_t>& getOptionsToSet();

    private:
      bool exits_;
      bool drops_;
      std::list<std::size_t> fields_to_disable_;
      std::list<std::size_t> fields_to_ignore_case_;
      std::list<std::size_t> options_to_set_;
    };
  }
}

#endif // #ifndef ELEPHEYE_FILTER_RULE_COMMANDS_HPP
