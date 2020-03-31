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
 * @file src/libcommon/CpuActivity.hpp
 * @brief Measure CPU activity by interpreting /proc/stat.
 */

#ifndef libcommon_linux_sensors_CpuActivity_hpp
#define libcommon_linux_sensors_CpuActivity_hpp

#include <cstdint>
#include <vector>

namespace xxx {

  /** @brief Processor activity statistics for a single logical cpu. */
  struct CpuActivityEntry {
     unsigned int
       nice, user, kernel, irq, soft_irq, steal, guest, io_wait,
       total;
  };

  /** @brief Measure processor activity by interpreting /proc/stat.
    *
    * This class is a vector of CpuActivityEntry.
    * Index 0 of the vector is the aggregate of all processors,
    * followed by an entry for each logical cpu. */
  class CpuActivity : public std::vector<CpuActivityEntry> {
    public:

      /** @brief Processor activity statistics (for internal usage by class CpuActivity). */
      struct Statistics {
        struct {
          uint64_t
              user, nice, system, idle, io_wait, irq, soft_irq, steal, guest,
              system_all, idle_all, total;
        } time;
        struct {
          uint64_t
              user, nice, system, idle, io_wait, irq, soft_irq, steal, guest,
              system_all, idle_all, total;
        } period;
      };

      /** @param detail
        *   false == only the 'total' field in the output vector are valid.
        *   true  == all fields in the output vector are valid.
        * @param account_for_guest
        *   (Only affects detail=true.) 
        *   Account for the 'guest' entry in /proc/stat when true. */
      CpuActivity(bool detail = true, bool account_for_guest = true);

      /* Refreshes the activity counters. */
      void update();

    protected:

      /** @brief Statistics for each logical cpu. */
      std::vector<Statistics> cpu_stats_;

      bool detailed_cpu_activity_;
      bool account_for_guest_;

      /** @brief Subtract two unsigned integers,
        * limiting the result to a positive integer value. */
      inline uint64_t wrap_substract(uint64_t a, uint64_t b);

      /** @brief Clamp a value in-between a minimum and maximum value. */
      inline unsigned int clamp(
          unsigned int value, unsigned int min, unsigned int max);
  };

  inline uint64_t CpuActivity::wrap_substract(uint64_t a, uint64_t b) {
    return (a > b) ? a - b : 0;
  }

  inline unsigned int CpuActivity::clamp(
      unsigned int value, unsigned int min, unsigned int max) {
    return (value > max) ? max : ((value < min) ? min : value);
  }

} /* ends namespace xxx */

#endif

