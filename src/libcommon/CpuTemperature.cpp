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
 * @file src/libcommon/CpuTemperature.cpp
 * @brief Measure CPU temperature using sysfs (implementation).
 */
#include <fstream>
#include <sstream>

#include "Directory.hpp"
#include "CpuTemperature.hpp"

namespace xxx {

  CpuTemperature::CpuTemperature() {
    static constexpr const char* str_coretemp_ = "coretemp";
    std::string sysfs_path;
    /* Locate the directory in sysfs that contains the 'coretemp' hwmon */
    Directory::Traverse("/sys/class/hwmon", [&](auto path, auto st) {
      if (S_ISDIR(st->st_mode)) {
        auto&& dir = Directory::Read(path);
        long idx = dir.find("name");
        if (idx >= 0) {
          std::string fn(path);
          fn.push_back('/');
          fn.append(std::get<0>(dir[static_cast<size_t>(idx)]));
          std::ifstream ifs(fn);
          if (ifs.good()) {
            std::string buf;
            std::getline(ifs, buf);
            if (buf == str_coretemp_) {
              /* Found it! stop scanning */
              sysfs_path = path;
              return 1;
            }
          }
        }
      }
      return 0;
    });
    /* Create a vector of available inputs. */
    if (!sysfs_path.empty()) {
      while (true) {
        std::stringstream ss;
        ss << sysfs_path << "/temp" << size() + 1;
        /* temp?_input */
        std::string fn(ss.str());
        fn.append("_input");
        std::ifstream ifs(fn);
        if (!ifs.good()) break;
        CpuTemperatureEntry i;
        i.path = std::move(fn);
        ifs.close();
        /* temp?_label */
        fn = ss.str();
        fn.append("_label");
        ifs.open(fn);
        if (ifs.good()) std::getline(ifs, i.label);
        else i.label = "Unknown";
        ifs.close();
        /* temp?_max */
        fn = ss.str();
        fn.append("_max");
        ifs.open(fn);
        if (ifs.good()) {
          ifs >> i.max;
          i.max /= 1000;
        }
        else i.max = -1;
        ifs.close();
        /* temp?_crit */
        fn = ss.str();
        fn.append("_crit");
        ifs.open(fn);
        if (ifs.good()) {
          ifs >> i.crit;
          i.crit /= 1000;
        }
        else i.crit = -1;
        ifs.close();
        /* add the input to our vector */
        push_back(std::move(i));
      }
    }
    /* update the current values */
    update();
  }

  void CpuTemperature::update() {
    /* update the value of each input */
    for (auto& input : *this) {
      std::ifstream ifs(input.path);
      if (ifs.good()) {
        ifs >> input.value;
        input.value /= 1000;
      }
      else {
        input.value = -1;
      }
    }
  }

} // ends namespace xxx

