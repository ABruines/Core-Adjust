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
 * @file src/core-adjust-qt/main.cpp
 * @brief Core Adjust GUI application entry point.
 */
// STL
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <filesystem>
// Qt
#include <QApplication>
#include <QStyleFactory>
#include <QDebug>
#include <QMessageBox>
#include <QRect>
#include <QScreen>
#include <QSize>
#include <QStyle>
#include <QCommandLineParser>
// App
#include "Dbg.hpp"
#include "MainWindow.hpp"
#include "Shell.hpp"

/*
 * Adding a new tab to the application:
 *
 * - Create a new class 'foo' that inherits and implements class TabMemberWidget
 *   (if possible using class TabMemberTemplate as a base).
 *
 * - If the new tab stores something in the Common section of the INI file then
 *   create a class 'foo::CommonSettings' and let it be inherited by class CommonSettings.
 *   Also create a class 'foo::CommonValues' and let it be inherited by class CommonValues.
 *
 * - If the new tab stores per-cpu settings in the INI file then:
 *    - Create a new class 'foo::Values' with the data
 *      and let it be inherited by class TabMemberValues.
 *    - Create a new class 'foo::Settings' that inherits class 'foo::Values' and adds
 *      configuration data that is not directly read from the cpu/system/...
 *      and let it be inherited by class TabMemberSettings.
 *
 * - Use the other TabMemberWidget implementations as a guide to implement
 *   class 'foo' and its sub-classes.
 *
 * - Add class 'foo' to CoreAdjust::TabMemberFactory or CoreAdjust::TabMemberAllCpusFactory.
 */

int main(int argc, char** argv) {
  /* Set environment variable KDE_FULL_SESSION to 'true' if it is not defined.
   * This allows us to use the KDE theme for root (when launched by pkexec).
   * It should have no adverse effect on other desktop environments. */
  setenv("KDE_FULL_SESSION", "true", 0);


  /* Create a Qt application instance */
  QApplication::setApplicationName(PACKAGE_NAME);
  QApplication::setApplicationVersion(PACKAGE_VERSION);
  QApplication app(argc, argv);

  /* Test if we are root */
  if (getuid() != 0) {
    QMessageBox::critical(nullptr, "Core Adjust",
        "Error, this application must be run with root privileges!");
    return -1;
  }

  /* Do not allow the application to execute as SUID/SGID
   * Note: QApplication also tests for SUID */
  try { xxx::shell_safety_test("/proc/self/exe"); }
  catch (const std::runtime_error& e) {
    QMessageBox::critical(nullptr, "Core Adjust", std::move(QString(
        "<p>%1</p><p>The application will now exit!</p>").arg(e.what())));
    return -1;
  }

  /* Read CPUID info for all processors. */
  DBGMSG("main(): Creating global CpuId instance.")
  CpuId cpuId(false);

  /* Get type/model information for all processors. */
  DBGMSG("main(): Creating global CpuInfo instance.")
  CpuInfo cpuInfo(false);

  /* Init the current values for all tabs. */
  DBGMSG("main(): Creating global TabValues instance.")
  TabValues cpuValues(cpuInfo);

  /* Load the settings INI file from disk. */
  DBGMSG("main(): Creating global TabSettings instance.")
  TabSettings settings(cpuInfo, cpuValues);

  /* Create an instance of the application main window and
   * determine what size the window wants to be. */
  DBGMSG("main(): Creating MainWindow instance.")
  MainWindow window(cpuId, cpuInfo, cpuValues, settings);
  auto&& size = window.sizeHint();

  /* (At startup) the screen holding the mouse pointer will most
   * likely also be the screen where our window is displayed.
   * Get the available width and height of that screen. */
  auto&& screen = QApplication::screenAt(QCursor::pos())->availableGeometry();

  /* The -100 here seems to prevent a strange quirk of Qt. If the window is
   * set to the screen width and/or height some strange thing happens when
   * 'restoring' the window after a 'maximize' event. The maximize will work
   * fine but when un-maximizing the window it does not return to its
   * previous geometry, but to a smaller one. I Guess that it has something to
   * do with Qt's rule not to let a window cover more then 2/3 of the desktop
   * by default!? Anyway, the -100 pixels seems to prevent that and
   * restores the window to the (correct) previous geometry... */
  screen.setWidth(screen.width() - 100);
  screen.setHeight(screen.height() - 100);

  /* Resize the window to the size it wants to be,
   * or to the available screen size if that is smaller. */
  if (size.width() > screen.width() || size.height() > screen.height()) {
    window.resize(
        (size.width() > screen.width()) ? screen.width() : size.width(),
        (size.height() > screen.height()) ? screen.height() : size.height());
  }
  else {
    window.resize(size);
  }

  /* Center the window on the screen */
  window.setGeometry(QStyle::alignedRect(
      Qt::LeftToRight,
      Qt::AlignCenter,
      window.size(),
      screen));

  /* Set the titlebar text and show the window. */
  window.setWindowTitle("Core Adjust");
  window.show();

  /* Wait untill the application has finished, then exit. */
  DBGMSG("main(): Ready, waiting for QApplication instance to finish.")
  return app.exec();
}

