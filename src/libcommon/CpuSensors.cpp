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
 * @file src/libcommon/CpuSensors.cpp
 * @brief Measure several CPU statisics.
 */

#include <thread>
#include "CpuSensors.hpp"

xxx::CpuSensors::CpuSensors()
  : cpu_active_(),
    cpu_freq_(),
    cpu_temp_(),
    cpu_power_() {
}


void xxx::CpuSensors::update() {
  std::thread
      th1(&CpuActivity::update, &cpu_active_),
      th2(&CpuFrequency::update, &cpu_freq_),
      th3(&CpuTemperature::update, &cpu_temp_),
      th4(&PowerCap::IntelRAPL::update, &cpu_power_);
  th1.join();
  th2.join();
  th3.join();
  th4.join();
}

