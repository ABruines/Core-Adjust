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
 * @file src/libcommon/CpuSensors.hpp
 * @brief Measure several CPU statisics.
 */

#ifndef libcommon_linux_sensors_CpuSensors_hpp
#define libcommon_linux_sensors_CpuSensors_hpp

#include "CpuActivity.hpp"
#include "CpuFrequency.hpp"
#include "CpuTemperature.hpp"
#include "PowerCap.hpp"

namespace xxx {

  /** @brief Collection of CPU sensors. */
  class CpuSensors {
    public:

      CpuSensors();
      ~CpuSensors() = default;

      /* Refreshes the values for all sensors (in parallel). */
      void update();

      inline const CpuActivity& cpu_activity();
      inline const CpuFrequency& cpu_frequency();
      inline const CpuTemperature& cpu_temperature();
      inline const PowerCap::IntelRAPL& cpu_power();

    protected:
      CpuActivity cpu_active_;
      CpuFrequency cpu_freq_;
      CpuTemperature cpu_temp_;
      PowerCap::IntelRAPL cpu_power_;
  };

  const CpuActivity& CpuSensors::cpu_activity() { return cpu_active_; }
  const CpuFrequency& CpuSensors::cpu_frequency() { return cpu_freq_; }
  const CpuTemperature& CpuSensors::cpu_temperature() { return cpu_temp_; }
  const PowerCap::IntelRAPL& CpuSensors::cpu_power() { return cpu_power_; }

} // ends namespace xxx

#endif

