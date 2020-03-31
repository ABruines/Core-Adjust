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
  * @file src/libcommon/Strong.hpp
  * @brief Template (class) to create a strongly typed variant of a C++ built-in data type.
  *
  * @class Strong
  * @brief Template (class) to create a strongly typed variant of a C++ built-in data type.
  */
#ifndef libcommon_Strong_hpp
#define libcommon_Strong_hpp

template<typename T, typename Tag>
struct Strong {
  T value;
  Strong() = default;
  explicit Strong(T v) : value(v) {}
  inline T operator() () const { return value; }
  inline bool operator== (const Strong<T,Tag>& rhs) const { return value == rhs.value; }
  inline bool operator!= (const Strong<T,Tag>& rhs) const { return value != rhs.value; }
  inline bool operator< (const Strong<T,Tag>& rhs) const { return value < rhs.value; }
  inline bool operator> (const Strong<T,Tag>& rhs) const { return value > rhs.value; }
  inline bool operator<= (const Strong<T,Tag>& rhs) const { return value <= rhs.value; }
  inline bool operator>= (const Strong<T,Tag>& rhs) const { return value >= rhs.value; }
  inline Strong& operator++() { ++value; return *this; } /* prefix */
  inline Strong operator++(int) { Strong tmp(*this); operator++(); return tmp; } /* postfix */
  inline Strong& operator--() { --value; return *this; } /* prefix */
  inline Strong operator--(int) { Strong tmp(*this); operator--(); return tmp; } /* postfix */
};

#endif

