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

#include <string>
#include <stdexcept>
#include "Msr.hpp"
#include "Shell.hpp"

uint64_t readMsr(LogicalCpuNr cpu, int address) {
  uint64_t output;
  auto rv = xxx::shell_command(
      { "rdmsr", "-X", "-0", "-p", std::to_string(cpu()),
          std::to_string(address) },
      [&output](auto, auto str){
    output = std::stoull(str, nullptr, 16);
    return 0;
  });
  if (rv) throw std::runtime_error("Failed to read MSR");
  return output;
}

int writeMsr(LogicalCpuNr cpu, int address, uint64_t value) {
  return xxx::shell_command({
      "wrmsr", "-p", std::to_string(cpu()),
      std::to_string(address), std::to_string(value)
  });
}

