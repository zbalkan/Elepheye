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

#include <elepheye/ExceptionProxy.hpp>

namespace elepheye
{
  namespace
  {
    ExceptionLogger* logger_ = 0;
    ExceptionLogger::Level max_level_ = static_cast<ExceptionLogger::Level>(0);
    ExceptionLogger::Level verbose_level_ = ExceptionLogger::LEVEL_ERROR;
  }

  ExceptionLogger* ExceptionProxy::getLogger() throw()
  {
    return logger_;
  }

  void ExceptionProxy::setLogger(ExceptionLogger* logger) throw()
  {
    logger_ = logger;
  }

  void ExceptionProxy::log(
    const Exception& e, ExceptionLogger::Level level) throw()
  {
    // Execute the logger.
    if (logger_ && level >= verbose_level_)
    {
      logger_->logException(e, level);
    }

    // Update the max level.
    if (level > max_level_)
    {
      max_level_ = level;
    }
  }

  ExceptionLogger::Level ExceptionProxy::getMaxLevel() throw()
  {
    return max_level_;
  }

  void ExceptionProxy::setMaxLevel(ExceptionLogger::Level level) throw()
  {
    max_level_ = level;
  }

  ExceptionLogger::Level ExceptionProxy::getVerboseLevel() throw()
  {
    return verbose_level_;
  }

  void ExceptionProxy::setVerboseLevel(ExceptionLogger::Level level) throw()
  {
    verbose_level_ = level;
  }
}
