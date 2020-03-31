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
  * @file src/core-adjust-qt/VoltageOffsets.hpp
  * @brief A TabMemberWidget for adjusting FIVR Voltage Offsets (undocumented MSR 0x150). 
  *
  * @file src/core-adjust-qt/VoltageOffsets.cpp
  * @brief A TabMemberWidget for adjusting FIVR Voltage Offsets (undocumented MSR 0x150) (implementation). 
  *
  * @class VoltageOffsets
  * @brief A TabMemberWidget for adjusting FIVR Voltage Offsets (undocumented MSR 0x150).
  *
  * @fn explicit VoltageOffsets::VoltageOffsets(const SingleCpuInfo& cpuInfo, const SingleCpuId& cpuId, TabMemberValues& tabValues, TabMemberSettings& tabSettings, QWidget* parent)
  * @param cpuInfo
  * Reference to the global CpuInfo instance.
  * @param cpuId
  * Reference to the global CpuId instance.
  * @param tabValues
  * Reference to the TabMemberValues instance.
  * @param tabSettings
  * Reference to the TabMemberSettings instance.
  * @param parent
  * Pointer to the parent widget.
  *
  *
  * @class VoltageOffsets::Values
  * @brief Values to read from the processor
  *
  * @fn double VoltageOffsets::Values::plane0VoltageOffset() const
  * @brief Get the CPU FIVR Voltage Offset for voltage plane 0
  * @returns The voltage offset in millivolts.
  *
  * @fn double VoltageOffsets::Values::plane1VoltageOffset() const
  * @brief Get the CPU FIVR Voltage Offset for voltage plane 1
  * @returns The voltage offset in millivolts.
  *
  * @fn double VoltageOffsets::Values::plane2VoltageOffset() const
  * @brief Get the CPU FIVR Voltage Offset for voltage plane 2
  * @returns The voltage offset in millivolts.
  *
  * @fn double VoltageOffsets::Values::plane3VoltageOffset() const
  * @brief Get the CPU FIVR Voltage Offset for voltage plane 3
  * @returns The voltage offset in millivolts.
  *
  * @fn double VoltageOffsets::Values::plane4VoltageOffset() const
  * @brief Get the CPU FIVR Voltage Offset for voltage plane 4
  * @returns The voltage offset in millivolts.
  *
  * @fn double VoltageOffsets::Values::plane5VoltageOffset() const
  * @brief Get the CPU FIVR Voltage Offset for voltage plane 5
  * @returns The voltage offset in millivolts.
  *
  * @fn void VoltageOffsets::Values::plane0VoltageOffset(double vo)
  * @brief Set the CPU FIVR Voltage Offset for voltage plane 0
  * @param vo The voltage offset in millivolts.
  *
  * @fn void VoltageOffsets::Values::plane1VoltageOffset(double vo)
  * @brief Set the CPU FIVR Voltage Offset for voltage plane 1
  * @param vo The voltage offset in millivolts.
  *
  * @fn void VoltageOffsets::Values::plane2VoltageOffset(double vo)
  * @brief Set the CPU FIVR Voltage Offset for voltage plane 2
  * @param vo The voltage offset in millivolts.
  *
  * @fn void VoltageOffsets::Values::plane3VoltageOffset(double vo)
  * @brief Set the CPU FIVR Voltage Offset for voltage plane 3
  * @param vo The voltage offset in millivolts.
  *
  * @fn void VoltageOffsets::Values::plane4VoltageOffset(double vo)
  * @brief Set the CPU FIVR Voltage Offset for voltage plane 4
  * @param vo The voltage offset in millivolts.
  *
  * @fn void VoltageOffsets::Values::plane5VoltageOffset(double vo)
  * @brief Set the CPU FIVR Voltage Offset for voltage plane 5
  * @param vo The voltage offset in millivolts.
  *
  *
  * @class VoltageOffsets::Settings
  * @brief Settings for class VoltageOffsets
  *
  * @var static constexpr const double VoltageOffsets::Settings::VOLTAGE_OFFSET_STEP { 0.9765625 };
  * @brief The smallest possible increase/decrease of the various voltage offsets (1/1024 Volt).
  *
  * @var static constexpr const double VoltageOffsets::Settings::VOLTAGE_OFFSET_MIN { -999.0234375 };
  * @brief The largest possible negetive voltage offset.
  *
  * @var static constexpr const double VoltageOffsets::Settings::VOLTAGE_OFFSET_MAX { 999.0234375 };
  * @brief The largest possible positive voltage offset.
  *
  * @fn bool VoltageOffsets::Settings::plane0VoltageOffsetEnabled() const
  * @brief Read TabMemberSettings::enable_plane0_voffset_.
  * @returns The enabled state.
  *
  * @fn bool VoltageOffsets::Settings::plane1VoltageOffsetEnabled() const
  * @brief Read TabMemberSettings::enable_plane1_voffset_.
  * @returns The enabled state.
  *
  * @fn bool VoltageOffsets::Settings::plane2VoltageOffsetEnabled() const
  * @brief Read TabMemberSettings::enable_plane2_voffset_.
  * @returns The enabled state.
  *
  * @fn bool VoltageOffsets::Settings::plane3VoltageOffsetEnabled() const
  * @brief Read TabMemberSettings::enable_plane3_voffset_.
  * @returns The enabled state.
  *
  * @fn bool VoltageOffsets::Settings::plane4VoltageOffsetEnabled() const
  * @brief Read enable_plane4_voffset_.
  * @returns The enabled state.
  *
  * @fn bool VoltageOffsets::Settings::plane5VoltageOffsetEnabled() const
  * @brief Read TabMemberSettings::enable_plane5_voffset_.
  * @returns The enabled state.
  *
  * @fn void VoltageOffsets::Settings::plane0VoltageOffsetEnabled(bool state)
  * @brief Set TabMemberSettings::enable_plane0_voffset_.
  * @param state The new enabled state.
  *
  * @fn void VoltageOffsets::Settings::plane1VoltageOffsetEnabled(bool state)
  * @brief Set TabMemberSettings::enable_plane1_voffset_.
  * @param state The new enabled state.
  *
  * @fn void VoltageOffsets::Settings::plane2VoltageOffsetEnabled(bool state)
  * @brief Set TabMemberSettings::enable_plane2_voffset_.
  * @param state The new enabled state.
  *
  * @fn void VoltageOffsets::Settings::plane3VoltageOffsetEnabled(bool state)
  * @brief Set TabMemberSettings::enable_plane3_voffset_.
  * @param state The new enabled state.
  *
  * @fn void VoltageOffsets::Settings::plane4VoltageOffsetEnabled(bool state)
  * @brief Set TabMemberSettings::enable_plane4_voffset_.
  * @param state The new enabled state.
  *
  * @fn void VoltageOffsets::Settings::plane5VoltageOffsetEnabled(bool state)
  * @brief Set TabMemberSettings::enable_plane5_voffset_.
  * @param state The new enabled state.
  *
  * @fn bool VoltageOffsets::Settings::allowPositiveValues() const
  * @brief Read TabMemberSettings::allow_positive_values_.
  * @returns The allowed state.
  *
  * @fn void VoltageOffsets::Settings::allowPositiveValues(bool state)
  * @brief Set TabMemberSettings::allow_positive_values_.
  * @param state The new allowed state.
  */
