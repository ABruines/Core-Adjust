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
 * @file src/libcommon/PowerCap.cpp
 * @brief Client for reading Intel RAPL "Running Average Power Limit" (RAPL) technology (implementation).
 */
#include <fstream>

#include "Directory.hpp"
#include "PowerCap.hpp"

namespace xxx {

#define POWERCAP_READ(VAR) \
  s = path;                \
  s.append("/" #VAR);      \
  ifs.open(s);             \
  if (ifs.good()) {        \
    have_##VAR##_ = true;  \
    ifs >> VAR##_;         \
  }                        \
  ifs.close();

#define POWERCAP_READ_PATH(VAR) \
  s = path;                     \
  s.append("/" #VAR);           \
  ifs.open(s);                  \
  if (ifs.good()) {             \
    have_##VAR##_ = true;       \
    VAR##_path_ = s;            \
  }                             \
  ifs.close();

  /*
   * Attributes
   */

  PowerCap::Attributes::Attributes()
    : have_name_(false),
      have_energy_uj_(false),
      have_max_energy_range_uj_(false),
      max_energy_range_uj_(0),
      energy_uj_(0),
      prev_energy_uj_(0),
      power_(0.),
      prev_power_(0.) { }

  PowerCap::Attributes::Attributes(const std::string& path)
    : Attributes() {
    std::string s;
    std::ifstream ifs;
    POWERCAP_READ(name)
    POWERCAP_READ_PATH(energy_uj)
    POWERCAP_READ(max_energy_range_uj)
  }

  void PowerCap::Attributes::update(uint64_t diff_us) {
    if (have_energy_uj_) {
      std::ifstream ifs(energy_uj_path_);
      ifs >> energy_uj_;
      if (prev_energy_uj_ > 0 && energy_uj_ >= prev_energy_uj_) {
        // P = E / t = Watt
        power_ = static_cast<double>(energy_uj_ - prev_energy_uj_) / diff_us;
      }
      else {
        power_ = prev_power_;
      }
      prev_energy_uj_ = energy_uj_;
      prev_power_ = power_;
    }
  }

  /*
   * Constraints
   */

  PowerCap::Constraints::Constraints()
    : have_enabled_(false),
      have_constraint_0_(false),
      have_constraint_0_name_(false),
      have_constraint_0_min_power_uw_(false),
      constraint_0_min_power_uw_(0),
      have_constraint_0_max_power_uw_(false),
      constraint_0_max_power_uw_(0),
      have_constraint_0_min_time_window_us_(false),
      constraint_0_min_time_window_us_(0),
      have_constraint_0_max_time_window_us_(false),
      constraint_0_max_time_window_us_(0),
      have_constraint_1_(false),
      have_constraint_1_name_(false),
      have_constraint_1_min_power_uw_(false),
      constraint_1_min_power_uw_(0),
      have_constraint_1_max_power_uw_(false),
      constraint_1_max_power_uw_(0),
      have_constraint_1_min_time_window_us_(false),
      constraint_1_min_time_window_us_(0),
      have_constraint_1_max_time_window_us_(false),
      constraint_1_max_time_window_us_(0) { }

  PowerCap::Constraints::Constraints(const std::string& path)
    : Constraints() {
    std::string s;
    std::ifstream ifs;
    POWERCAP_READ_PATH(enabled)
    s = path;
    s.append("/constraint_0_power_limit_uw");
    ifs.open(s);
    if (ifs.good()) {
      have_constraint_0_ = true;
      constraint_0_power_limit_uw_path_ = s;
      s = path;
      s.append("/constraint_0_time_window_us");
      constraint_0_time_window_us_path_ = s;
    }
    ifs.close();
    if (have_constraint_0_) {
      POWERCAP_READ(constraint_0_name)
      POWERCAP_READ(constraint_0_min_power_uw)
      POWERCAP_READ(constraint_0_max_power_uw)
      POWERCAP_READ(constraint_0_min_time_window_us)
      POWERCAP_READ(constraint_0_max_time_window_us)
    }
    s = path;
    s.append("/constraint_1_power_limit_uw");
    ifs.open(s);
    if (ifs.good()) {
      have_constraint_1_ = true;
      constraint_1_power_limit_uw_path_ = s;
      s = path;
      s.append("/constraint_1_time_window_us");
      constraint_1_time_window_us_path_ = s;
    }
    ifs.close();
    if (have_constraint_1_) {
      POWERCAP_READ(constraint_1_name)
      POWERCAP_READ(constraint_1_min_power_uw)
      POWERCAP_READ(constraint_1_max_power_uw)
      POWERCAP_READ(constraint_1_min_time_window_us)
      POWERCAP_READ(constraint_1_max_time_window_us)
    }
  }

  /*
   * SubZone
   */

  PowerCap::SubZone::SubZone()
    : Attributes(),
      Constraints() { }

  PowerCap::SubZone::SubZone(const std::string& path)
    : Attributes(path),
      Constraints(path) { }

  /*
   * PowerZone
   */

  PowerCap::PowerZone::PowerZone()
    : Attributes(),
      Constraints() { }

  PowerCap::PowerZone::PowerZone(const char* powercap_driver, const std::string& path)
    : Attributes(path),
      Constraints(path) {
    for (auto& e : Directory::Read(path.c_str(), false)) {
      if (S_ISDIR(std::get<1>(e).st_mode)) {
        std::string fn(powercap_driver);
        fn.push_back(':');
        if (std::get<0>(e).find(fn) != std::string::npos) {
          fn = path;
          fn.push_back('/');
          fn.append(std::get<0>(e));
          emplace_back(fn);
        }
      }
    }
  }

  void PowerCap::PowerZone::update(uint64_t diff_us) {
    Attributes::update(diff_us);
    for (auto& sub_zone : *this) sub_zone.update(diff_us);
  }

  /*
   * IntelRAPL
   */

  PowerCap::IntelRAPL::IntelRAPL()
    : PowerZones(),
      tp_(std::chrono::high_resolution_clock::now()) {
    int isEnabled = 0;
    std::ifstream ifs("/sys/devices/virtual/powercap/intel-rapl/enabled");
    ifs >> isEnabled;
    ifs.close();
    if (isEnabled)
      for (auto& e : Directory::Read("/sys/devices/virtual/powercap/intel-rapl", true))
        if (S_ISDIR(std::get<1>(e).st_mode))
          if (std::get<0>(e).find("intel-rapl:") != std::string::npos)
            emplace_back("intel-rapl", std::get<0>(e));
    update();
  }

  void PowerCap::IntelRAPL::update() {
    using namespace std::chrono;
    auto tnow = high_resolution_clock::now();
    uint64_t diff_us = duration_cast<duration<uint64_t, std::micro>>(tnow - tp_).count();
    for (auto& power_zone : *this) power_zone.update(diff_us);
    tp_ = high_resolution_clock::now();
  }

} // ends namespace xxx

