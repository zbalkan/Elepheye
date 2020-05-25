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

#ifndef ELEPHEYE_COMMAND_HPP
#define ELEPHEYE_COMMAND_HPP

#include <elepheye/Input.hpp>
#include <elepheye/Record.hpp>
#include <elepheye/RecordDiffLogger.hpp>
#include <elepheye/RecordName.hpp>
#include <elepheye/Shared.hpp>
#include <vector>

namespace elepheye
{
  /**
   * Static class for application commands.
   */
  class Command
  {
  public:

    /**
     * Alias to input records.
     */
    typedef Input<Record, RecordName> RecordInput;

    /**
     * Create an input for the record source.
     * @param type - The type name of the input.
     * @param args - Arguments for the input.
     * @return The input.
     */
    static Shared<RecordInput> createSource(
      const String& type, const std::vector<String>& args);

    /**
     * Create an input for the record filter.
     * @param type - The type name of the input.
     * @param args - Arguments for the input.
     * @param src - The source instance.
     * @return The input.
     */
    static Shared<RecordInput> createFilter(
      const String& type,
      const std::vector<String>& args,
      const Shared<RecordInput>& src);

    /**
     * Input all records.
     * @param input - The input.
     */
    static void input(const Shared<RecordInput>& input);

    /**
     * Compare all records.
     * @param from - The input before changed.
     * @param to - The input after changed.
     * @param logger - The logger.
     */
    static void compare(
      const Shared<RecordInput>& from,
      const Shared<RecordInput>& to,
      RecordDiffLogger& logger);
  };
}

#endif // #ifndef ELEPHEYE_COMMAND_HPP
