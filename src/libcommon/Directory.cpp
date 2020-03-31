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

#include "Directory.hpp"

namespace xxx {

  Directory Directory::Read(const char *path, bool fullPath) {
    Directory v;
    if (DIR* dir = opendir(path)) {
      while (struct dirent* d = readdir(dir)) {
        struct stat st{};
        std::string p(path);
        p.push_back('/');
        p.append(&d->d_name[0]);
        if (!stat(p.c_str(), &st)) {
          if (S_ISDIR(st.st_mode)) {
            if (std::char_traits<char>::compare(
                &d->d_name[0], path_self, pathlen_self + 1) == 0) continue;
            if (std::char_traits<char>::compare(
                &d->d_name[0], path_updir, pathlen_updir + 1) == 0) continue;
          }
          if (fullPath) {
            v.emplace_back(std::move(p), std::move(st));
          }
          else {
            v.emplace_back(d->d_name, std::move(st));
          }
        }
      }
      closedir(dir);
    }
    return v;
  }

  int Directory::Traverse(const char *path, const DirEntryFunc &func, bool fullPath) {
    if (DIR* dir = opendir(path)) {
      while (struct dirent* d = readdir(dir)) {
        struct stat st{};
        std::string p(path);
        p.push_back('/');
        p.append(&d->d_name[0]);
        if (!stat(p.c_str(), &st)) {
          if (S_ISDIR(st.st_mode)) {
            if (std::char_traits<char>::compare(
                &d->d_name[0], path_self, pathlen_self + 1) == 0) continue;
            if (std::char_traits<char>::compare(
                &d->d_name[0], path_updir, pathlen_updir + 1) == 0) continue;
          }
          if (func(fullPath ? p.c_str() : &d->d_name[0], &st)) break;
        }
        else return -1;
      }
      closedir(dir);
    }
    return 0;
  }

  long Directory::find(const char *fn) {
    for (auto i = begin(); i != end(); ++i) {
      if (std::get<0>(*i) == fn) {
        return std::distance(begin(), i);
      }
    }
    return -1;
  }

}

