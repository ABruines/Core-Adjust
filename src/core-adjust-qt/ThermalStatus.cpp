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
  * @file src/core-adjust-qt/ThermalStatus.hpp
  * @brief A TabMemberWidget for displaying CPU thermal status information.
  *
  * @file src/core-adjust-qt/ThermalStatus.cpp
  * @brief A TabMemberWidget for displaying CPU thermal status information (implementation).
  *
  * @class ThermalStatus
  * @brief A TabMemberWidget for displaying CPU thermal status information.
  *
  * @class ThermalStatusCore
  * @brief Helper class for class ThermalStatus.
  *
  * @class ThermalStatusPackage
  * @brief Helper class for class ThermalStatus.
  */

#include <csignal>
#include <QDebug>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "TabMember.hpp"
#include "Strings.hpp"
#include "Msr.hpp"
#include "ThermalStatus.hpp"

/*
 * ThermalStatus
 */

ThermalStatus::ThermalStatus(
  const SingleCpuInfo& c, const SingleCpuId& i,
  TabMemberValues& v, TabMemberSettings& s, QWidget *p)
  : TabMemberTemplate(c, i, v, s, p, false) {

  tabs_ = new QTabWidget();

  pkg_supported_ = cpuId().EAX_06H.EAX & (1 << 6); // PTM flag
  package_ = new ThermalStatusPackage(cpuInfo(), cpuId(), tabValues());
  package_->setEnabled(pkg_supported_);
  tabs_->addTab(package_, "Package");

  core_supported_ = cpuId().EAX_01H.EDX & (1 << 22); // ACPI flag
  for (CpuCoreNr i(0); i.value < cpuInfo().cores(); ++i) {
    core_.push_back(new ThermalStatusCore(cpuInfo(), cpuId(), tabValues(), i));
    core_.back()->setEnabled(core_supported_);
    tabs_->addTab(core_.back(), std::move(QString("Core %1").arg(i.value)));
  }

  /* Add the inner widgets to the layout of the scroll widget */

  auto* layout = new QVBoxLayout(this);
  layout->addSpacing(10);
  layout->addWidget(tabs_);

  /* Signals */

  connect(tabs_, SIGNAL(currentChanged(int)), this, SLOT(tabSwitchSlot(int)));
}

void ThermalStatus::timed(bool is_current_tab) {
  if (is_current_tab) {
    if (pkg_supported_) package_->timed(tabIndex_ == 0);
    if (core_supported_) {
      int index = 1;
      for (auto* core : core_) {
        core->timed(tabIndex_ == index);
        ++index;
      }
    }
  }
}

void ThermalStatus::tabSwitchSlot(int index) {
  tabIndex_ = index;
}

/*
 * ThermalStatusPackage
 */

