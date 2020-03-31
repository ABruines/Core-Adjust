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
  * @file src/core-adjust-qt/CpuNumber.hpp
  * @brief Integer types for various processor numbers.
  */
#ifndef CoreAdjust_CpuNumber_hpp
#define CoreAdjust_CpuNumber_hpp

#include "Strong.hpp"

/* Make sure we cannot mistake a logical for a physical cpu number and
 * visa versa by declaring both as seperate strongly-typed integers. */

/** @brief A strongly typed unsigned integer used as 'logical processor number'. */
typedef Strong<unsigned long, struct LogicalCpuNrTag> LogicalCpuNr;

/** @brief A strongly typed unsigned integer used as 'physical processor number'. */
typedef Strong<unsigned long, struct PhysCpuNrTag> PhysCpuNr;

/* Make sure we cannot mistake a core for a sibling and
 * visa versa by declaring both as seperate strongly-typed integers. */

/** @brief A strongly typed unsigned integer used as 'core number'. */
typedef Strong<unsigned long, struct CpuCoreNrTag> CpuCoreNr;

/** @brief A strongly typed unsigned integer used as 'sibling number'. */
typedef Strong<unsigned long, struct CpuSiblingNrTag> CpuSiblingNr;

#endif

