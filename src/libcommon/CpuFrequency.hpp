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
 * @file src/libcommon/CpuFrequency.hpp
 * @brief Measure CPU frequency using sysfs.
 */
#ifndef libcommon_linux_sensors_CpuFrequency_hpp
#define libcommon_linux_sensors_CpuFrequency_hpp

#include <cstdint>
#include <vector>

namespace xxx {

  /** @brief CPU frequency readout using /sys/bus/cpu/devices/cpu?/cpufreq/scaling_cur_freq
    *
    * This class is a vector of integers with the current
    * frequencies (in MHz) for all logical cpus. */
  class CpuFrequency : public std::vector<unsigned long> {
    private:
      std::vector<size_t> logical_;
    public:
      CpuFrequency();
      /** @brief Update all frequencies in the vector. */
      void update();
      /** @brief get the logical cpu number for entry 'index' */
      size_t logical(size_t index) const;
  };

} /* ends namespace xxx */

#endif

