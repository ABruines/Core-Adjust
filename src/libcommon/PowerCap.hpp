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
 * @file src/libcommon/PowerCap.hpp
 * @brief Client for reading Intel RAPL "Running Average Power Limit" (RAPL) technology.
 */

#ifndef libcommon_PowerCap_hpp
#define libcommon_PowerCap_hpp

#include <string>
#include <vector>
#include <chrono>

namespace xxx {

  /** @brief Power Capping Framework
    *
    * The kernel exposes power capping devices to user space via sysfs in the
    * form of a tree of objects. The objects at the root level of the tree represent
    * 'control types', which correspond to different methods of power capping.
    *
    * The classes in this namespace use the framework to access the
    * Intel RAPL ("Running Average Power Limit") technology control type.
    * @note Sysfs location: /sys/devices/virtual/powercap/intel-rapl
    * @see kernel sources: Documentation/power/powercap/powercap.txt */
  namespace PowerCap {

    /** @brief PowerZone attributes
      * @see kernel sources: Documentation/power/powercap/powercap.txt */
    class Attributes {
      public:
        Attributes();
        ~Attributes() = default;
        explicit Attributes(const std::string& path);

        inline const std::string& name() const { return name_; }
        inline double average_power() const { return power_; }

      protected:
        void update(uint64_t diff_us);

        bool have_name_;
        std::string name_;
        bool have_energy_uj_;
        std::string energy_uj_path_;
        bool have_max_energy_range_uj_;
        uint64_t max_energy_range_uj_;

        uint64_t energy_uj_;
        uint64_t prev_energy_uj_;
        double power_;
        double prev_power_;
    };

    /** @brief PowerZone constraints
      * @see kernel sources: Documentation/power/powercap/powercap.txt */
    class Constraints {
      public:
        Constraints();
        ~Constraints() = default;
        explicit Constraints(const std::string& path);

      protected:
        bool have_enabled_;
        std::string enabled_path_;
        bool have_constraint_0_;
        std::string constraint_0_power_limit_uw_path_;
        std::string constraint_0_time_window_us_path_;
        bool have_constraint_0_name_;
        std::string constraint_0_name_;
        bool have_constraint_0_min_power_uw_;
        uint64_t constraint_0_min_power_uw_;
        bool have_constraint_0_max_power_uw_;
        uint64_t constraint_0_max_power_uw_;
        bool have_constraint_0_min_time_window_us_;
        uint64_t constraint_0_min_time_window_us_;
        bool have_constraint_0_max_time_window_us_;
        uint64_t constraint_0_max_time_window_us_;
        bool have_constraint_1_;
        std::string constraint_1_power_limit_uw_path_;
        std::string constraint_1_time_window_us_path_;
        bool have_constraint_1_name_;
        std::string constraint_1_name_;
        bool have_constraint_1_min_power_uw_;
        uint64_t constraint_1_min_power_uw_;
        bool have_constraint_1_max_power_uw_;
        uint64_t constraint_1_max_power_uw_;
        bool have_constraint_1_min_time_window_us_;
        uint64_t constraint_1_min_time_window_us_;
        bool have_constraint_1_max_time_window_us_;
        uint64_t constraint_1_max_time_window_us_;
    };

    /** @brief Sub PowerZone
      * @see kernel sources: Documentation/power/powercap/powercap.txt */
    class SubZone : public Attributes, public Constraints {
      friend class PowerZone;
      public:
        SubZone();
        ~SubZone() = default;
        explicit SubZone(const std::string& path);
    };

    /** @brief A vector of SubZone. */
    class SubZones : public std::vector<SubZone> {
      public:
        using std::vector<SubZone>::vector;
        ~SubZones() = default;
    };

    /** @brief A single PowerZone and its (optional) sub-zones.
      * @see kernel sources: Documentation/power/powercap/powercap.txt */
    class PowerZone : public Attributes, public Constraints, public SubZones {
      friend class IntelRAPL;
      public:
        PowerZone();
        ~PowerZone() = default;
        explicit PowerZone(const char* powercap_driver, const std::string& path);
      protected:
        void update(uint64_t diff_us);
    };

    /** @brief A vector of PowerZone. */
    class PowerZones : public std::vector<PowerZone> {
      public:
        using std::vector<PowerZone>::vector;
        ~PowerZones() = default;
    };

    /** @brief Client for reading Intel RAPL ("Running Average Power Limit") technology powercap control-type
      * (/sys/devices/virtual/powercap/intel-rapl).
      *
      * A vector where each entry represents a PowerZone and its sub-zones.
      * @see kernel sources: Documentation/power/powercap/powercap.txt */
    class IntelRAPL : public PowerZones {
      public:
        IntelRAPL();
        ~IntelRAPL() = default;
        /** @brief Update the PowerZones.
          *
          * This function must be called regularly in order
          * to read the RAPL values from sysfs and calculate
          * the energy consumption over a timeperiod,
          * thus converting the number of Joules consumed
          * per time interval into powerusage in Watts.
          */
        void update();
      protected:
        /** @brief Timepoint used to calculate the time interval
          * between consequtive calls to IntelRAPL::update(). */
        std::chrono::high_resolution_clock::time_point tp_;
    };

  }

} // ends namespace xxx

#endif

