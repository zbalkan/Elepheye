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

#ifndef ELEPHEYE_EXCEPTION_PROXY_HPP
#define ELEPHEYE_EXCEPTION_PROXY_HPP

#include <elepheye/ExceptionLogger.hpp>

namespace elepheye
{
  /**
   * Static class to proxy exceptions.
   */
  class ExceptionProxy
  {
  public:

    /**
     * Get the logger.
     * The initial logger is 0 (does not exist).
     * @return The logger.
     */
    static ExceptionLogger* getLogger() throw();

    /**
     * Set the logger.
     * @param logger - The logger.
     */
    static void setLogger(ExceptionLogger* logger) throw();

    /**
     * Log an exception.
     * @param e - The exception.
     * @param level - The level.
     */
    static void log(
      const Exception& e,
      ExceptionLogger::Level level = ExceptionLogger::LEVEL_ERROR) throw();

    /**
     * Get the max level which has been logged.
     * The initial level is the lowest level.
     * @return The level.
     */
    static ExceptionLogger::Level getMaxLevel() throw();

    /**
     * Set the max level which has been logged.
     * @param level - The level.
     */
    static void setMaxLevel(ExceptionLogger::Level level) throw();

    /**
     * Get the verbose level to be logged.
     * The initial level is LEVEL_ERROR, and lower levels are ignored.
     * @return The level.
     */
    static ExceptionLogger::Level getVerboseLevel() throw();

    /**
     * Set the verbose level to be logged.
     * @param level - The level.
     */
    static void setVerboseLevel(ExceptionLogger::Level level) throw();
  };
}

#endif // #ifndef ELEPHEYE_EXCEPTION_PROXY_HPP
