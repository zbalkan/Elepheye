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

#ifndef CUI_COMMAND_LINE_TOKEN_HPP
#define CUI_COMMAND_LINE_TOKEN_HPP

#include <elepheye/String.hpp>

namespace cui
{
  /**
   * Class for the value of CommandLineTokenizer.
   */
  class CommandLineToken
  {
  public:

    /**
     * Constructor.
     */
    CommandLineToken();

    /**
     * Get the string.
     * @return The string.
     */
    const elepheye::String& get() const;

    /**
     * Set the string.
     * @param s - The string.
     */
    void set(const elepheye::String& s);

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

  private:
    elepheye::String s_;
    bool is_switch_;
  };
}

#endif // #ifndef CUI_COMMAND_LINE_TOKEN_HPP
