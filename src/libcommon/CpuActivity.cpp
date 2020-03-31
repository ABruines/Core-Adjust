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
 * @file src/libcommon/CpuActivity.cpp
 * @brief Measure CPU activity by interpreting /proc/stat.
 */
#include <fstream>
#include <sstream>
#include <string>

#include "CpuActivity.hpp"

namespace xxx {

  CpuActivity::CpuActivity(bool detail, bool account_for_guest)
    : detailed_cpu_activity_(detail), account_for_guest_(account_for_guest) {
    /* Scan the /proc/stat file for entries starting with 'cpu' or 'cpuN'
     * (where N is a positive value starting at 0) and create a place where
     * the statistics on those lines will be stored. */
    std::string line_buf;
    std::string label;
    std::ifstream ifs("/proc/stat");
    while (ifs.good()) {
      std::getline(ifs, line_buf);
      std::istringstream iss(line_buf);
      iss >> label;
      if (label.find("cpu") == 0) {
        cpu_stats_.emplace_back();
        emplace_back();
      }
    }
  }

  void CpuActivity::update()
  {
    size_t cpu_count = 0;

    uint64_t user_time, nice_time, system_time, idle_time,
        io_wait, irq, soft_irq, steal, guest, guest_nice,
        idle_all_time, system_all_time, guest_all_time, total_time;

    std::string line_buf;
    std::string label;
    std::ifstream ifs("/proc/stat");

    while (ifs.good()) {
      /* Read a line of text from /proc/stat and extract the 'label'. */
      std::getline(ifs, line_buf);
      std::istringstream iss(line_buf);
      iss >> label;

      /* We are only interested in lines that are labeled 'cpu' or 'cpuN'. */
      if (label.find("cpu") == 0) {

        /* Extract the (10) values from the line. */
        iss >> user_time >> nice_time >> system_time >> idle_time
            >> io_wait >> irq >> soft_irq >> steal >> guest >> guest_nice;

        /* Guest time is already accounted for in user_time/nice_time. */
        user_time = user_time - guest;
        nice_time = nice_time - guest_nice;

        /* Calculate totals. */
        idle_all_time = idle_time + io_wait;
        system_all_time = system_time + irq + soft_irq;
        guest_all_time = guest + guest_nice;
        total_time = user_time + nice_time + system_all_time + idle_all_time + steal + guest_all_time;

        /* Update the statistics and calculate the percentages for this cpu. */
        try {
          Statistics& stats = cpu_stats_.at(cpu_count);
          CpuActivityEntry& cpu_activity = at(cpu_count);
          ++cpu_count;

          stats.period.user = wrap_substract(user_time, stats.time.user);
          stats.period.nice = wrap_substract(nice_time, stats.time.nice);
          stats.period.system = wrap_substract(system_time, stats.time.system);
          stats.period.idle = wrap_substract(idle_time, stats.time.idle);
          stats.period.io_wait = wrap_substract(io_wait, stats.time.io_wait);
          stats.period.irq = wrap_substract(irq, stats.time.irq);
          stats.period.soft_irq = wrap_substract(soft_irq, stats.time.soft_irq);
          stats.period.steal = wrap_substract(steal, stats.time.steal);
          stats.period.guest = wrap_substract(guest_all_time, stats.time.guest);
          stats.period.system_all = wrap_substract(system_all_time, stats.time.system_all);
          stats.period.idle_all = wrap_substract(idle_all_time, stats.time.idle_all);
          stats.period.total = wrap_substract(total_time, stats.time.total);

          stats.time.user = user_time;
          stats.time.nice = nice_time;
          stats.time.system = system_time;
          stats.time.idle = idle_time;
          stats.time.io_wait = io_wait;
          stats.time.irq = irq;
          stats.time.soft_irq = soft_irq;
          stats.time.steal = steal;
          stats.time.guest = guest_all_time;
          stats.time.system_all = system_all_time;
          stats.time.idle_all = idle_all_time;
          stats.time.total = total_time;

          double period_total =
              (stats.period.total == 0) ? 1. : stats.period.total;

          unsigned int percent_total;

          cpu_activity.nice =
              static_cast<unsigned int>(stats.period.nice / period_total * 100.);
          cpu_activity.user =
              static_cast<unsigned int>(stats.period.user / period_total * 100.);

          if (detailed_cpu_activity_) {
            cpu_activity.kernel =
                static_cast<unsigned int>(stats.period.system / period_total * 100.);
            cpu_activity.irq =
                static_cast<unsigned int>(stats.period.irq / period_total * 100.);
            cpu_activity.soft_irq =
                static_cast<unsigned int>(stats.period.soft_irq / period_total * 100.);
            cpu_activity.steal =
                static_cast<unsigned int>(stats.period.steal / period_total * 100.);
            cpu_activity.guest =
                static_cast<unsigned int>(stats.period.guest / period_total * 100.);
            cpu_activity.io_wait =
                static_cast<unsigned int>(stats.period.io_wait / period_total * 100.);

            if (account_for_guest_) {
               percent_total =
                   cpu_activity.nice + cpu_activity.user +
                   cpu_activity.kernel + cpu_activity.irq +
                   cpu_activity.soft_irq + cpu_activity.steal +
                   cpu_activity.guest;
            } else {
               percent_total =
                   cpu_activity.nice + cpu_activity.user +
                   cpu_activity.kernel + cpu_activity.irq +
                   cpu_activity.soft_irq;
            }
          }
          else {
            cpu_activity.kernel =
                static_cast<unsigned int>(stats.period.system_all / period_total * 100.);
            cpu_activity.irq =
                static_cast<unsigned int>((stats.period.steal + stats.period.guest) / period_total * 100.);
            percent_total =
                cpu_activity.nice + cpu_activity.user +
                cpu_activity.kernel + cpu_activity.irq;
          }

          cpu_activity.total = clamp(percent_total, 0, 100);

        }
        catch(...) {
          /*
           * Catch out-of-bound exceptions,
           * a new hotplugged cpu will be picked up on the next update.
           */
          cpu_stats_.emplace_back();
          emplace_back();
          return;
        }
      }
    }
  }

} // ends namespace xxx

