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

#ifndef CUI_COMMAND_LINE_TOKENIZER_HPP
#define CUI_COMMAND_LINE_TOKENIZER_HPP

#include <cui/CommandLineToken.hpp>
#include <elepheye/Input.hpp>
#include <list>

namespace cui
{
  /**
   * Implementation of elepheye::Input to tokenize command-line arguments.
   */
  class CommandLineTokenizer
    : public elepheye::Input<CommandLineToken, elepheye::String>
  {
  public:

    /**
     * Constructor.
     * @param argc - The count of arguments.
     * @param argv - Values of arguments.
     */
    CommandLineTokenizer(int argc, const ::TCHAR* const* argv);

    bool input();

    const ValueType& get() const;

    ValueType& get();

    const NameType& getName() const;

  private:
    std::list<elepheye::String> args_;
    ValueType value_;
    NameType name_;
  };
}

#endif // #ifndef CUI_COMMAND_LINE_TOKENIZER_HPP