ThermalStatusPackage::ThermalStatusPackage(
  const SingleCpuInfo& info,
  const SingleCpuId& id,
  const TabMemberValues& values,
  QWidget *parent)
  : QWidget(parent),
    cpuInfo_(info),
    cpuId_(id),
    tabValues_(values) {

  auto* thermalStatus_label = new QLabel("Thermal Status");
  auto* thermalStatusLog_label = new QLabel("Thermal Status log");
  auto* prochotEvent_label = new QLabel("PROCHOT# / FORCEPR#");
  auto* prochotEventLog_label = new QLabel("PROCHOT# / FORCEPR# log");
  auto* criticalTemp_label = new QLabel("Critical Temperature Status");
  auto* criticalTempLog_label = new QLabel("Critical Temperature Status log");
  auto* thermalThreshold1_label = new QLabel("Thermal Threshold 1 Status");
  auto* thermalThreshold1Log_label = new QLabel("Thermal Threshold 1 Status log");
  auto* thermalThreshold2_label = new QLabel("Thermal Threshold 2 Status");
  auto* thermalThreshold2Log_label = new QLabel("Thermal Threshold 2 Status log");
  auto* powerLimitation_label = new QLabel("Power Limitation Status");
  auto* powerLimitationLog_label = new QLabel("Power Limitation Status log");
  auto* digitalReadout_label = new QLabel("Digital Readout");

  thermalStatus_value_ = new QLabel("0");
  thermalStatusLog_value_ = new QLabel("0");
  prochotEvent_value_ = new QLabel("0");
  prochotEventLog_value_ = new QLabel("0");
  criticalTemp_value_ = new QLabel("0");
  criticalTempLog_value_ = new QLabel("0");
  thermalThreshold1_value_ = new QLabel("0");
  thermalThreshold1Log_value_ = new QLabel("0");
  thermalThreshold2_value_ = new QLabel("0");
  thermalThreshold2Log_value_ = new QLabel("0");
  powerLimitation_value_ = new QLabel("0");
  powerLimitationLog_value_ = new QLabel("0");
  digitalReadout_value_ = new QLabel("0");

  thermalStatus_button_ = new QPushButton("Reset");
  prochotEvent_button_ = new QPushButton("Reset");
  criticalTemp_button_ = new QPushButton("Reset");
  thermalThreshold1_button_ = new QPushButton("Reset");
  thermalThreshold2_button_ = new QPushButton("Reset");
  powerLimitation_button_ = new QPushButton("Reset");

  auto* grid = new QGridLayout();
  grid->addWidget(thermalStatus_label,         0, 0);
  grid->addWidget(thermalStatusLog_label,      1, 0);
  grid->addWidget(prochotEvent_label,          3, 0);
  grid->addWidget(prochotEventLog_label,       4, 0);
  grid->addWidget(criticalTemp_label,          6, 0);
  grid->addWidget(criticalTempLog_label,       7, 0);
  grid->addWidget(thermalThreshold1_label,     9, 0);
  grid->addWidget(thermalThreshold1Log_label, 10, 0);
  grid->addWidget(thermalThreshold2_label,    12, 0);
  grid->addWidget(thermalThreshold2Log_label, 13, 0);
  grid->addWidget(powerLimitation_label,      15, 0);
  grid->addWidget(powerLimitationLog_label,   16, 0);
  grid->addWidget(digitalReadout_label,       18, 0);

  grid->addWidget(thermalStatus_value_,         0, 1);
  grid->addWidget(thermalStatusLog_value_,      1, 1);
  grid->addWidget(prochotEvent_value_,          3, 1);
  grid->addWidget(prochotEventLog_value_,       4, 1);
  grid->addWidget(criticalTemp_value_,          6, 1);
  grid->addWidget(criticalTempLog_value_,       7, 1);
  grid->addWidget(thermalThreshold1_value_,     9, 1);
  grid->addWidget(thermalThreshold1Log_value_, 10, 1);
  grid->addWidget(thermalThreshold2_value_,    12, 1);
  grid->addWidget(thermalThreshold2Log_value_, 13, 1);
  grid->addWidget(powerLimitation_value_,      15, 1);
  grid->addWidget(powerLimitationLog_value_,   16, 1);
  grid->addWidget(digitalReadout_value_,       18, 1);

  grid->addWidget(thermalStatus_button_,      0, 3, 2, 1);
  grid->addWidget(prochotEvent_button_,       3, 3, 2, 1);
  grid->addWidget(criticalTemp_button_,       6, 3, 2, 1);
  grid->addWidget(thermalThreshold1_button_,  9, 3, 2, 1);
  grid->addWidget(thermalThreshold2_button_, 12, 3, 2, 1);
  grid->addWidget(powerLimitation_button_,   15, 3, 2, 1);

  grid->setRowMinimumHeight(2, 10);
  grid->setRowMinimumHeight(5, 10);
  grid->setRowMinimumHeight(8, 10);
  grid->setRowMinimumHeight(11, 10);
  grid->setRowMinimumHeight(14, 10);
  grid->setRowMinimumHeight(17, 10);
  grid->setColumnMinimumWidth(2, 10);
  grid->setColumnStretch(4, 1);

  if ((cpuId_.EAX_01H.EDX & (1 << 22)) == 0) {
    thermalStatus_label->setEnabled(false);
    thermalStatusLog_label->setEnabled(false);
    thermalStatus_value_->setEnabled(false);
    thermalStatusLog_value_->setEnabled(false);
    thermalStatus_button_->setEnabled(false);
    prochotEvent_label->setEnabled(false);
    prochotEventLog_label->setEnabled(false);
    prochotEvent_value_->setEnabled(false);
    prochotEventLog_value_->setEnabled(false);
    prochotEvent_button_->setEnabled(false);
    criticalTemp_label->setEnabled(false);
    criticalTempLog_label->setEnabled(false);
    criticalTemp_value_->setEnabled(false);
    criticalTempLog_value_->setEnabled(false);
    criticalTemp_button_->setEnabled(false);
  }

  if ((cpuId_.EAX_01H.ECX & (1 << 8)) == 0) {
    thermalThreshold1_label->setEnabled(false);
    thermalThreshold1Log_label->setEnabled(false);
    thermalThreshold2_label->setEnabled(false);
    thermalThreshold2Log_label->setEnabled(false);
    thermalThreshold1_value_->setEnabled(false);
    thermalThreshold1Log_value_->setEnabled(false);
    thermalThreshold2_value_->setEnabled(false);
    thermalThreshold2Log_value_->setEnabled(false);
    thermalThreshold1_button_->setEnabled(false);
    thermalThreshold2_button_->setEnabled(false);
  }

  if ((cpuId_.EAX_06H.EAX & (1 << 4)) == 0) {
    powerLimitation_label->setEnabled(false);
    powerLimitationLog_label->setEnabled(false);
    powerLimitation_value_->setEnabled(false);
    powerLimitationLog_value_->setEnabled(false);
    powerLimitation_button_->setEnabled(false);
  }

  if ((cpuId_.EAX_06H.EAX & (1 << 0)) == 0) {
    digitalReadout_label->setEnabled(false);
    digitalReadout_value_->setEnabled(false);
  }

  auto* hbox = new QHBoxLayout();
  hbox->addStretch(1);
  hbox->addLayout(grid);
  hbox->addStretch(1);

  /* Add the inner widgets to the layout of the scroll widget */

  auto* scroll_layout = new QVBoxLayout();
  scroll_layout->addStretch(1);
  scroll_layout->addLayout(hbox);
  scroll_layout->addStretch(1);

  /* Setup scroll widget/area */

  scroll_widget_ = new QWidget();
  scroll_widget_->setLayout(scroll_layout);

  scroll_area_ = new QScrollArea();
  scroll_area_->setWidget(scroll_widget_);
  scroll_area_->setWidgetResizable(true);

  /* main layout */

  auto* main_layout = new QVBoxLayout(this);
  main_layout->addWidget(scroll_area_);

  /* Signals */

  connect(thermalStatus_button_, SIGNAL(released()),
      this, SLOT(thermalStatus_reset()));
  connect(prochotEvent_button_, SIGNAL(released()),
      this, SLOT(prochotEvent_reset()));
  connect(criticalTemp_button_, SIGNAL(released()),
      this, SLOT(criticalTemp_reset()));
  connect(thermalThreshold1_button_, SIGNAL(released()),
      this, SLOT(thermalThreshold1_reset()));
  connect(thermalThreshold2_button_, SIGNAL(released()),
      this, SLOT(thermalThreshold2_reset()));
  connect(powerLimitation_button_, SIGNAL(released()),
      this, SLOT(powerLimitation_reset()));

  timed(true);
}

