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

#ifndef ELEPHEYE_SOURCE_REGISTRY_PATH_HPP
#define ELEPHEYE_SOURCE_REGISTRY_PATH_HPP

#include <elepheye/String.hpp>

namespace elepheye
{
  namespace source
  {
    /**
     * Class for the registry path.
     * The key name and value name are separated with `\\'.
     */
    class RegistryPath
    {
    public:

      /**
       * Constructor.
       * @param path - The path.
       */
      explicit RegistryPath(const String& path);

      /**
       * Constructor.
       * @param parent - The parent path.
       * @param child - The child name to append.
       * @param is_key - True if the child is a sub key.
       */
      RegistryPath(
        const RegistryPath& parent, const String& child, bool is_key);

      /**
       * Get the path.
       * @return The path.
       */
      const String& get() const;

      /**
       * Get the machine name.
       * @return The name.
       */
      const String& getMachineName() const;

      /**
       * Get the key name.
       * @return The name.
       */
      const String& getKeyName() const;

      /**
       * Get the sub key name.
       * @return The name.
       */
      const String& getSubKeyName() const;

      /**
       * Get the value name.
       * @return The name.
       */
      const String& getValueName() const;

      /**
       * Get the key handle.
       * @return The handle.
       */
      ::HKEY getKey() const;

      /**
       * Get whether the path is a key or value.
       * @return True if key.
       */
      bool isKey() const;

    private:
      void split(const String& path);
      void join();
      void setKey();
      String path_;
      String machine_name_;
      String key_name_;
      String sub_key_name_;
      String value_name_;
      ::HKEY key_;
      bool is_key_;
    };
  }
}

#endif // #ifndef ELEPHEYE_SOURCE_REGISTRY_PATH_HPP
