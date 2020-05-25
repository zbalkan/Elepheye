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

#ifndef ELEPHEYE_FILTER_RULE_TOKEN_HPP
#define ELEPHEYE_FILTER_RULE_TOKEN_HPP

#include <elepheye/String.hpp>
#include <elepheye/lib/TextPosition.hpp>

namespace elepheye
{
  namespace filter
  {
    /**
     * Class for the value of RuleTokenizer.
     */
    class RuleToken
    {
    public:

      /**
       * Constructor.
       */
      RuleToken();

      /**
       * Get the text.
       * @return The text.
       */
      const String& get() const;

      /**
       * Get the text.
       * @return The text.
       */
      String& get();

      /**
       * Get whether the token is quoted.
       * @return True if quoted.
       */
      bool isQuoted() const;

      /**
       * Set whether the token is quoted.
       * @param is_quoted - True if quoted.
       */
      void setIsQuoted(bool is_quoted);

      /**
       * Get whether the token is a switch.
       * @return True if a switch.
       */
      bool isSwitch() const;

      /**
       * Set whether the token is a switch.
       * @param is_switch - True if a switch.
       */
      void setIsSwitch(bool is_switch);

      /**
       * Get the position.
       * @return The position.
       */
      const lib::TextPosition& getPosition() const;

      /**
       * Set the position.
       * @param position - The position.
       */
      void setPosition(const lib::TextPosition& position);

    private:
      String text_;
      bool is_switch_;
      bool is_quoted_;
      lib::TextPosition position_;
    };
  }
}

#endif // #ifndef ELEPHEYE_FILTER_RULE_TOKEN_HPP