QSize	ThermalStatusPackage::sizeHint() const {
  auto&& size = scroll_widget_->sizeHint();
  return QSize(size.width() + 50, size.height() + 50);
}

void ThermalStatusPackage::timed(bool is_current_tab) {
  if (is_current_tab) {
    IA32_PACKAGE_THERM_STATUS msr(cpuInfo_.firstLogicalCpu());
    msr.read();
    thermalStatus_value_->setText(QString::number(msr.Thermal_Status));
    thermalStatusLog_value_->setText(QString::number(msr.Thermal_Status_Log));
    prochotEvent_value_->setText(QString::number(msr.PROCHOT_Event));
    prochotEventLog_value_->setText(QString::number(msr.PROCHOT_Log));
    criticalTemp_value_->setText(QString::number(msr.Critical_Temperature_Status));
    criticalTempLog_value_->setText(QString::number(msr.Critical_Temperature_Status_Log));
    thermalThreshold1_value_->setText(QString::number(msr.Thermal_Threshold_1_Status));
    thermalThreshold1Log_value_->setText(QString::number(msr.Thermal_Threshold_1_Log));
    thermalThreshold2_value_->setText(QString::number(msr.Thermal_Threshold_2_Status));
    thermalThreshold2Log_value_->setText(QString::number(msr.Thermal_Threshold_2_Log));
    powerLimitation_value_->setText(QString::number(msr.Power_Limitation_Status));
    powerLimitationLog_value_->setText(QString::number(msr.Power_Limitation_Log));
    digitalReadout_value_->setText(std::move(QString(
        "%1 °C").arg(tabValues_.targetTemperature() - msr.Digital_Readout)));

    bool doWrite = false;
    if (thermalStatus_reset_) {
      msr.Thermal_Status_Log = 0;
      thermalStatus_reset_ = false;
      doWrite = true;
    }
    if (prochotEvent_reset_) {
      msr.PROCHOT_Log = 0;
      prochotEvent_reset_ = false;
      doWrite = true;
    }
    if (criticalTemp_reset_) {
      msr.Critical_Temperature_Status_Log = 0;
      doWrite = true;
      criticalTemp_reset_ = false;
    }
    if (thermalThreshold1_reset_) {
      msr.Thermal_Threshold_1_Log = 0;
      thermalThreshold1_reset_ = false;
      doWrite = true;
    }
    if (thermalThreshold2_reset_) {
      msr.Thermal_Threshold_2_Log = 0;
      thermalThreshold2_reset_ = false;
      doWrite = true;
    }
    if (powerLimitation_reset_) {
      msr.Power_Limitation_Log = 0;
      powerLimitation_reset_ = false;
      doWrite = true;
    }
    if (doWrite) {
      msr.write();
    }
  }
}

