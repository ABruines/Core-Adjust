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
  * @file src/core-adjust-qt/Dbg.hpp
  * @brief Macro to write debug messages to the console.
  */

#ifndef CoreAdjust_Dbg
#define CoreAdjust_Dbg

#include "config.h"

#ifdef DEBUG
#include <QDebug>
#define DBGMSG(msg) qDebug() << msg;
#else
#define DBGMSG(msg)
#endif

#endif

