/*
 * This file is part of 'Core Adjust'.
 *
 * Core Adjust - Adjust various settings of Intel Processors.
 * Copyright (C) 2020, Alexander Bruines <alexander.bruines@gmail.com>
 *
 * Core Adjust is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Core Adjust is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Core Adjust. If not, see <https://www.gnu.org/licenses/>.
 */
/**
  * @file src/libcommon/Directory.hpp
  * @brief Class to generate and/or traverse directory listings.
  *
  * @file src/libcommon/Directory.cpp
  * @brief Class to generate and/or traverse directory listings (implementation).
  */

#ifndef libcommon_linux_Directory_hpp
#define libcommon_linux_Directory_hpp

#include <functional>
#include <string>
#include <vector>
#include <utility>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>

namespace xxx {

  using DirEntryPair = std::pair<std::string, struct stat>;
  using DirEntryFunc = std::function<int(const char* fn, const struct stat*)>;

  /** @brief Generate and/or traverse directory listings. */
  class Directory : public std::vector<DirEntryPair> {
    private:
      static constexpr const char* path_self = ".";
      static constexpr const char* path_updir = "..";
      static constexpr size_t pathlen_self = 1;
      static constexpr size_t pathlen_updir = 2;

    public:
      /* inherit constructors of the base class */
      using std::vector<DirEntryPair>::vector;

      /** @brief Get a Directory listing.
        *
        * Read a directory listing of a given path and return a
        * vector of DirEntryPair.
        * @param path The location of the directory.
        * @param fullPath true -> fn is full path, false -> fn is filename only
        * @note Directories . and .. are omitted
       */
      static Directory Read(const char* path, bool fullPath = false);

      /** @brief Traverse a Directory.
        *
        * Traverse the directory of a given path and call a function for
        * each directory entry.
        * @param path The location of the directory.
        * @param fullPath = true -> fn is full path, false -> fn is filename only
        * @note Returning non-zero from the callback function ends the traversal.
        * @note Directories . and .. are skipped.
        * @note If this function returns non-zero the (next) entry could not be
        * read with errno holding the reason of the failure.
        */
      static int Traverse(
          const char* path, const DirEntryFunc& func, bool fullPath = true);

      /** @brief Locate an entry in this Directory.
        *
        * Find a file-entry in the current directory (ie. no recursion).
        * Returns the index of the file or -1.
        * @param fn The name of the file-entry. */
      long find(const char* fn);
  };

} // ends namespace xxx

#endif

