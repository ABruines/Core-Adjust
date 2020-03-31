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
 * @file src/libcommon/Strings.cpp
 * @brief Various string related functions.
 */
#include <cstring>
#include "Strings.hpp"

namespace libcommon {
  /* A regular expression that matches any ANSI escape sequence. */
  std::regex ansi_regex("\x1B\[[0-?]*[ -/]*[@-~]");

  /* A string (empty) that strip_ansi() uses to replace ANSI codes with. */
  std::string ansi_replacement;
}

using namespace libcommon;

/* Strip ANSI escape sequences from a string (const char* version) */
std::string xxx::strip_ansi(const char* in) {
  std::string&& out = std::regex_replace(in, ansi_regex, ansi_replacement);
  return std::move(out);
}

/* Strip ANSI escape sequences from a string (const std::string& version) */
std::string xxx::strip_ansi(const std::string& in) {
  std::string&& out = std::regex_replace(in, ansi_regex, ansi_replacement);
  return std::move(out);
}

char* xxx::strtok_r(char *str, const char *delim, char **nextp) {
  char *ret;
  if (str == nullptr) str = *nextp;
  str += std::strspn(str, delim);
  if (*str == '\0') return nullptr;
  ret = str;
  str += std::strcspn(str, delim);
  if (*str) *str++ = '\0';
  *nextp = str;
  return ret;
}


std::vector<std::string> xxx::tokenize(
    const std::string& str, const std::string& delim) {
  std::vector<std::string> out;
  char *endptr = nullptr;
  std::string copy(str);
  copy += '\0';
  char* token = xxx::strtok_r(&*copy.begin(), delim.c_str(), &endptr);
  while (token) {
    out.emplace_back(token);
    token = xxx::strtok_r(nullptr, delim.c_str(), &endptr);
  }
  return out;
}


void xxx::tokenize(
    const std::string& str, const std::string& delim,
    const std::function<int(std::string)>& callback) {
  char* endptr = nullptr;
  std::string copy(str);
  copy.push_back('\0');
  char* token = xxx::strtok_r(&*copy.begin(), delim.c_str(), &endptr);
  while (token) {
    if (callback(std::string(token))) break;
    token = xxx::strtok_r(nullptr, delim.c_str(), &endptr);
  }
}

