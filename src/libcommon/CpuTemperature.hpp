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
 * @file src/libcommon/CpuTemperature.hpp
 * @brief Measure CPU temperature using sysfs.
 */

#ifndef libcommon_linux_sensors_CpuTemperature_hpp
#define libcommon_linux_sensors_CpuTemperature_hpp

#include <string>
#include <vector>

namespace xxx {

  /** @brief A single 'coretemp input' from /sys/class/hwmon. */
  struct CpuTemperatureEntry {
    friend class CpuTemperature;
    public:
      std::string label;  /* label of the input */
      int max;            /* maximum temperature in °C */
      int crit;           /* critical temperature in °C */
      int value;          /* last updated value from the input */
    private:
      std::string path;   /* sysfs path of the hwmon coretemp input */
  };

  /** @brief Measure processor temperature(s) by interpreting the 'coretemp inputs' listed in /sys/class/hwmon. */
  class CpuTemperature : public std::vector<CpuTemperatureEntry> {
    public:
      CpuTemperature();
      void update();
  };

} // ends namespace xxx

#endif

