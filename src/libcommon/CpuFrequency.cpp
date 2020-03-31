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
 * @file src/libcommon/CpuFrequency.cpp
 * @brief Measure CPU frequency using sysfs.
 */
#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>

#include "CpuFrequency.hpp"
#include "Directory.hpp"

xxx::CpuFrequency::CpuFrequency() {
  size_t count = 0;
  while (true) {
    std::stringstream ss;
    ss << "/sys/devices/system/cpu/cpu" << count;
    auto&& directory = Directory::Read(ss.str().c_str(), false);
    if (directory.size() == 0) break; // no more cpus
    // Test if the cpu is online
    std::stringstream ss1;
    ss1 << ss.str() << "/online";
    std::ifstream ifs(ss1.str());
    if (ifs.good()) {
      int online;
      ifs >> online;
      if (!online) {
        ++count;
        continue; // the cpu is offline
      }
    }
    // cpu is online, is there a 'cpufreq/scaling_cur_freq' file?
    for (auto& entry : directory) {
      if (S_ISDIR(entry.second.st_mode)) {
        if (entry.first.compare("cpufreq") == 0) {
          ss << "/cpufreq/scaling_cur_freq";
          std::ifstream ifs(ss.str());
          if (!ifs.good()) continue;
          uint64_t freq;
          ifs >> freq;
          // add the cpu to our vectors
          push_back(freq / 1000);
          logical_.push_back(count);
          break;
        }
      }
    }
    ++count;
  }
}

void xxx::CpuFrequency::update() {
  auto ilogical = logical_.begin();
  for (auto ifreq = begin(); ifreq != end(); ++ifreq, ++ilogical) {
    std::stringstream ss;
    ss << "/sys/devices/system/cpu/cpu" << *ilogical << "/cpufreq/scaling_cur_freq";
    std::ifstream ifs(ss.str());
    if (!ifs.good()) continue;
    uint64_t freq;
    ifs >> freq;
    *ifreq = freq / 1000;
  }
}

size_t xxx::CpuFrequency::logical(size_t index) const {
  if ((index) >= size()) return ULONG_MAX;
  return logical_[index];
}