// STL
#include <iomanip>
// Qt
#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QSplitter>
// App
#include "CpuInfo.hpp"
#include "Dbg.hpp"
#include "Shell.hpp"
#include "TabMember.hpp"
#include "VoltageOffsets.hpp"

/* Safe floating point boolean == operation */
#define EQUAL(a, b) ((a) <= (b) && (a) >= (b))

VoltageOffsets::VoltageOffsets(
    const SingleCpuInfo& i, const SingleCpuId& c, TabMemberValues& a,
    TabMemberSettings& s, QWidget* p)
    : TabMemberTemplate(i, c, a, s, p),
      shell_(this, "Adjust FIVR Voltage Offsets",
          ShellCommand::EnableStd | ShellCommand::DisableAbortButton) {

  bool enable_widget = false;

  auto* splitter = new QSplitter(Qt::Horizontal);
  auto* group_box_cv = new QGroupBox(tr("Current FIVR Voltage Offsets"));
  auto* group_box = new QGroupBox(tr("Adjust FIVR Voltage Offsets"));
  auto* force_label = new QLabel(tr("Allow overvoltage"));
  force_voltage_ = new QCheckBox();

  auto&& ivrNames = SingleCpuInfo::IvrNames(cpuInfo().MicroArch);
  plane0_ = new VoltageOffsetSlider(ivrNames[0].c_str());
  plane1_ = new VoltageOffsetSlider(ivrNames[1].c_str());
  plane2_ = new VoltageOffsetSlider(ivrNames[2].c_str());
  plane3_ = new VoltageOffsetSlider(ivrNames[3].c_str());
  plane4_ = new VoltageOffsetSlider(ivrNames[4].c_str());
  plane5_ = new VoltageOffsetSlider(ivrNames[5].c_str());

  auto* current_plane0_label = new QLabel(QString::fromStdString(ivrNames[0]));
  auto* current_plane1_label = new QLabel(QString::fromStdString(ivrNames[1]));
  auto* current_plane2_label = new QLabel(QString::fromStdString(ivrNames[2]));
  auto* current_plane3_label = new QLabel(QString::fromStdString(ivrNames[3]));
  auto* current_plane4_label = new QLabel(QString::fromStdString(ivrNames[4]));
  auto* current_plane5_label = new QLabel(QString::fromStdString(ivrNames[5]));
  current_plane0_value_ = new QLabel();
  current_plane1_value_ = new QLabel();
  current_plane2_value_ = new QLabel();
  current_plane3_value_ = new QLabel();
  current_plane4_value_ = new QLabel();
  current_plane5_value_ = new QLabel();

  current_plane0_label->setMinimumWidth(80);
  current_plane1_label->setMinimumWidth(80);
  current_plane2_label->setMinimumWidth(80);
  current_plane3_label->setMinimumWidth(80);
  current_plane4_label->setMinimumWidth(80);
  current_plane5_label->setMinimumWidth(80);
  current_plane0_value_->setMinimumWidth(80);
  current_plane1_value_->setMinimumWidth(80);
  current_plane2_value_->setMinimumWidth(80);
  current_plane3_value_->setMinimumWidth(80);
  current_plane4_value_->setMinimumWidth(80);
  current_plane5_value_->setMinimumWidth(80);

  current_plane0_label->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  current_plane1_label->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  current_plane2_label->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  current_plane3_label->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  current_plane4_label->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  current_plane5_label->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  current_plane0_value_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  current_plane1_value_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  current_plane2_value_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  current_plane3_value_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  current_plane4_value_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  current_plane5_value_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

  if (cpuInfo().isSkyLakeOrLater()) {
    plane5_->setEnabled(false);
    enable_widget = true;
  }
  else if (cpuInfo().isHaswellOrLater()) {
    enable_widget = true;
  }
  else {
    enable_widget = false;
  }
  setEnabled(enable_widget);

  /* layout of the widgets in the scroll area */

  auto* group_box_cv_layout = new QHBoxLayout();
  auto* group_box_layout = new QVBoxLayout();
  auto* voltage_offsets = new QVBoxLayout();
  auto* hbox_force = new QHBoxLayout();
  auto* grid_current_voltages = new QGridLayout();

  grid_current_voltages->addWidget(current_plane0_label, 0, 0);
  grid_current_voltages->addWidget(current_plane1_label, 0, 1);
  grid_current_voltages->addWidget(current_plane2_label, 0, 2);
  grid_current_voltages->addWidget(current_plane3_label, 0, 3);
  grid_current_voltages->addWidget(current_plane4_label, 0, 4);
  grid_current_voltages->addWidget(current_plane5_label, 0, 5);
  grid_current_voltages->addWidget(current_plane0_value_, 1, 0);
  grid_current_voltages->addWidget(current_plane1_value_, 1, 1);
  grid_current_voltages->addWidget(current_plane2_value_, 1, 2);
  grid_current_voltages->addWidget(current_plane3_value_, 1, 3);
  grid_current_voltages->addWidget(current_plane4_value_, 1, 4);
  grid_current_voltages->addWidget(current_plane5_value_, 1, 5);

  group_box_cv_layout->addLayout(grid_current_voltages);

  voltage_offsets->addWidget(plane0_);
  voltage_offsets->addWidget(splitter);
  voltage_offsets->addWidget(plane1_);
  voltage_offsets->addWidget(splitter);
  voltage_offsets->addWidget(plane2_);
  voltage_offsets->addWidget(splitter);
  voltage_offsets->addWidget(plane3_);
  voltage_offsets->addWidget(splitter);
  voltage_offsets->addWidget(plane4_);
  voltage_offsets->addWidget(splitter);
  voltage_offsets->addWidget(plane5_);

  hbox_force->addStretch(1);
  hbox_force->addWidget(force_label);
  hbox_force->addWidget(force_voltage_);

  group_box_layout->addLayout(voltage_offsets);
  group_box_layout->addSpacing(10);
  group_box_layout->addLayout(hbox_force);

  group_box_cv->setLayout(group_box_cv_layout);
  group_box_cv->setFlat(true);

  group_box->setLayout(group_box_layout);
  group_box->setFlat(true);

  /* Add the inner widgets to the layout of the scroll widget */

  scroll_layout_->addSpacing(10);
  scroll_layout_->addWidget(group_box_cv);
  scroll_layout_->addWidget(splitter);
  scroll_layout_->addWidget(group_box);

  /* signals */

  connect(&shell_, SIGNAL(finished()), &shell_, SLOT(accept()));

  connect(force_voltage_, SIGNAL(stateChanged(int)),
      this, SLOT(forceVoltageChecked(int)));

  connect(plane0_, SIGNAL(voltageChanged(void)),
      this, SLOT(voltageChanged(void)));
  connect(plane1_, SIGNAL(voltageChanged(void)),
      this, SLOT(voltageChanged(void)));
  connect(plane2_, SIGNAL(voltageChanged(void)),
      this, SLOT(voltageChanged(void)));
  connect(plane3_, SIGNAL(voltageChanged(void)),
      this, SLOT(voltageChanged(void)));
  connect(plane4_, SIGNAL(voltageChanged(void)),
      this, SLOT(voltageChanged(void)));
  connect(plane5_, SIGNAL(voltageChanged(void)),
      this, SLOT(voltageChanged(void)));

  connect(plane0_, SIGNAL(rangeSelected(enum VoltageOffsetSlider::Range)),
      this, SLOT(plane0RangeSelected(enum VoltageOffsetSlider::Range)));
  connect(plane1_, SIGNAL(rangeSelected(enum VoltageOffsetSlider::Range)),
      this, SLOT(plane1RangeSelected(enum VoltageOffsetSlider::Range)));
  connect(plane2_, SIGNAL(rangeSelected(enum VoltageOffsetSlider::Range)),
      this, SLOT(plane2RangeSelected(enum VoltageOffsetSlider::Range)));
  connect(plane3_, SIGNAL(rangeSelected(enum VoltageOffsetSlider::Range)),
      this, SLOT(plane3RangeSelected(enum VoltageOffsetSlider::Range)));
  connect(plane4_, SIGNAL(rangeSelected(enum VoltageOffsetSlider::Range)),
      this, SLOT(plane4RangeSelected(enum VoltageOffsetSlider::Range)));
  connect(plane5_, SIGNAL(rangeSelected(enum VoltageOffsetSlider::Range)),
      this, SLOT(plane5RangeSelected(enum VoltageOffsetSlider::Range)));
}