void ThermalStatusPackage::thermalStatus_reset() {
  thermalStatus_reset_ = true;
}

void ThermalStatusPackage::prochotEvent_reset() {
  prochotEvent_reset_ = true;
}

void ThermalStatusPackage::criticalTemp_reset() {
  criticalTemp_reset_ = true;
}

void ThermalStatusPackage::thermalThreshold1_reset() {
  thermalThreshold1_reset_ = true;
}

void ThermalStatusPackage::thermalThreshold2_reset() {
  thermalThreshold2_reset_ = true;
}

void ThermalStatusPackage::powerLimitation_reset() {
  powerLimitation_reset_ = true;
}

/*
 * ThermalStatusCore
 */

ThermalStatusCore::ThermalStatusCore(
  const SingleCpuInfo& info,
  const SingleCpuId& id,
  const TabMemberValues& values,
  CpuCoreNr core_nr, // 0 based
  QWidget *parent)
  : QWidget(parent),
    cpuInfo_(info),
    cpuId_(id),
    tabValues_(values) {

  cpu_ = cpuInfo_.getLogicalCpu(core_nr);

  auto* thermalStatus_label = new QLabel("Thermal Status");
  auto* thermalStatusLog_label = new QLabel("Thermal Status log");
  auto* prochotEvent_label = new QLabel("PROCHOT# / FORCEPR#");
  auto* prochotEventLog_label = new QLabel("PROCHOT# / FORCEPR# log");
  auto* criticalTemp_label = new QLabel("Critical Temperature Status");
  auto* criticalTempLog_label = new QLabel("Critical Temperature Status log");
  auto* thermalThreshold1_label = new QLabel("Thermal Threshold 1 Status");
  auto* thermalThreshold1Log_label = new QLabel("Thermal Threshold 1 Status log");
  auto* thermalThreshold2_label = new QLabel("Thermal Threshold 2 Status");
  auto* thermalThreshold2Log_label = new QLabel("Thermal Threshold 2 Status log");
  auto* powerLimitation_label = new QLabel("Power Limitation Status");
  auto* powerLimitationLog_label = new QLabel("Power Limitation Status log");
  auto* currentLimitation_label = new QLabel("Current Limitation Status");
  auto* currentLimitationLog_label = new QLabel("Current Limitation Status log");
  auto* domainLimitation_label = new QLabel("Cross Domain Limitation Status");
  auto* domainLimitationLog_label = new QLabel("Cross Domain Limitation Status log");
  auto* digitalReadout_label = new QLabel("Digital Readout");
  auto* resolution_label_ = new QLabel("Digital Readout resolution");
  auto* readingValid_label_ = new QLabel("Readout valid");

  thermalStatus_value_ = new QLabel("0");
  thermalStatusLog_value_ = new QLabel("0");
  prochotEvent_value_ = new QLabel("0");
  prochotEventLog_value_ = new QLabel("0");
  criticalTemp_value_ = new QLabel("0");
  criticalTempLog_value_ = new QLabel("0");
  thermalThreshold1_value_ = new QLabel("0");
  thermalThreshold1Log_value_ = new QLabel("0");
  thermalThreshold2_value_ = new QLabel("0");
  thermalThreshold2Log_value_ = new QLabel("0");
  powerLimitation_value_ = new QLabel("0");
  powerLimitationLog_value_ = new QLabel("0");
  currentLimitation_value_ = new QLabel("0");
  currentLimitationLog_value_ = new QLabel("0");
  domainLimitation_value_ = new QLabel("0");
  domainLimitationLog_value_ = new QLabel("0");
  digitalReadout_value_ = new QLabel("0");
  resolution_value_ = new QLabel("0");
  readingValid_value_ = new QLabel("0");

  thermalStatus_button_ = new QPushButton("Reset");
  prochotEvent_button_ = new QPushButton("Reset");
  criticalTemp_button_ = new QPushButton("Reset");
  thermalThreshold1_button_ = new QPushButton("Reset");
  thermalThreshold2_button_ = new QPushButton("Reset");
  powerLimitation_button_ = new QPushButton("Reset");
  currentLimitation_button_ = new QPushButton("Reset");
  domainLimitation_button_ = new QPushButton("Reset");

  auto* grid = new QGridLayout();
  grid->addWidget(thermalStatus_label,         0, 0);
  grid->addWidget(thermalStatusLog_label,      1, 0);
  grid->addWidget(prochotEvent_label,          3, 0);
  grid->addWidget(prochotEventLog_label,       4, 0);
  grid->addWidget(criticalTemp_label,          6, 0);
  grid->addWidget(criticalTempLog_label,       7, 0);
  grid->addWidget(thermalThreshold1_label,     9, 0);
  grid->addWidget(thermalThreshold1Log_label, 10, 0);
  grid->addWidget(thermalThreshold2_label,    12, 0);
  grid->addWidget(thermalThreshold2Log_label, 13, 0);
  grid->addWidget(powerLimitation_label,      15, 0);
  grid->addWidget(powerLimitationLog_label,   16, 0);
  grid->addWidget(currentLimitation_label,    18, 0);
  grid->addWidget(currentLimitationLog_label, 19, 0);
  grid->addWidget(domainLimitation_label,     21, 0);
  grid->addWidget(domainLimitationLog_label,  22, 0);
  grid->addWidget(digitalReadout_label,       24, 0);
  grid->addWidget(resolution_label_,          26, 0);
  grid->addWidget(readingValid_label_,        28, 0);

  grid->addWidget(thermalStatus_value_,         0, 1);
  grid->addWidget(thermalStatusLog_value_,      1, 1);
  grid->addWidget(prochotEvent_value_,          3, 1);
  grid->addWidget(prochotEventLog_value_,       4, 1);
  grid->addWidget(criticalTemp_value_,          6, 1);
  grid->addWidget(criticalTempLog_value_,       7, 1);
  grid->addWidget(thermalThreshold1_value_,     9, 1);
  grid->addWidget(thermalThreshold1Log_value_, 10, 1);
  grid->addWidget(thermalThreshold2_value_,    12, 1);
  grid->addWidget(thermalThreshold2Log_value_, 13, 1);
  grid->addWidget(powerLimitation_value_,      15, 1);
  grid->addWidget(powerLimitationLog_value_,   16, 1);
  grid->addWidget(currentLimitation_value_,    18, 1);
  grid->addWidget(currentLimitationLog_value_, 19, 1);
  grid->addWidget(domainLimitation_value_,     21, 1);
  grid->addWidget(domainLimitationLog_value_,  22, 1);
  grid->addWidget(digitalReadout_value_,       24, 1);
  grid->addWidget(resolution_value_,           26, 1);
  grid->addWidget(readingValid_value_,         28, 1);

  grid->addWidget(thermalStatus_button_,      0, 3, 2, 1);
  grid->addWidget(prochotEvent_button_,       3, 3, 2, 1);
  grid->addWidget(criticalTemp_button_,       6, 3, 2, 1);
  grid->addWidget(thermalThreshold1_button_,  9, 3, 2, 1);
  grid->addWidget(thermalThreshold2_button_, 12, 3, 2, 1);
  grid->addWidget(powerLimitation_button_,   15, 3, 2, 1);
  grid->addWidget(currentLimitation_button_, 18, 3, 2, 1);
  grid->addWidget(domainLimitation_button_,  21, 3, 2, 1);

  grid->setRowMinimumHeight(2, 10);
  grid->setRowMinimumHeight(5, 10);
  grid->setRowMinimumHeight(8, 10);
  grid->setRowMinimumHeight(11, 10);
  grid->setRowMinimumHeight(14, 10);
  grid->setRowMinimumHeight(17, 10);
  grid->setRowMinimumHeight(20, 10);
  grid->setRowMinimumHeight(23, 10);
  grid->setRowMinimumHeight(25, 10);
  grid->setRowMinimumHeight(27, 10);
  grid->setColumnMinimumWidth(2, 10);
  grid->setColumnStretch(4, 1);

  if ((cpuId_.EAX_01H.ECX & (1 << 8)) == 0) { // if TM2 not supported
    thermalThreshold1_label->setEnabled(false);
    thermalThreshold1Log_label->setEnabled(false);
    thermalThreshold2_label->setEnabled(false);
    thermalThreshold2Log_label->setEnabled(false);
    thermalThreshold1_value_->setEnabled(false);
    thermalThreshold1Log_value_->setEnabled(false);
    thermalThreshold2_value_->setEnabled(false);
    thermalThreshold2Log_value_->setEnabled(false);
    thermalThreshold1_button_->setEnabled(false);
    thermalThreshold2_button_->setEnabled(false);
  }

  if ((cpuId_.EAX_06H.EAX & (1 << 7)) == 0) { // if HWP not supported
    currentLimitation_label->setEnabled(false);
    currentLimitationLog_label->setEnabled(false);
    currentLimitation_value_->setEnabled(false);
    currentLimitationLog_value_->setEnabled(false);
    currentLimitation_button_->setEnabled(false);
    domainLimitation_label->setEnabled(false);
    domainLimitationLog_label->setEnabled(false);
    domainLimitation_value_->setEnabled(false);
    domainLimitationLog_value_->setEnabled(false);
    domainLimitation_button_->setEnabled(false);
  }

  if ((cpuId_.EAX_06H.EAX & (1 << 0)) == 0) { // if Digital temperature sensor not supported
    digitalReadout_label->setEnabled(false);
    digitalReadout_value_->setEnabled(false);
    resolution_label_->setEnabled(false);
    resolution_value_->setEnabled(false);
    readingValid_label_->setEnabled(false);
    readingValid_value_->setEnabled(false);
  }

  if ((cpuId_.EAX_06H.EAX & (1 << 4)) == 0) { // if PLN not supported
    powerLimitation_label->setEnabled(false);
    powerLimitationLog_label->setEnabled(false);
    powerLimitation_value_->setEnabled(false);
    powerLimitationLog_value_->setEnabled(false);
    powerLimitation_button_->setEnabled(false);
  }

  auto* hbox = new QHBoxLayout();
  hbox->addStretch(1);
  hbox->addLayout(grid);
  hbox->addStretch(1);

  /* Add the inner widgets to the layout of the scroll widget */

  auto* scroll_layout = new QVBoxLayout();
  scroll_layout->addStretch(1);
  scroll_layout->addLayout(hbox);
  scroll_layout->addStretch(1);

  /* Setup scroll widget/area */

  scroll_widget_ = new QWidget();
  scroll_widget_->setLayout(scroll_layout);

  scroll_area_ = new QScrollArea();
  scroll_area_->setWidget(scroll_widget_);
  scroll_area_->setWidgetResizable(true);

  /* main layout */

  auto* main_layout = new QVBoxLayout(this);
  main_layout->addWidget(scroll_area_);

  /* Signals */

  connect(thermalStatus_button_, SIGNAL(released()), this, SLOT(thermalStatus_reset()));
  connect(prochotEvent_button_, SIGNAL(released()), this, SLOT(prochotEvent_reset()));
  connect(criticalTemp_button_, SIGNAL(released()), this, SLOT(criticalTemp_reset()));
  connect(thermalThreshold1_button_, SIGNAL(released()), this, SLOT(thermalThreshold1_reset()));
  connect(thermalThreshold2_button_, SIGNAL(released()), this, SLOT(thermalThreshold2_reset()));
  connect(powerLimitation_button_, SIGNAL(released()), this, SLOT(powerLimitation_reset()));
  connect(currentLimitation_button_, SIGNAL(released()), this, SLOT(currentLimitation_reset()));
  connect(domainLimitation_button_, SIGNAL(released()), this, SLOT(domainLimitation_reset()));

  timed(true);
}

