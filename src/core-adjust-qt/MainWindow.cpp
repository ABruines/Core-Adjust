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

#include <QApplication>
#include <QEvent>
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include "CoreAdjust.hpp"
#include "MainWindow.hpp"

MainWindow::MainWindow(
  CpuId& id,
  CpuInfo& info,
  TabValues& values,
  TabSettings& settings,
QWidget* parent) : QMainWindow(parent)
{
  /* Create a menubar */

  //QMenu* file = menuBar()->addMenu("&File");
  //auto* quitAction = new QAction("&Quit", this);
  //file->addAction(quitAction);
  //connect(quitAction, SIGNAL(triggered()), this, SLOT(menuQuit()));

  auto* bar = new QMenuBar(this);
  auto* helpMenu = new QMenu("&Help", bar);
  auto* aboutAction = new QAction("&About", bar);
  auto* aboutQtAction = new QAction("About &Qt", bar);
  helpMenu->addAction(aboutAction);
  helpMenu->addAction(aboutQtAction);
  bar->addMenu(helpMenu);
  menuBar()->setCornerWidget(bar);
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(menuAbout()));
  connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(menuAboutQt()));

  /* Create and set a central widget for this QMainWindow */
  auto* widget = new CoreAdjust(id, info, values, settings, this);
  setCentralWidget(widget);

  /* Install the CentralWidget widget as eventFilter so that it may
   * intercept events like QEvent::Close */
  installEventFilter(widget);
}

//void MainWindow::menuQuit() {
//  /* Request the application to close */
//  QEvent ev(QEvent::Close);
//  QApplication::sendEvent(qApp, &ev);
//}

void MainWindow::menuAbout() {
  QMessageBox::information(this,
      tr("About"),
      tr("<p>Core Adjust</p><p>A tool for adjusting various settings of Intel processors based on the 'Haswell' or newer microarchitecture.</p>"
         "<p><nobr>Copyright (C) 2020, <a href='mailto:alexander.bruines@gmail.com'>Alexander Bruines</a></nobr></p>"
         "<p>Licenced under the GNU General Public Licence, version 3 or later.</p>")
  );
}

void MainWindow::menuAboutQt() {
  QMessageBox::aboutQt(this);
}

