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

#ifndef ELEPHEYE_LIB_NO_THROW_STRING_HPP
#define ELEPHEYE_LIB_NO_THROW_STRING_HPP

#include <elepheye/Shared.hpp>
#include <string>

namespace elepheye
{
  namespace lib
  {
    /**
     * Class for a string which never throws exceptions.
     * @param Char - The type of the character.
     */
    template<class Char>
    class NoThrowString
    {
    public:

      /**
       * Constructor.
       */
      NoThrowString() throw() : is_lost_(false)
      {
      }

      /**
       * Set the string.
       * @param s - The string.
       */
      void set(const Char* s) throw()
      {
        is_lost_ = false;
        if (s)
        {
          try
          {
            s_ = Shared<const std::basic_string<Char> >(
              new std::basic_string<Char>(s));
          }
          catch (...)
          {
            clear();
            is_lost_ = true;
          }
        }
        else
        {
          clear();
        }
      }

      /**
       * Set the string.
       * @param s - The string.
       */
      void set(const std::basic_string<Char>& s) throw()
      {
        try
        {
          set(s.c_str());
        }
        catch (...)
        {
          clear();
          is_lost_ = true;
        }
      }

      /**
       * Get the string.
       * @param alt - The alternate string for the lost.
       * @return The string.
       */
      const Char* get(const Char* alt) const throw()
      {
        if (is_lost_)
        {
          return alt;
        }
        if (!s_)
        {
          return 0;
        }
        try
        {
          return s_->c_str();
        }
        catch (...)
        {
          return alt;
        }
      }

    private:

      void clear() throw()
      {
        s_ = Shared<const std::basic_string<Char> >();
      }

      Shared<const std::basic_string<Char> > s_;

      bool is_lost_;
    };
  }
}

#endif // #ifndef ELEPHEYE_LIB_NO_THROW_STRING_HPP
