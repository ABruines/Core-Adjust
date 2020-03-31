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
  * @file src/core-adjust-qt/MainWindow.hpp
  * @brief QMainWindow for the Core Adjust application.
  *
  * @file src/core-adjust-qt/MainWindow.cpp
  * @brief QMainWindow for the Core Adjust application (implementation).
  */
#ifndef CoreAdjust_MainWindow
#define CoreAdjust_MainWindow

#include <QMainWindow>
#include "CpuId.hpp"
#include "CpuInfo.hpp"
#include "TabMember.hpp"

/** @brief QMainWindow for the Core Adjust application. */
class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    explicit MainWindow(
        CpuId&,
        CpuInfo&,
        TabValues&,
        TabSettings&,
        QWidget* parent = nullptr
    );
    virtual ~MainWindow() = default;
  private slots:
    //void menuQuit();
    void menuAbout();
    void menuAboutQt();
};

#endif

