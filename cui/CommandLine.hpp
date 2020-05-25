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

#ifndef CUI_COMMAND_LINE_HPP
#define CUI_COMMAND_LINE_HPP

#include <cui/CommandLineTokenizer.hpp>
#include <elepheye/Command.hpp>
#include <elepheye/ForwardIterator.hpp>

namespace cui
{
  /**
   * Class to parse command-line arguments.
   */
  class CommandLine
  {
  public:

    /**
     * Constructor.
     * @param argc - The count of arguments.
     * @param argv - Values of arguments.
     */
    CommandLine(int argc, const ::TCHAR* const* argv);

  private:
    void parseSource();
    void parseFilter();
    elepheye::ForwardIterator<CommandLineTokenizer> token_;
    elepheye::ForwardIterator<CommandLineTokenizer> token_end_;
    elepheye::Shared<elepheye::Command::RecordInput> input_;
    elepheye::Shared<elepheye::Command::RecordInput> input_to_compare_;
  };
}

#endif // #ifndef CUI_COMMAND_LINE_HPP