bool VoltageOffsets::read(std::ostringstream& ss) {
  bool retv = true;

  /*
   * Read the values for this tab from the processor
   */

  /* Set defaults */
  tabValues().plane0VoltageOffset(0.);
  tabValues().plane1VoltageOffset(0.);
  tabValues().plane2VoltageOffset(0.);
  tabValues().plane3VoltageOffset(0.);
  tabValues().plane4VoltageOffset(0.);
  tabValues().plane5VoltageOffset(0.);

  /* run the core-adjust script */
  std::vector<std::string> output;
  auto rv = xxx::shell_command({
    TabSettings::ScriptPath,
    "--processor", std::to_string(cpuInfo().physicalId().value),
    "--verbose", "--read" },
    [&output](auto, auto str){
      output.push_back(std::move(str));
      return 0;
    }
  );

  /*
   * Expected output (data @ column 31):
   * line 0: 'processor #0'
   * line 1: '  default target temperature :  100 °C'
   * line 2: '  temperature offset range   :   15 °C'
   * line 3: '  target temperature         :   98 °C'
   * line 4: '  voltage plane #0 offset    :  -65.4296875 mV'
   * line 5: '  voltage plane #1 offset    :  -65.4296875 mV'
   * line 6: '  voltage plane #2 offset    :  -65.4296875 mV'
   * line 7: '  voltage plane #3 offset    :  -90.8203125 mV'
   * line 8: '  voltage plane #4 offset    :  -49.8046875 mV'
   * line 9: '  voltage plane #5 offset    :  -49.8046875 mV'
   */
  if (!rv && output.size() >= 10) {
    tabValues().plane0VoltageOffset(std::strtod(&output[4].data()[31], nullptr));
    tabValues().plane1VoltageOffset(std::strtod(&output[5].data()[31], nullptr));
    tabValues().plane2VoltageOffset(std::strtod(&output[6].data()[31], nullptr));
    tabValues().plane3VoltageOffset(std::strtod(&output[7].data()[31], nullptr));
    tabValues().plane4VoltageOffset(std::strtod(&output[8].data()[31], nullptr));
    tabValues().plane5VoltageOffset(std::strtod(&output[9].data()[31], nullptr));
  }
  else {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, unexpected output from command (VoltageOffsets):</b></p>"
      "<nobr>core-adjust core-adjust --read --verbose</nobr>");
    setEnabled(false);
    return true;
  }

  /*
   * Compare the newly read values against the desired values
   */

  if (tabSettings().plane0VoltageOffsetEnabled()) {
    if (!EQUAL(tabSettings().plane0VoltageOffset(), tabValues().plane0VoltageOffset())) {
      ss << "<li><nobr>The <b>FIVR Voltage Offset</b> for processor "
         << cpuInfo().physicalId().value << ", plane 0 is <b>"
         << tabValues().plane0VoltageOffset() << "</b> instead of <b>"
         << tabSettings().plane0VoltageOffset() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().plane1VoltageOffsetEnabled()) {
    if (!EQUAL(tabSettings().plane1VoltageOffset(), tabValues().plane1VoltageOffset())) {
      ss << "<li><nobr>The <b>FIVR Voltage Offset</b> for processor "
         << cpuInfo().physicalId().value << ", plane 1 is <b>"
         << tabValues().plane1VoltageOffset() << "</b> instead of <b>"
         << tabSettings().plane1VoltageOffset() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().plane2VoltageOffsetEnabled()) {
    if (!EQUAL(tabSettings().plane2VoltageOffset(), tabValues().plane2VoltageOffset())) {
      ss << "<li><nobr>The <b>FIVR Voltage Offset</b> for processor "
         << cpuInfo().physicalId().value << ", plane 2 is <b>"
         << tabValues().plane2VoltageOffset() << "</b> instead of <b>"
         << tabSettings().plane2VoltageOffset() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().plane3VoltageOffsetEnabled()) {
    if (!EQUAL(tabSettings().plane3VoltageOffset(), tabValues().plane3VoltageOffset())) {
      ss << "<li><nobr>The <b>FIVR Voltage Offset</b> for processor "
         << cpuInfo().physicalId().value << ", plane 3 is <b>"
         << tabValues().plane3VoltageOffset() << "</b> instead of <b>"
         << tabSettings().plane3VoltageOffset() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().plane4VoltageOffsetEnabled()) {
    if (!EQUAL(tabSettings().plane4VoltageOffset(), tabValues().plane4VoltageOffset())) {
      ss << "<li><nobr>The <b>FIVR Voltage Offset</b> for processor "
         << cpuInfo().physicalId().value << ", plane 4 is <b>"
         << tabValues().plane4VoltageOffset() << "</b> instead of <b>"
         << tabSettings().plane4VoltageOffset() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().plane5VoltageOffsetEnabled()) {
    if (!EQUAL(tabSettings().plane5VoltageOffset(), tabValues().plane5VoltageOffset())) {
      ss << "<li><nobr>The <b>FIVR Voltage Offset</b> for processor "
         << cpuInfo().physicalId().value << ", plane 5 is <b>"
         << tabValues().plane5VoltageOffset() << "</b> instead of <b>"
         << tabSettings().plane5VoltageOffset() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  /*
   * Return false if the newly read values differ from the desired values.
   */
  return retv;
}

void VoltageOffsets::load() {
  force_voltage_->setChecked(tabSettings().allowPositiveValues());
  plane0_->setVoltage(tabSettings().plane0VoltageOffset(), true, tabSettings().allowPositiveValues());
  plane1_->setVoltage(tabSettings().plane1VoltageOffset(), true, tabSettings().allowPositiveValues());
  plane2_->setVoltage(tabSettings().plane2VoltageOffset(), true, tabSettings().allowPositiveValues());
  plane3_->setVoltage(tabSettings().plane3VoltageOffset(), true, tabSettings().allowPositiveValues());
  plane4_->setVoltage(tabSettings().plane4VoltageOffset(), true, tabSettings().allowPositiveValues());
  plane5_->setVoltage(tabSettings().plane5VoltageOffset(), true, tabSettings().allowPositiveValues());
  plane0_->setChecked(tabSettings().plane0VoltageOffsetEnabled());
  plane1_->setChecked(tabSettings().plane1VoltageOffsetEnabled());
  plane2_->setChecked(tabSettings().plane2VoltageOffsetEnabled());
  plane3_->setChecked(tabSettings().plane3VoltageOffsetEnabled());
  plane4_->setChecked(tabSettings().plane4VoltageOffsetEnabled());
  plane5_->setChecked(tabSettings().plane5VoltageOffsetEnabled());
}

void VoltageOffsets::store() {
  /* Store current UI values to the global TabMemberSettings instance */
  store(tabSettings());
}

void VoltageOffsets::store(Settings& data) {
  /* Store current UI values to the specified TabMemberSettings instance */
  data.allowPositiveValues(force_voltage_->isChecked());
  if (plane0_->isChecked()) {
    data.plane0VoltageOffset(plane0_->getVoltage());
  }
  if (plane1_->isChecked()) {
    data.plane1VoltageOffset(plane1_->getVoltage());
  }
  if (plane2_->isChecked()) {
    data.plane2VoltageOffset(plane2_->getVoltage());
  }
  if (plane3_->isChecked()) {
    data.plane3VoltageOffset(plane3_->getVoltage());
  }
  if (plane4_->isChecked()) {
    data.plane4VoltageOffset(plane4_->getVoltage());
  }
  if (plane5_->isChecked()) {
    data.plane5VoltageOffset(plane5_->getVoltage());
  }
  data.plane0VoltageOffsetEnabled(plane0_->isChecked());
  data.plane1VoltageOffsetEnabled(plane1_->isChecked());
  data.plane2VoltageOffsetEnabled(plane2_->isChecked());
  data.plane3VoltageOffsetEnabled(plane3_->isChecked());
  data.plane4VoltageOffsetEnabled(plane4_->isChecked());
  data.plane5VoltageOffsetEnabled(plane5_->isChecked());
}

void VoltageOffsets::refresh() {
  static const std::string red = "<font color='red'>";
  static const std::string green = "<font color='green'>";

  /* Exit if we are disabled */
  if (!isEnabled()) return;

  std::stringstream ss;

  ss << std::setprecision(1) << std::fixed << std::setw(6)
     << std::showpos << ((tabValues().plane0VoltageOffset() > 0.) ? red : green)
     << tabValues().plane0VoltageOffset() << " mV</font>";
  current_plane0_value_->setText(QString::fromStdString(ss.str()));

  ss.str(std::string());
  ss.clear();
  ss << ((tabValues().plane1VoltageOffset() > 0.) ? red : green)
     << tabValues().plane1VoltageOffset() << " mV</font>";
  current_plane1_value_->setText(QString::fromStdString(ss.str()));

  ss.str(std::string());
  ss.clear();
  ss << ((tabValues().plane2VoltageOffset() > 0.) ? red : green)
     << tabValues().plane2VoltageOffset() << " mV</font>";
  current_plane2_value_->setText(QString::fromStdString(ss.str()));

  ss.str(std::string());
  ss.clear();
  ss << ((tabValues().plane3VoltageOffset() > 0.) ? red : green)
     << tabValues().plane3VoltageOffset() << " mV</font>";
  current_plane3_value_->setText(QString::fromStdString(ss.str()));

  ss.str(std::string());
  ss.clear();
  ss << ((tabValues().plane4VoltageOffset() > 0.) ? red : green)
     << tabValues().plane4VoltageOffset() << " mV</font>";
  current_plane4_value_->setText(QString::fromStdString(ss.str()));

  ss.str(std::string());
  ss.clear();
  ss << ((tabValues().plane5VoltageOffset() > 0.) ? red : green)
     << tabValues().plane5VoltageOffset() << " mV</font>";
  current_plane5_value_->setText(QString::fromStdString(ss.str()));
}

bool VoltageOffsets::apply() {
  DBGMSG("VoltageOffsets::apply starts")

  /* Exit if we are disabled */
  if (!isEnabled()) return true;

  /* set to true when we need to run the script */
  bool doExec = false;

  /* Start assembling the command to execute */
  std::vector<std::string> cmd {
    TabSettings::ScriptPath,
    "--verbose",
    "--processor", std::to_string(cpuInfo().physicalId().value)
  };

  /* Allow positive voltages if desired */
  if (tabSettings().allowPositiveValues()) {
    cmd.emplace_back("--force");
  }

  /*
   * Set CPU FIVR Voltage Offsets
   *
   * The Voltage offset for each plane is set when:
   *  a) the voltage plane is enabled in the configuration.
   *  b) the current value of the voltage plane differs from the configured value.
   */

  /* FIVR Voltage Offset Plane 0 */  
  if (tabSettings().plane0VoltageOffsetEnabled()) {
    if (!EQUAL(tabSettings().plane0VoltageOffset(),
        tabValues().plane0VoltageOffset())) {
      cmd.emplace_back("--plane0");
      std::stringstream ss;
      ss << std::setprecision(7) << std::fixed << std::noshowpos
         << tabSettings().plane0VoltageOffset();
      cmd.emplace_back(ss.str());
      doExec = true;
    }
  }

  /* FIVR Voltage Offset Plane 1 */  
  if (tabSettings().plane1VoltageOffsetEnabled()) {
    if (!EQUAL(tabSettings().plane1VoltageOffset(),
        tabValues().plane1VoltageOffset())) {
      cmd.emplace_back("--plane1");
      std::stringstream ss;
      ss << std::setprecision(7) << std::fixed << std::noshowpos
         << tabSettings().plane1VoltageOffset();
      cmd.emplace_back(ss.str());
      doExec = true;
    }
  }

  /* FIVR Voltage Offset Plane 2 */  
  if (tabSettings().plane2VoltageOffsetEnabled()) {
    if (!EQUAL(tabSettings().plane2VoltageOffset(),
        tabValues().plane2VoltageOffset())) {
      cmd.emplace_back("--plane2");
      std::stringstream ss;
      ss << std::setprecision(7) << std::fixed << std::noshowpos
         << tabSettings().plane2VoltageOffset();
      cmd.emplace_back(ss.str());
      doExec = true;
    }
  }

  /* FIVR Voltage Offset Plane 3 */  
  if (tabSettings().plane3VoltageOffsetEnabled()) {
    if (!EQUAL(tabSettings().plane3VoltageOffset(),
        tabValues().plane3VoltageOffset())) {
      cmd.emplace_back("--plane3");
      std::stringstream ss;
      ss << std::setprecision(7) << std::fixed << std::noshowpos
         << tabSettings().plane3VoltageOffset();
      cmd.emplace_back(ss.str());
      doExec = true;
    }
  }

  /* FIVR Voltage Offset Plane 4 */  
  if (tabSettings().plane4VoltageOffsetEnabled()) {
    if (!EQUAL(tabSettings().plane4VoltageOffset(),
        tabValues().plane4VoltageOffset())) {
      cmd.emplace_back("--plane4");
      std::stringstream ss;
      ss << std::setprecision(7) << std::fixed << std::noshowpos
         << tabSettings().plane4VoltageOffset();
      cmd.emplace_back(ss.str());
      doExec = true;
    }
  }

  /* FIVR Voltage Offset Plane 5 */  
  if (tabSettings().plane5VoltageOffsetEnabled()) {
    if (!EQUAL(tabSettings().plane5VoltageOffset(),
        tabValues().plane5VoltageOffset())) {
      cmd.emplace_back("--plane5");
      std::stringstream ss;
      ss << std::setprecision(7) << std::fixed << std::noshowpos
         << tabSettings().plane5VoltageOffset();
      cmd.emplace_back(ss.str());
      doExec = true;
    }
  }

  /* Run the core-adjust shell script. */
  if (doExec) {
    if (shell_.run(std::move(cmd))) {
      DBGMSG("VoltageOffsets::apply FAILED!")
      return false;
    }
  }

  DBGMSG("VoltageOffsets::apply success")
  return true;
}

bool VoltageOffsets::compare()
{
  /* Store the current UI values to a temporary object */
  Settings data(tabSettings());
  store(data);

  /* Compare the relevant values in the temporary object against the
   * global TabMemberSettings instance and return false if they differ. */

  if (data.plane0VoltageOffsetEnabled() != tabSettings().plane0VoltageOffsetEnabled() ||
      data.plane1VoltageOffsetEnabled() != tabSettings().plane1VoltageOffsetEnabled() ||
      data.plane2VoltageOffsetEnabled() != tabSettings().plane2VoltageOffsetEnabled() ||
      data.plane3VoltageOffsetEnabled() != tabSettings().plane3VoltageOffsetEnabled() ||
      data.plane4VoltageOffsetEnabled() != tabSettings().plane4VoltageOffsetEnabled() ||
      data.plane5VoltageOffsetEnabled() != tabSettings().plane5VoltageOffsetEnabled() ||
      !EQUAL(data.plane0VoltageOffset(), tabSettings().plane0VoltageOffset()) ||
      !EQUAL(data.plane1VoltageOffset(), tabSettings().plane1VoltageOffset()) ||
      !EQUAL(data.plane2VoltageOffset(), tabSettings().plane2VoltageOffset()) ||
      !EQUAL(data.plane3VoltageOffset(), tabSettings().plane3VoltageOffset()) ||
      !EQUAL(data.plane4VoltageOffset(), tabSettings().plane4VoltageOffset()) ||
      !EQUAL(data.plane5VoltageOffset(), tabSettings().plane5VoltageOffset())) {
    DBGMSG("VoltageOffsets::compare(): Voltage Offsets do not match")
    return false;
  }

  DBGMSG("VoltageOffsets::compare(): Settings match!")
  return true;
}

/* SLOTS */

void VoltageOffsets::voltageChanged() {
  emit valueChanged(this);
}


void VoltageOffsets::forceVoltageChecked(int state) {
  /* Display a cancel/continue dialog when this checkbox is checked.
   * The call to isVisible() prevents the dialog when this VoltageOffset
   * instance is invisible (ie. during construction of the instance when the
   * VoltageOffset::load() function triggers this event before the widget is
   * shown. */
  if (state && isVisible()) {
    auto reply = QMessageBox::warning(
        this, tr("Allow FIVR overvoltage?"),
        tr("Operating the CPU at a voltage above its disgn limits is harmfull to the CPU. "
        "It may shorten the lifespan of the CPU or even destroy it in seconds!<br/><br/>"
        "Are you absolutely sure that you want to overvolt the CPU?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply == QMessageBox::No) {
      force_voltage_->blockSignals(true);
      force_voltage_->setCheckState(Qt::Unchecked);
      force_voltage_->blockSignals(false);
      return;
    }
  }
  /* Adjust the available range of all sliders */
  plane0_->setRange(VoltageOffsetSlider::KeepCurrent, state);
  plane1_->setRange(VoltageOffsetSlider::KeepCurrent, state);
  plane2_->setRange(VoltageOffsetSlider::KeepCurrent, state);
  plane3_->setRange(VoltageOffsetSlider::KeepCurrent, state);
  plane4_->setRange(VoltageOffsetSlider::KeepCurrent, state);
  plane5_->setRange(VoltageOffsetSlider::KeepCurrent, state);
}


void VoltageOffsets::plane0RangeSelected(enum VoltageOffsetSlider::Range range) {
  plane0_->setRange(range, force_voltage_->isChecked());
}


void VoltageOffsets::plane1RangeSelected(enum VoltageOffsetSlider::Range range) {
  plane1_->setRange(range, force_voltage_->isChecked());
}


void VoltageOffsets::plane2RangeSelected(enum VoltageOffsetSlider::Range range) {
  plane2_->setRange(range, force_voltage_->isChecked());
}


void VoltageOffsets::plane3RangeSelected(enum VoltageOffsetSlider::Range range) {
  plane3_->setRange(range, force_voltage_->isChecked());
}


void VoltageOffsets::plane4RangeSelected(enum VoltageOffsetSlider::Range range) {
  plane4_->setRange(range, force_voltage_->isChecked());
}


void VoltageOffsets::plane5RangeSelected(enum VoltageOffsetSlider::Range range) {
  plane5_->setRange(range, force_voltage_->isChecked());
}


void VoltageOffsets::Settings::load(
    QSettings& qs, const TabMemberValues& cv) {

  TabMemberSettings& d = tabMemberSettings();

  /* select the INI section for this processor */
  std::stringstream ss;
  ss << TabSettings::INI_GRP_PROCESSOR << d.processorNumber().value;
  qs.beginGroup(ss.str().c_str());

  /* Load the settings or set a default value: */

  d.allowPositiveValues(
      qs.value(INI_FIVR_FORCE, false).toBool());

  d.plane0VoltageOffset(
      qs.value(INI_FIVR_PLANE0_VOFFSET, cv.plane0VoltageOffset()).toDouble());

  d.plane1VoltageOffset(
      qs.value(INI_FIVR_PLANE1_VOFFSET, cv.plane1VoltageOffset()).toDouble());

  d.plane2VoltageOffset(
      qs.value(INI_FIVR_PLANE2_VOFFSET, cv.plane2VoltageOffset()).toDouble());

  d.plane3VoltageOffset(
      qs.value(INI_FIVR_PLANE3_VOFFSET, cv.plane3VoltageOffset()).toDouble());

  d.plane4VoltageOffset(
      qs.value(INI_FIVR_PLANE4_VOFFSET, cv.plane4VoltageOffset()).toDouble());

  d.plane5VoltageOffset(
      qs.value(INI_FIVR_PLANE5_VOFFSET, cv.plane5VoltageOffset()).toDouble());

  d.plane0VoltageOffsetEnabled(
      qs.value(INI_FIVR_PLANE0_VOFFSET_ENABLE, false).toBool());

  d.plane1VoltageOffsetEnabled(
      qs.value(INI_FIVR_PLANE1_VOFFSET_ENABLE, false).toBool());

  d.plane2VoltageOffsetEnabled(
      qs.value(INI_FIVR_PLANE2_VOFFSET_ENABLE, false).toBool());

  d.plane3VoltageOffsetEnabled(
      qs.value(INI_FIVR_PLANE3_VOFFSET_ENABLE, false).toBool());

  d.plane4VoltageOffsetEnabled(
      qs.value(INI_FIVR_PLANE4_VOFFSET_ENABLE, false).toBool());

  d.plane5VoltageOffsetEnabled(
      qs.value(INI_FIVR_PLANE5_VOFFSET_ENABLE, false).toBool());

  /* ensure voltage offsets are within range */
  if (!d.allowPositiveValues()) {
    if (d.plane0VoltageOffset() > 0.0) d.plane0VoltageOffset(0.);
    if (d.plane1VoltageOffset() > 0.0) d.plane1VoltageOffset(0.);
    if (d.plane2VoltageOffset() > 0.0) d.plane2VoltageOffset(0.);
    if (d.plane3VoltageOffset() > 0.0) d.plane3VoltageOffset(0.);
    if (d.plane4VoltageOffset() > 0.0) d.plane4VoltageOffset(0.);
    if (d.plane5VoltageOffset() > 0.0) d.plane5VoltageOffset(0.);
    if (d.plane0VoltageOffset() < VOLTAGE_OFFSET_MIN) d.plane0VoltageOffset(0.);
    if (d.plane1VoltageOffset() < VOLTAGE_OFFSET_MIN) d.plane1VoltageOffset(0.);
    if (d.plane2VoltageOffset() < VOLTAGE_OFFSET_MIN) d.plane2VoltageOffset(0.);
    if (d.plane3VoltageOffset() < VOLTAGE_OFFSET_MIN) d.plane3VoltageOffset(0.);
    if (d.plane4VoltageOffset() < VOLTAGE_OFFSET_MIN) d.plane4VoltageOffset(0.);
    if (d.plane5VoltageOffset() < VOLTAGE_OFFSET_MIN) d.plane5VoltageOffset(0.);
  }
  else {
    if (d.plane0VoltageOffset() < VOLTAGE_OFFSET_MIN || d.plane0VoltageOffset() > VOLTAGE_OFFSET_MAX) d.plane0VoltageOffset(0.);
    if (d.plane1VoltageOffset() < VOLTAGE_OFFSET_MIN || d.plane1VoltageOffset() > VOLTAGE_OFFSET_MAX) d.plane1VoltageOffset(0.);
    if (d.plane2VoltageOffset() < VOLTAGE_OFFSET_MIN || d.plane2VoltageOffset() > VOLTAGE_OFFSET_MAX) d.plane2VoltageOffset(0.);
    if (d.plane3VoltageOffset() < VOLTAGE_OFFSET_MIN || d.plane3VoltageOffset() > VOLTAGE_OFFSET_MAX) d.plane3VoltageOffset(0.);
    if (d.plane4VoltageOffset() < VOLTAGE_OFFSET_MIN || d.plane4VoltageOffset() > VOLTAGE_OFFSET_MAX) d.plane4VoltageOffset(0.);
    if (d.plane5VoltageOffset() < VOLTAGE_OFFSET_MIN || d.plane5VoltageOffset() > VOLTAGE_OFFSET_MAX) d.plane5VoltageOffset(0.);
    /* Disable allowPositiveValues if none of the voltages are above 0. */
    if (d.plane0VoltageOffset() <= 0. && d.plane1VoltageOffset() <= 0. &&
        d.plane2VoltageOffset() <= 0. && d.plane3VoltageOffset() <= 0. &&
        d.plane4VoltageOffset() <= 0. && d.plane5VoltageOffset() <= 0.) {
      d.allowPositiveValues(false);
    }
  }

  qs.endGroup();

  DBGMSG("VoltageOffsets::Settings::allowPositiveValues()             <--" << allowPositiveValues())
  DBGMSG("VoltageOffsets::Settings::plane0VoltageOffset()             <--" << plane0VoltageOffset())
  DBGMSG("VoltageOffsets::Settings::plane1VoltageOffset()             <--" << plane1VoltageOffset())
  DBGMSG("VoltageOffsets::Settings::plane2VoltageOffset()             <--" << plane2VoltageOffset())
  DBGMSG("VoltageOffsets::Settings::plane3VoltageOffset()             <--" << plane3VoltageOffset())
  DBGMSG("VoltageOffsets::Settings::plane4VoltageOffset()             <--" << plane4VoltageOffset())
  DBGMSG("VoltageOffsets::Settings::plane5VoltageOffset()             <--" << plane5VoltageOffset())
}

void VoltageOffsets::Settings::save(QSettings& qs) {
  TabMemberSettings& d = tabMemberSettings();

  /* select the INI section for this processor */
  std::stringstream ss;
  ss << TabSettings::INI_GRP_PROCESSOR << d.processorNumber().value;
  qs.beginGroup(ss.str().c_str());

  /* Save the settings: */

  /* Reset allowPositiveValues if none of the voltages are above 0. */
  if (d.allowPositiveValues()) {
    if (d.plane0VoltageOffset() <= 0.0 &&
        d.plane1VoltageOffset() <= 0.0 &&
        d.plane2VoltageOffset() <= 0.0 &&
        d.plane3VoltageOffset() <= 0.0 &&
        d.plane4VoltageOffset() <= 0.0 &&
        d.plane5VoltageOffset() <= 0.0) {
      d.allowPositiveValues(false);
    }
  }

  qs.setValue(INI_FIVR_FORCE, d.allowPositiveValues());

  qs.setValue(INI_FIVR_PLANE0_VOFFSET,
      QVariant::fromValue<double>(d.plane0VoltageOffset()));

  qs.setValue(INI_FIVR_PLANE1_VOFFSET,
      QVariant::fromValue<double>(d.plane1VoltageOffset()));

  qs.setValue(INI_FIVR_PLANE2_VOFFSET,
      QVariant::fromValue<double>(d.plane2VoltageOffset()));

  qs.setValue(INI_FIVR_PLANE3_VOFFSET,
      QVariant::fromValue<double>(d.plane3VoltageOffset()));

  qs.setValue(INI_FIVR_PLANE4_VOFFSET,
      QVariant::fromValue<double>(d.plane4VoltageOffset()));

  qs.setValue(INI_FIVR_PLANE5_VOFFSET,
      QVariant::fromValue<double>(d.plane5VoltageOffset()));

  qs.setValue(INI_FIVR_PLANE0_VOFFSET_ENABLE,
      QVariant::fromValue<bool>(d.plane0VoltageOffsetEnabled()));

  qs.setValue(INI_FIVR_PLANE1_VOFFSET_ENABLE,
      QVariant::fromValue<bool>(d.plane1VoltageOffsetEnabled()));

  qs.setValue(INI_FIVR_PLANE2_VOFFSET_ENABLE,
      QVariant::fromValue<bool>(d.plane2VoltageOffsetEnabled()));

  qs.setValue(INI_FIVR_PLANE3_VOFFSET_ENABLE,
      QVariant::fromValue<bool>(d.plane3VoltageOffsetEnabled()));

  qs.setValue(INI_FIVR_PLANE4_VOFFSET_ENABLE,
      QVariant::fromValue<bool>(d.plane4VoltageOffsetEnabled()));

  qs.setValue(INI_FIVR_PLANE5_VOFFSET_ENABLE,
      QVariant::fromValue<bool>(d.plane5VoltageOffsetEnabled()));

  qs.endGroup();

  DBGMSG("VoltageOffsets::Settings::allowPositiveValues()             -->" << allowPositiveValues())
  DBGMSG("VoltageOffsets::Settings::plane0VoltageOffset()             -->" << plane0VoltageOffset())
  DBGMSG("VoltageOffsets::Settings::plane1VoltageOffset()             -->" << plane1VoltageOffset())
  DBGMSG("VoltageOffsets::Settings::plane2VoltageOffset()             -->" << plane2VoltageOffset())
  DBGMSG("VoltageOffsets::Settings::plane3VoltageOffset()             -->" << plane3VoltageOffset())
  DBGMSG("VoltageOffsets::Settings::plane4VoltageOffset()             -->" << plane4VoltageOffset())
  DBGMSG("VoltageOffsets::Settings::plane5VoltageOffset()             -->" << plane5VoltageOffset())
}

