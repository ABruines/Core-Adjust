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
 * @file src/libcommon/Strings.hpp
 * @brief Various string related functions.
 */

#ifndef libcommon_Strings_hpp
#define libcommon_Strings_hpp

#include <algorithm> 
#include <cctype>
#include <functional>
#include <regex>
#include <string>
#include <vector>

/** @brief This namespace contains data that is 'private' to libcommon */
namespace libcommon {
  /** @brief A regular expression that matches any ANSI escape sequence. */
  extern std::regex ansi_regex;
  /** @brief A string (empty) that strip_ansi() uses to replace ANSI codes with. */
  extern std::string ansi_replacement;
}

namespace xxx {

  /** @brief Strip ANSI escape sequences from a string
    * (and replaces them with the text in libcommon::ansi_replacement).*/
  std::string strip_ansi(const char*);

  /** @brief Strip ANSI escape sequences from a string
    * (and replaces them with the text in libcommon::ansi_replacement).*/
  std::string strip_ansi(const std::string&);

  /** @brief Trim white-space chracters on the left using the specified locale (in place).
    * @param s   The string to trim.
    * @returns   The trimmed string.
    * @note
    *   When using the 'C' locale white-space means the characters:
    *   0x20 (SPC), 0x09 (TAB), 0x0A (LF), 0x0B (VT), 0x0C (FF) and 0x0D (CR).
    *
    *   Other locales may consider a different selection of characters as
    *   white-spaces, but never a character that returns true for std::isalnum. */
  inline std::string& ltrim(std::string& s) {
    s.erase(
      s.begin(),
      std::find_if(s.begin(), s.end(), [](int ch) {
          return !std::isspace(ch); })
    );
    return s;
  }

  /** @brief Trim white-space chracters on the right using the specified locale (in place).
    * @param s   The string to trim.
    * @returns   The trimmed string.
    * @note
    *   When using the 'C' locale white-space means the characters:
    *   0x20 (SPC), 0x09 (TAB), 0x0A (LF), 0x0B (VT), 0x0C (FF) and 0x0D (CR).
    *
    *   Other locales may consider a different selection of characters as
    *   white-spaces, but never a character that returns true for std::isalnum. */
  inline std::string& rtrim(std::string& s) {
    s.erase(
      std::find_if(s.rbegin(), s.rend(), [](int ch) {
          return !std::isspace(ch); }).base(), /* (base() returns an iterator in the oposite direction) */
      s.end()
    );
    return s;
  }

  /** @brief Trim white-space chracters using the specified locale (in place).
    * @param s   The string to trim.
    * @returns   The trimmed string.
    * @note
    *   When using the 'C' locale white-space means the characters:
    *   0x20 (SPC), 0x09 (TAB), 0x0A (LF), 0x0B (VT), 0x0C (FF) and 0x0D (CR).
    *
    *   Other locales may consider a different selection of characters as
    *   white-spaces, but never a character that returns true for std::isalnum. */
  inline std::string& trim(std::string& s) {
    ltrim(s);
    rtrim(s);
    return s;
  }

  /** @brief Trim white-space chracters on the left using the specified locale (copy).
    * @param s   The string to trim.
    * @param loc The locale to use, default is 'C'.
    * @returns   A copy of the trimmed string.
    * @note
    *   When using the 'C' locale white-space means the characters:
    *   0x20 (SPC), 0x09 (TAB), 0x0A (LF), 0x0B (VT), 0x0C (FF) and 0x0D (CR).
    *
    *   Other locales may consider a different selection of characters as
    *   white-spaces, but never a character that returns true for std::isalnum. */
  inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
  }

  /** @brief Trim white-space chracters on the right using the specified locale (copy).
    * @param s   The string to trim.
    * @param loc The locale to use, default is 'C'.
    * @returns   A copy of the trimmed string.
    * @note
    *   When using the 'C' locale white-space means the characters:
    *   0x20 (SPC), 0x09 (TAB), 0x0A (LF), 0x0B (VT), 0x0C (FF) and 0x0D (CR).
    *
    *   Other locales may consider a different selection of characters as
    *   white-spaces, but never a character that returns true for std::isalnum. */
  inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
  }

  /** @brief Trim white-space chracters using the specified locale (copy).
    * @param s   The string to trim.
    * @returns   A copy of the trimmed string.
    * @note
    *   When using the 'C' locale white-space means the characters:
    *   0x20 (SPC), 0x09 (TAB), 0x0A (LF), 0x0B (VT), 0x0C (FF) and 0x0D (CR).
    *
    *   Other locales may consider a different selection of characters as
    *   white-spaces, but never a character that returns true for std::isalnum. */
  inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
  }

  /** @brief Thread safe strtok()
    * @param str The C string to tokenize
    * @param delim The delimiters for the tokens
    * @param nextp Internal string 'iterator'
    * @return A pointer to the next token or NULL if there are no more tokens. */
  char* strtok_r(char *str, const char *delim, char **nextp);

  /** @brief Tokenize a string and place the tokens in a vector.
    * @param str The string to tokenize.
    * @param delim The delimiters for the tokens
    * @return A vector of strings with the tokens. */
  std::vector<std::string> tokenize(
      const std::string& str, const std::string& delim);

  /** @brief Tokenize a string and iterate over the tokens by calling a function.
    * @param str The string to tokenize.
    * @param delim The delimiters for the tokens
    * @param callback The function to call for each token. */
  void tokenize(const std::string& str, const std::string& delim,
      const std::function<int(std::string)>& callback);

} // ends namespace xxx

#endif // ends header guard