QSize	ThermalStatusCore::sizeHint() const {
  auto&& size = scroll_widget_->sizeHint();
  return QSize(size.width() + 50, size.height() + 50);
}

void ThermalStatusCore::timed(bool is_current_tab) {
  if (is_current_tab) {
    IA32_THERM_STATUS msr(cpu_);
    msr.read();
    thermalStatus_value_->setText(QString::number(msr.Thermal_Status));
    thermalStatusLog_value_->setText(QString::number(msr.Thermal_Status_Log));
    prochotEvent_value_->setText(QString::number(msr.PROCHOT_Or_FORCEPR_Event));
    prochotEventLog_value_->setText(QString::number(msr.PROCHOT_Or_FORCEPR_Log));
    criticalTemp_value_->setText(QString::number(msr.Critical_Temperature_Status));
    criticalTempLog_value_->setText(QString::number(msr.Critical_Temperature_Status_Log));
    thermalThreshold1_value_->setText(QString::number(msr.Thermal_Threshold_1_Status));
    thermalThreshold1Log_value_->setText(QString::number(msr.Thermal_Threshold_1_Log));
    thermalThreshold2_value_->setText(QString::number(msr.Thermal_Threshold_2_Status));
    thermalThreshold2Log_value_->setText(QString::number(msr.Thermal_Threshold_2_Log));
    powerLimitation_value_->setText(QString::number(msr.Power_Limitation_Status));
    powerLimitationLog_value_->setText(QString::number(msr.Power_Limitation_Log));
    currentLimitation_value_->setText(QString::number(msr.Current_Limit_Status));
    currentLimitationLog_value_->setText(QString::number(msr.Current_Limit_Log));
    domainLimitation_value_->setText(QString::number(msr.Cross_Domain_Limit_Status));
    domainLimitationLog_value_->setText(QString::number(msr.Cross_Domain_Limit_Log));
    readingValid_value_->setText(QString::number(msr.Reading_Valid));
    digitalReadout_value_->setText(std::move(QString(
        "%1 °C").arg(tabValues_.targetTemperature() - msr.Digital_Readout)));
    resolution_value_->setText(std::move(QString(
        "%1 °C").arg(msr.Resolution_In_Degrees_Celsius)));

    bool doWrite = false;
    if (thermalStatus_reset_) {
      msr.Thermal_Status_Log = 0;
      thermalStatus_reset_ = false;
      doWrite = true;
    }
    if (prochotEvent_reset_) {
      msr.PROCHOT_Or_FORCEPR_Log = 0;
      prochotEvent_reset_ = false;
      doWrite = true;
    }
    if (criticalTemp_reset_) {
      msr.Critical_Temperature_Status_Log = 0;
      doWrite = true;
      criticalTemp_reset_ = false;
    }
    if (thermalThreshold1_reset_) {
      msr.Thermal_Threshold_1_Log = 0;
      thermalThreshold1_reset_ = false;
      doWrite = true;
    }
    if (thermalThreshold2_reset_) {
      msr.Thermal_Threshold_2_Log = 0;
      thermalThreshold2_reset_ = false;
      doWrite = true;
    }
    if (powerLimitation_reset_) {
      msr.Power_Limitation_Log = 0;
      powerLimitation_reset_ = false;
      doWrite = true;
    }
    if (currentLimitation_reset_) {
      msr.Current_Limit_Log = 0;
      currentLimitation_reset_ = false;
      doWrite = true;
    }
    if (domainLimitation_reset_) {
      msr.Cross_Domain_Limit_Log = 0;
      domainLimitation_reset_ = false;
      doWrite = true;
    }
    if (doWrite) {
      msr.write();
    }
  }
}

void ThermalStatusCore::thermalStatus_reset() {
  thermalStatus_reset_ = true;
}

void ThermalStatusCore::prochotEvent_reset() {
  prochotEvent_reset_ = true;
}

void ThermalStatusCore::criticalTemp_reset() {
  criticalTemp_reset_ = true;
}

void ThermalStatusCore::thermalThreshold1_reset() {
  thermalThreshold1_reset_ = true;
}

void ThermalStatusCore::thermalThreshold2_reset() {
  thermalThreshold2_reset_ = true;
}

void ThermalStatusCore::powerLimitation_reset() {
  powerLimitation_reset_ = true;
}

void ThermalStatusCore::currentLimitation_reset() {
  currentLimitation_reset_ = true;
}

void ThermalStatusCore::domainLimitation_reset() {
  domainLimitation_reset_ = true;
}

