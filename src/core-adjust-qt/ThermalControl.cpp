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

/*
 * This tab is a bit of a kludge, but it needs to re-writen for a
 * next release anyway (if I figure out how to enable TM2 on my machine
 * that supports TM2 but it has been disabled by the BIOS somehow).
 */

/**
  * @file src/core-adjust-qt/ThermalControl.hpp
  * @brief A TabMemberWidget for adjusting MSR_THERM2_CTL and MSR_TEMPERATURE_TARGET.
  *
  * @file src/core-adjust-qt/ThermalControl.cpp
  * @brief A TabMemberWidget for adjusting MSR_THERM2_CTL and MSR_TEMPERATURE_TARGET (implementation).
  *
  * @class ThermalControl
  * @brief A TabMemberWidget for adjusting MSR_THERM2_CTL and MSR_TEMPERATURE_TARGET.
  *
  * Modifies MSRs:
  *
  *   IA32_MISC_ENABLE (reads only)
  *     - TM2_ENABLE
  *
  *   MSR_THERM2_CTL
  *     - TM_SELECT
  *
  *   MSR_TEMPERATURE_TARGET
  *     - Temperature_Target
  *
  * @fn explicit ThermalControl::ThermalControl(const SingleCpuInfo& cpuInfo, const SingleCpuId& cpuId, TabMemberValues& tabValues, TabMemberSettings& tabSettings, QWidget* parent)
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
  *
  * @class ThermalControl::Values
  * @brief Values to read from the processor for class ThermalControl.
  *
  * @fn size_t ThermalControl::Values::targetTemperature() const
  * @brief Get the CPU Target Temperature.
  * @returns The Target Temperature.
  *
  * @fn size_t ThermalControl::Values::targetTemperatureOffset() const
  * @brief Get the CPU Target Temperature Offset.
  * @returns The Target Temperature Offset.
  *
  * @fn void ThermalControl::Values::targetTemperature(size_t t)
  * @brief Set the CPU Target Temperature.
  * @param t The Target Temperature.
  *
  * @fn void ThermalControl::Values::targetTemperatureOffset(size_t t)
  * @brief Set the CPU Target Temperature Offset.
  * @param t The Target Temperature Offset.
  *
  * @fn bool ThermalControl::Values::tmSelect() const
  * @brief Get the mode of the automatic thermal monitor (TM1/TM2).
  * @returns State of TM_SELECT.
  *
  * @fn void ThermalControl::Values::tmSelect(bool state)
  * @brief Select the mode of the automatic thermal monitor (TM1/TM2).
  * @param state The new state of TM_SELECT.
  *
  * @fn bool ThermalControl::Values::tm2Enable() const
  * @brief Get the mode of the automatic thermal monitor (TM1/TM2).
  * @returns State of IA32_MISC_ENABLE.TM_SELECT[13].
  *
  * @fn void ThermalControl::Values::tm2Enable(bool state)
  * @brief Select the mode of the automatic thermal monitor (TM1/TM2).
  * @param state The new state of TM_SELECT.
  *
  *
  *
  * @class ThermalControl::Settings
  * @brief Settings for class ThermalControl.
  *
  * @fn virtual void ThermalControl::Settings::load(QSettings&, const SingleCpuInfo&, const TabMemberValues&)
  * @brief Load the settings for this TabMemberWidget from the INI file.
  *
  * @fn virtual void ThermalControl::Settings::save(QSettings&)
  * @brief Save the settings for this TabMemberWidget to the INI file.
  *
  * @fn size_t ThermalControl::Settings::targetTemperatureOffsetBattery() const
  * @brief Set The target temperature offset while on battery power.
  *
  * @fn void ThermalControl::Settings::targetTemperatureOffsetBattery(size_t offset)
  * @param offset The new Target Temperature Offset for 'battery' mode.
  *
  * @fn bool ThermalControl::Settings::targetTemperatureEnabled() const
  * @brief Enable adjusting the target temperature? (getter)
  *
  * @fn bool ThermalControl::Settings::targetTemperatureBatteryEnabled() const
  * @brief Enable a separate battery target temperature? (getter)
  *
  * @fn void ThermalControl::Settings::targetTemperatureEnabled(bool state)
  * @brief Enable adjusting the target temperature? (setter)
  *
  * @fn void ThermalControl::Settings::targetTemperatureBatteryEnabled(bool state)
  * @brief Enable a separate battery target temperature? (setter)
  *
  * @fn bool ThermalControl::Settings::tmSelectEnabled() const
  * @brief Enable adjusting the mode of the automatic thermal monitor? (getter)
  *
  * @fn void ThermalControl::Settings::tmSelectEnabled(bool state)
  * @brief Enable adjusting the mode of the automatic thermal monitor? (setter)
  *
  * @fn bool ThermalControl::Settings::tm2EnableEnabled() const
  * @brief Enable adjusting the mode of the automatic thermal monitor? (getter)
  *
  * @fn void ThermalControl::Settings::tm2EnableEnabled(bool state)
  * @brief Enable adjusting the mode of the automatic thermal monitor? (setter)
  */
// STL
#include <iomanip>
// Qt
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
// App
#include "CpuId.hpp"
#include "CpuInfo.hpp"
#include "Msr.hpp"
#include "ThermalControl.hpp"
#include "TabMember.hpp"
#include "Shell.hpp"
#include "config.h"

ThermalControl::ThermalControl(
  const SingleCpuInfo& info,
  const SingleCpuId& id,
  TabMemberValues& values,
  TabMemberSettings& data,
  QWidget *parent)
  : TabMemberTemplate(info, id, values, data, parent),
    shell_(this, "Adjust Thermal settings", ShellCommand::EnableStd | ShellCommand::DisableAbortButton) {

  /* box 0 */

  auto* b0l1 = new QLabel("Thermal Monitor #1:");
  tm1_supported_ = new QLabel("enabled/disables/not supported");
  auto* b0h1 = new QHBoxLayout();
  b0l1->setToolTip(tr(
    "<p>TM1:<br/>"
    "Controls the processor’s temperature by modulating "
    "the duty cycle of the processor clock.</p>"
    "<p>TM1 is supported if CPUID.1:EDX.TM[bit 29] = 1 and can be "
    "enabled/disabled by the thermal-monitor enable flag (bit 3) "
    "in IA32_MISC_ENABLE.</p>"
  ));
  b0h1->addWidget(b0l1);
  b0h1->addWidget(tm1_supported_);
  b0h1->addStretch(1);

  auto* b0l2 = new QLabel("Thermal Monitor #2:");
  tm2_supported_ = new QLabel("enabled/disables/not supported");
  auto* b0h2 = new QHBoxLayout();
  b0l2->setToolTip(tr(
    "<p>TM2:<br/>"
    "Controls the core temperature of the processor by reducing the operating "
    "frequency and voltage of the processor and offers a higher performance "
    "level for a given level of power reduction than TM1."
    "TM2 is triggered by the same temperature sensor as TM1.</p>"
    "<p>Support for TM2 is indicated by CPUID.1:ECX.TM2[bit 8] = 1.</p>"
    "<p>TM2 is enabled by setting bit 13 of IA32_MISC_ENABLE register to 1.</p>"
  ));
  b0h2->addWidget(b0l2);
  b0h2->addWidget(tm2_supported_);
  b0h2->addStretch(1);

  auto* b0l3 = new QLabel("Programmable Tj offset:");
  tj_offset_ = new QLabel("supported/not supported");
  auto* b0h3 = new QHBoxLayout();
  b0l3->setToolTip(tr(
    "<p>If MSR_PLATFORM_INFO bit 30 is set then MSR_TEMPERATURE_TARGET bit 27:24 "
    "is valid and writeable to specifiy a temperature offset. "
    "Supported on Haswell and later microarchitectures (Atom: Goldmont and later).</p>"
  ));
  b0h3->addWidget(b0l3);
  b0h3->addWidget(tj_offset_);
  b0h3->addStretch(1);

  auto* b0l4 = new QLabel("Effective Temperature Target:");
  target_temp_ = new QLabel("value");
  auto* b0h4 = new QHBoxLayout();
  b0h4->addWidget(b0l4);
  b0h4->addWidget(target_temp_);
  b0h4->addStretch(1);

  auto* box0layout = new QVBoxLayout();
  box0layout->addLayout(b0h1);
  box0layout->addLayout(b0h2);
  box0layout->addLayout(b0h3);
  box0layout->addLayout(b0h4);

  auto* box0 = new QGroupBox(tr("Automatic Thermal Control Method(s)"));
  box0->setLayout(box0layout);
  box0->setFlat(true);

  /* box 1 */

  tm1_select_ = new QRadioButton(tr("Use Thermal Monitor #1 (stop-clock duty cycle modulation)."));
  tm2_select_ = new QRadioButton(tr("Use Thermal Monitor #2 (thermally-initiated frequency transitions)."));
  tm_default_ = new QRadioButton(tr("Do not adjust."));
  auto* box1layout = new QVBoxLayout();
  box1layout->addWidget(tm1_select_);
  box1layout->addWidget(tm2_select_);
  box1layout->addWidget(tm_default_);

  auto* box1 = new QGroupBox(tr("Adjust Automatic Thermal Control Method"));
  box1->setLayout(box1layout);
  box1->setFlat(true);

  /* box 2 */

  auto* box2layout = new QVBoxLayout();
  auto *hbox = new QHBoxLayout();
  auto *vbox_labels = new QVBoxLayout();
  auto *vbox_sliders = new QVBoxLayout();
  auto *vbox_values = new QVBoxLayout();
  ac_label_ = new QLabel(tr("AC Powered: "));
  batt_label_ = new QLabel(tr("DC Powered: "));
  ac_slider_ = new SaferSlider(Qt::Horizontal);
  batt_slider_ = new SaferSlider(Qt::Horizontal);
  ac_value_ = new QLabel(tr("n.a."));
  batt_value_ = new QLabel(tr("n.a."));
  batt_enable_ = new QCheckBox(tr("Use a separate temperature target while on DC power?"));
  temp_enable_ = new QCheckBox(tr("Adjust temperature target?"));

  auto range = SingleCpuInfo::TargetTempRange(cpuInfo().MicroArch);

  ac_slider_->setPageStep(1);
  ac_slider_->setRange(
      static_cast<int>(tabValues().targetTemperature() - range),
      static_cast<int>(tabValues().targetTemperature()));
  ac_value_->setMinimumWidth(45);
  ac_value_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

  batt_slider_->setPageStep(1);
  batt_slider_->setRange(
      static_cast<int>(tabValues().targetTemperature() - range),
      static_cast<int>(tabValues().targetTemperature()));
  batt_value_->setMinimumWidth(45);
  batt_value_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

  vbox_labels->addWidget(ac_label_);
  vbox_labels->addWidget(batt_label_);

  vbox_sliders->addWidget(ac_slider_, 1);
  vbox_sliders->addWidget(batt_slider_, 1);

  vbox_values->addWidget(ac_value_);
  vbox_values->addWidget(batt_value_);

  hbox->addLayout(vbox_labels);
  hbox->addLayout(vbox_sliders, 1);
  hbox->addLayout(vbox_values);

  box2layout->addLayout(hbox);
  box2layout->addSpacing(8);
  box2layout->addWidget(batt_enable_);
  box2layout->addWidget(temp_enable_);
  box2layout->addStretch(1);

  box2_ = new QGroupBox(tr("Adjust Effective Temperature Target"));
  box2_->setLayout(box2layout);
  box2_->setFlat(true);

  /* Add the inner widgets to the layout of the scroll widget */

  scroll_layout_->addStretch(1);
  scroll_layout_->addWidget(box0);
  scroll_layout_->addStretch(1);
  scroll_layout_->addWidget(box1);
  scroll_layout_->addStretch(1);
  scroll_layout_->addWidget(box2_);
  scroll_layout_->addStretch(1);

  /* Disable box2_ if a programmable Tj offset is not supported */

  if ((cpuInfo().isHaswellOrLater() ||
      cpuInfo().isGoldmontOrLater() ||
      cpuInfo().isXeonPhi())) {
    MSR_PLATFORM_INFO msr_platform_info(cpuInfo().firstLogicalCpu());
    msr_platform_info.read();
    box2_->setEnabled(msr_platform_info.Programmable_TJ_OFFSET != 0);
    box2_->setToolTip(tr("Adjust the Programmable Tj offset."));
    tj_offset_->setText(tr("<font color='green'>Supported</font>"));
  }
  else {
    box2_->setEnabled(false);
    box2_->setToolTip(tr("Programmable Tj offset is not supported by this processor."));
    tj_offset_->setText(tr("<font color='red'>Not Supported</font>"));
  }

  timed(true);

  /* Signals */

  connect(tm1_select_, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
  connect(tm2_select_, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
  connect(tm_default_, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));

  connect(temp_enable_, SIGNAL(stateChanged(int)),
      this, SLOT(enableCheckedSlot(int)));

  connect(batt_enable_, SIGNAL(stateChanged(int)),
      this, SLOT(battEnableCheckedSlot(int)));

  connect(ac_slider_, SIGNAL(valueChanged(int)),
      this, SLOT(targetTemperatureChangedSlot(int)));

  connect(batt_slider_, SIGNAL(valueChanged(int)),
      this, SLOT(battTargetTemperatureChangedSlot(int)));

  connect(&shell_, SIGNAL(finished()), &shell_, SLOT(accept()));
}

bool ThermalControl::read(std::ostringstream& ss) {
  bool retv = true;

  /* Set defaults */
  tabValues().targetTemperature(100);
  tabValues().targetTemperatureOffset(0);
  tabValues().tmSelect(false);
  tabValues().tm2Enable(false);

  /*
   * Read the values for this tab from the processor
   */

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
    tabValues().targetTemperature(static_cast<size_t>(strtol(&output[1].data()[31], nullptr, 10)));
    tabValues().targetTemperatureOffset(
        static_cast<size_t>(tabValues().targetTemperature() -
            static_cast<size_t>(strtol(&output[3].data()[31], nullptr, 10))));
  }
  else {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, unexpected output from command (ThermalControl):</b></p>"
      "<nobr>core-adjust core-adjust --read --verbose</nobr>");
    setEnabled(false);
  }

  MSR_THERM2_CTL msr_therm2_ctl(cpuInfo().firstLogicalCpu());
  if (msr_therm2_ctl.read()) {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, Could not read MSR_THERM2_CTL!</b></p>");
    setEnabled(false);
  }
  tabValues().tmSelect(msr_therm2_ctl.TM_SELECT != 0);

  IA32_MISC_ENABLE ia32_misc_enable(cpuInfo().firstLogicalCpu());
  if (ia32_misc_enable.read()) {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, Could not read IA32_MISC_ENABLE!</b></p>");
    setEnabled(false);
  }
  tabValues().tm2Enable(ia32_misc_enable.TM2_ENABLE != 0);

  /* Exit if an error occurred when reading the MSRs */
  if (!isEnabled()) return true;

  /*
   * Compare the newly read values against the desired values
   */

  if (tabSettings().targetTemperatureEnabled()) {
    if (tabSettings().targetTemperature() != tabValues().targetTemperature()) {
      ss << "<li><nobr>The '<b>Target Temperature</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().targetTemperature() << "</b> instead of <b>"
         << tabSettings().targetTemperature() << "</b>.</nobr></li>";
      retv = false;
    }
    if (tabSettings().targetTemperatureBatteryEnabled() && !on_ac_power_) {
      if (tabSettings().targetTemperatureOffsetBattery() != tabValues().targetTemperatureOffset()) {
        ss << "<li><nobr>The '<b>Target Temperature Offset (DC)</b>' for processor "
           << cpuInfo().physicalId().value << " is <b>"
           << tabValues().targetTemperatureOffset() << "</b> instead of <b>"
           << tabSettings().targetTemperatureOffsetBattery() << "</b>.</nobr></li>";
        retv = false;
      }
    }
    else {
      if (tabSettings().targetTemperatureOffset() != tabValues().targetTemperatureOffset()) {
        ss << "<li><nobr>The '<b>Target Temperature Offset (AC)</b>' for processor "
           << cpuInfo().physicalId().value << " is <b>"
           << tabValues().targetTemperatureOffset() << "</b> instead of <b>"
           << tabSettings().targetTemperatureOffset() << "</b>.</nobr></li>";
        retv = false;
      }
    }
  }

  if (tabSettings().tmSelectEnabled()) {
    if (tabSettings().tmSelect() != tabValues().tmSelect()) {
      ss << "<li><nobr>'<b>TM_SELECT</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tmSelect() << "</b> instead of <b>"
         << tabSettings().tmSelect() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().tm2EnableEnabled()) {
    if (tabSettings().tm2Enable() != tabValues().tm2Enable()) {
      ss << "<li><nobr>'<b>TM2_ENABLE</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tm2Enable() << "</b> instead of <b>"
         << tabSettings().tm2Enable() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  /*
   * Return false if the newly read values differ from the desired values.
   */
  return retv;
}

void ThermalControl::load() {
  /* 'load' the data from the Settings instance */

  tm1_select_->blockSignals(true);
  tm2_select_->blockSignals(true);
  tm_default_->blockSignals(true);
  batt_enable_->blockSignals(true);
  temp_enable_->blockSignals(true);
  ac_slider_->blockSignals(true);
  batt_slider_->blockSignals(true);

  if (tabSettings().tmSelectEnabled()) {
    /* TM_SELECT is disabled */
    tm_default_->setChecked(true);
  }
  else {
    if (tabSettings().tmSelect() == 0) {
      /* TM1 is selected */
      tm1_select_->setChecked(true);
    }
    else {
      /* TM2 is selected */
      tm2_select_->setChecked(true);
    }
  }

  /* set slider values and visibility states */
  ac_slider_->setValue(
      static_cast<int>(tabSettings().targetTemperature() -
          tabSettings().targetTemperatureOffset()));
  batt_slider_->setValue(
      static_cast<int>(tabSettings().targetTemperature() -
          tabSettings().targetTemperatureOffsetBattery()));

  /* checked state differs from the new value? */
  if (batt_enable_->isChecked() !=
      (tabSettings().targetTemperatureEnabled() & tabSettings().targetTemperatureBatteryEnabled())) {
    /* yes, a valueChanged signal will be emitted */
    batt_enable_->setChecked(
        tabSettings().targetTemperatureEnabled() & tabSettings().targetTemperatureBatteryEnabled());
  }
  else {
    batt_label_->setEnabled(false);
    batt_slider_->setEnabled(false);
    batt_value_->setEnabled(false);
  }

  /* checked state differs from the new value? */
  if (temp_enable_->isChecked() != tabSettings().targetTemperatureEnabled()) {
    /* yes, a valueChanged signal will be emitted */
    temp_enable_->setChecked(tabSettings().targetTemperatureEnabled());
  }
  else {
    ac_label_->setEnabled(false);
    ac_slider_->setEnabled(false);
    ac_value_->setEnabled(false);
  }

  if (temp_enable_->isChecked()) {
    batt_enable_->setEnabled(true);
  }
  else {
    batt_enable_->setEnabled(false);
  }

  tm1_select_->blockSignals(false);
  tm2_select_->blockSignals(false);
  tm_default_->blockSignals(false);
  batt_enable_->blockSignals(false);
  temp_enable_->blockSignals(false);
  ac_slider_->blockSignals(false);
  batt_slider_->blockSignals(false);
}

void ThermalControl::store() {
  /* Store current UI values to the global TabMemberSettings instance */
  store(tabSettings());
}

void ThermalControl::store(Settings& data) {
  /* Store current UI values to the specified TabMemberSettings instance */

  if (tm_default_->isChecked()) {
    /* Do not adjust (now and at boot) */
    data.tmSelectEnabled(false);
    data.tmSelect(tabValues().tmSelect());
  }
  else {
    data.tmSelectEnabled(true);
    if (tm1_select_->isChecked()) {
      /* TM1 is selected */
      data.tmSelect(0);
    }
    else {
      /* TM2 is selected */
      data.tmSelect(1);
    }
  }

  data.targetTemperatureEnabled(temp_enable_->isChecked());
  if (temp_enable_->isChecked()) {
    data.targetTemperatureOffset(
        data.targetTemperature() - static_cast<unsigned long>(ac_slider_->value()));
    data.targetTemperatureBatteryEnabled(batt_enable_->isChecked());
    if (batt_enable_->isChecked()) {
      data.targetTemperatureOffsetBattery(
          data.targetTemperature() -
              static_cast<unsigned long>(batt_slider_->value()));
    }
  }
}

void ThermalControl::refresh() {

  /* Exit if we are disabled */
  if (!isEnabled()) return;

  // box0 + box1

  // read IA32_MISC_ENABLE
  IA32_MISC_ENABLE ia32_misc_enable(cpuInfo().firstLogicalCpu());
  ia32_misc_enable.read();

  // TM1 supported?
  if ((cpuId().EAX_01H.EDX & (1 << 29)) != 0) {
    // TM1 supported, is it enabled?
    if (ia32_misc_enable.Automatic_Thermal_Control_Circuit_Enable != 0) {
      // TM1 is enabled
      tm1_supported_->setText(tr("<font color='Green'>Enabled</font>"));
      tm1_select_->setEnabled(true);
      tm1_select_->setToolTip(tr("Sets MSR_THERM2_CTL.TM_SELECT[bit 16] to 0, selecting Thermal Monitor #1."));
    }
    else {
      // TM1 is disabled
      tm1_supported_->setText(tr("<font color='red'>Supported but disabled by BIOS</font>"));
      tm1_select_->setEnabled(false);
      tm1_select_->setToolTip(tr("TM1 has NOT been enabled by the BIOS, cannot select Thermal Monitor #1."));
    }
  }
  else {
    // TM1 not supported
    tm1_supported_->setText(tr("<font color='red'>Not Supported</font>"));
    tm1_select_->setEnabled(false);
    tm1_select_->setToolTip(tr("TM1 is not supported by this processor."));
  }

  // TM2 supported?
  if ((cpuId().EAX_01H.ECX & (1 << 8)) != 0) {
    // TM2 supported, is it enabled?
    if (ia32_misc_enable.TM2_ENABLE != 0) {
      // TM2 is enabled
      tm2_supported_->setText(tr("<font color='Green'>Enabled</font>"));
      tm2_select_->setEnabled(true);
      tm2_select_->setToolTip(tr("Sets MSR_THERM2_CTL.TM_SELECT[bit 16] to 1, selecting Thermal Monitor #2."));
    }
    else {
      // TM2 is disabled
      tm2_supported_->setText(tr("<font color='red'>Supported but disabled by BIOS</font>"));
      tm2_select_->setEnabled(false);
      tm2_select_->setToolTip(tr("TM2 has NOT been enabled by the BIOS, cannot select Thermal Monitor #2."));
    }
  }
  else {
    // TM2 not supported
    tm2_supported_->setText(tr("<font color='red'>Not Supported</font>"));
    tm2_select_->setEnabled(false);
    tm2_select_->setToolTip(tr("TM2 is not supported by this processor."));
  }

  // Programmable Tj supported?
  if (box2_->isEnabled()) {
    // Programmable Tj is supported
    target_temp_->setText(std::move(QString(
        "%1 °C (on AC power)").arg(
            tabValues().targetTemperature() -
                tabValues().targetTemperatureOffset())));
  }
  else {
    // Programmable Tj is not supported
    target_temp_->setText(
        std::move(QString("%1 °C").arg(tabValues().targetTemperature())));
  }
  
  // box1
  tm1_select_->blockSignals(true);
  tm2_select_->blockSignals(true);
  tm_default_->blockSignals(true);
  if (tabSettings().tmSelectEnabled()) {
    if (tabValues().tmSelect()) {
      tm2_select_->setChecked(true);
    }
    else {
      tm1_select_->setChecked(true);
    }
  }
  else {
    tm_default_->setChecked(true);
  }
  tm1_select_->blockSignals(false);
  tm2_select_->blockSignals(false);
  tm_default_->blockSignals(false);

  // box2

  ac_slider_->blockSignals(true);
  batt_slider_->blockSignals(true);

  size_t value = tabSettings().targetTemperature() -
      tabSettings().targetTemperatureOffset();
  ac_slider_->setValue(static_cast<int>(value));
  ac_value_->setText(std::move(QString("%1 °C").arg(value)));

  if (tabSettings().targetTemperatureBatteryEnabled()) {
    value = tabSettings().targetTemperature() -
        tabSettings().targetTemperatureOffsetBattery();
    batt_slider_->setValue(static_cast<int>(value));
    batt_value_->setText(std::move(QString("%1 °C").arg(value)));
  }
  else {
    batt_slider_->setValue(static_cast<int>(value));
    batt_value_->setText(std::move(QString("%1 °C").arg(value)));
  }

  ac_slider_->blockSignals(false);
  batt_slider_->blockSignals(false);
}

bool ThermalControl::apply() {
  /* Exit if we are disabled */
  if (!isEnabled()) return true;

  /* Assemble the command to execute. */
  bool doRun = false;
  std::vector<std::string> cmd {
    TabSettings::ScriptPath, "-v",
    "-p", std::to_string(cpuInfo().physicalId().value)
  };

  if (tabSettings().tm2EnableEnabled()) {
    cmd.emplace_back(
        tabSettings().tm2Enable() ? "--tm2-enable" : "--tm2-disable");
    doRun = true;
  }

  if (tabSettings().tmSelectEnabled()) {
    cmd.emplace_back(
        tabSettings().tmSelect() ? "--tm2-select" : "--tm1-select");
    doRun = true;
  }

  /*
   * Set CPU Target Temperature 
   *
   * The Temperature Offset is set when:
   *  1) the Temperature Offset is enabled in the configuration.
   *  2) the current value of the Temperature Offset differs from the configured value.
   *
   * When the system is on battery power the battery target is programmed,
   * if on AC power or when the power state could not be dertermined the
   * 'on AC' target offset is programmed.
   */
  if (tabSettings().targetTemperatureEnabled()) {
    if (tabSettings().targetTemperatureBatteryEnabled()) {
      if (on_ac_power_) {
        cmd.emplace_back("--temp");
        cmd.emplace_back(std::to_string(tabSettings().targetTemperature() -
            tabSettings().targetTemperatureOffset()));
        doRun = true;
      }
      else {
        cmd.emplace_back("--temp");
        cmd.emplace_back(std::to_string(tabSettings().targetTemperature() -
            tabSettings().targetTemperatureOffsetBattery()));
        doRun = true;
      }
    }
    else {
      /* battery target disabled */
      cmd.emplace_back("--temp");
      cmd.emplace_back(std::to_string(tabSettings().targetTemperature() -
          tabSettings().targetTemperatureOffset()));
      doRun = true;
    }
  }

  /* Run the command */
#ifdef DEBUG
  std::stringstream ss;
  for (auto& s : cmd) ss << s << ' ';
  ss << '\n';
  shell_.cls();
  shell_.append(ss.str().c_str());
  return (doRun) ? shell_.run(std::move(cmd), true, false) == 0 : true;
#else
  return (doRun) ? shell_.run(std::move(cmd)) == 0 : true;
#endif
}

bool ThermalControl::compare() {
  /* Store the current UI values to a temporary object */
  Settings data(tabSettings());
  store(data);

  /* Compare the relevant values in the temporary object against the
   * global TabMemberSettings instance and return false if they differ. */

  if (data.tmSelectEnabled() != tabSettings().tmSelectEnabled() ||
      data.tmSelect() != tabSettings().tmSelect() ||
      data.targetTemperatureEnabled() != tabSettings().targetTemperatureEnabled() ||
      data.targetTemperatureOffset() != tabSettings().targetTemperatureOffset() ||
      data.targetTemperatureBatteryEnabled() != tabSettings().targetTemperatureBatteryEnabled() ||
      data.targetTemperatureOffsetBattery() != tabSettings().targetTemperatureOffsetBattery()) {
    DBGMSG("VoltageOffsets::compare(): Settings do not match")
    return false;
  }

  DBGMSG("VoltageOffsets::compare(): Settings match")
  return true;
}

void ThermalControl::timed(bool is_active_tab) {
  if (timed_count_ == 0) {
    timed_count_ = 4; // *500ms : once every 2 seconds is enough
  }
  else {
    --timed_count_;
    return;
  }

  if (is_active_tab) {
    /* Test the AC/DC status */
    switch(xxx::shell_command(TabSettings::Cmd_OnAcPower)) {
      case 0: /* on ac power */
      default: /* unknown power state */
        on_ac_power_ = true;
        break;
      case 1: /* on battery */
        on_ac_power_ = false;
        break;
    }

    /* update displayed effective target temperature */
    if (box2_->isEnabled()) {
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
      size_t ttemp;
      if (!rv && output.size() >= 10) {
        ttemp = static_cast<size_t>(strtol(&output[3].data()[31], nullptr, 10));
      }
      else {
        ttemp = tabValues().targetTemperature();
        ttemp -= tabValues().targetTemperatureOffset();
      }
      QString qs = QString("%1 °C").arg(ttemp);
      if (tabSettings().targetTemperatureEnabled()) {
        if (tabSettings().targetTemperatureBatteryEnabled()) {
          if (on_ac_power_) qs += " (on AC power)";
          else qs += " (on DC power)";
        }
        else  qs += " (on AC power)";
      }
      target_temp_->setText(std::move(qs));
    }
  }
}

/* SLOTS */

void ThermalControl::toggledSlot(bool state) {
  if (state) {
    emit valueChanged(this);
  }
}

void ThermalControl::enableCheckedSlot(int state) {
  ac_slider_->blockSignals(true);
  batt_slider_->blockSignals(true);

  /* set the new visibility of the ac and dc sliders */
  ac_label_->setEnabled(state);
  ac_slider_->setEnabled(state);
  ac_value_->setEnabled(state);
  if (!state) {
    /* if being disabled then also disable the battery slider visibility */
    batt_label_->setEnabled(false);
    batt_slider_->setEnabled(false);
    batt_value_->setEnabled(false);
    batt_enable_->setEnabled(false);
  }
  else {
    if (batt_enable_->isChecked()) {
      /* if being enabled and a seperate battery target,
       *  then enable the battery slider visibility */
      batt_label_->setEnabled(true);
      batt_slider_->setEnabled(true);
      batt_value_->setEnabled(true);
    }
    else {
      /* if being enabled but no seperate battery target,
       *  then disable the battery slider visibility */
      batt_label_->setEnabled(false);
      batt_slider_->setEnabled(false);
      batt_value_->setEnabled(false);
    }
    batt_enable_->setEnabled(true);
  }

  ac_slider_->blockSignals(false);
  batt_slider_->blockSignals(false);
  emit valueChanged(this);
}

void ThermalControl::battEnableCheckedSlot(int state) {
  batt_slider_->blockSignals(true);

  /* set the new visibility state of the battery slider */
  batt_label_->setEnabled(state);
  batt_slider_->setEnabled(state);
  batt_value_->setEnabled(state);

  /* set the value of the battery slider */
  if (state) {
    /* if being enabled: set the slider value to the battery target temperature */
    size_t value = tabSettings().targetTemperature() -
        tabSettings().targetTemperatureOffsetBattery();
    batt_slider_->setValue(static_cast<int>(value));
    auto&& s = std::to_string(value);
    s += " °C";
    batt_value_->setText(std::move(QString::fromStdString(std::move(s))));
  }
  else {
    /* if being disabled: set the slider value to the current AC target temperature */
    batt_slider_->setValue(ac_slider_->value());
    auto&& s = std::to_string(ac_slider_->value());
    s += " °C";
    batt_value_->setText(std::move(QString::fromStdString(std::move(s))));
  }

  batt_slider_->blockSignals(false);
  emit valueChanged(this);
}

void ThermalControl::targetTemperatureChangedSlot(int value) {
  batt_slider_->blockSignals(true);

  /* set the value of the ac slider and emit a valueChanged signal. */
  auto qs = QString("%1 °C").arg(value);
  if (!batt_enable_->isChecked()) {
    batt_slider_->setValue(value);
    batt_value_->setText(qs);
  }
  ac_value_->setText(std::move(qs));

  batt_slider_->blockSignals(false);
  emit valueChanged(this);
}

void ThermalControl::battTargetTemperatureChangedSlot(int value) {
  /* set the value of the battery slider and emit a valueChanged signal. */
  batt_value_->setText(std::move(QString("%1 °C").arg(value)));
  emit valueChanged(this);
}

/*
 * ThermalControl::Settings
 */

void ThermalControl::Settings::load(QSettings& qs, const TabMemberValues& cv) {

  /* select the INI section for this processor */
  std::stringstream ss;
  ss << TabSettings::INI_GRP_PROCESSOR
     << cpuInfo().physicalId().value;
  qs.beginGroup(ss.str().c_str());

  /* Load the settings or set a default value: */

  auto range = SingleCpuInfo::TargetTempRange(cpuInfo().MicroArch);

  /* - AC powered Target Temperature */
  size_t tt = qs.value(INI_TARGET_TEMPERATURE_AC,
      static_cast<quint64>(cv.targetTemperature() - cv.targetTemperatureOffset()))
      .value<quint64>();

  targetTemperature(cv.targetTemperature());
  targetTemperatureOffset(cv.targetTemperature() - tt);

  if (targetTemperatureOffset() > range) {
    targetTemperatureOffset(range);
  }

  /* - Battery powered Target Temperature */
  tt = qs.value(INI_TARGET_TEMPERATURE_BATT,
      static_cast<quint64>(cv.targetTemperature() - cv.targetTemperatureOffset()))
      .value<quint64>();

  targetTemperatureOffsetBattery(cv.targetTemperature() - tt);

  if (targetTemperatureOffsetBattery() > range) {
    targetTemperatureOffsetBattery(range);
  }

  /* - Target Temperature (AC/Batt.) enabled ? */
  targetTemperatureEnabled(
      qs.value(INI_TARGET_TEMPERATURE_ENABLE, false).toBool());

  targetTemperatureBatteryEnabled(
      qs.value(INI_TARGET_TEMPERATURE_ENABLE_BATT, false).toBool());

  /* - TM_SELECT */
  tmSelectEnabled(qs.value(INI_TM_SELECT_ENABLE, false).toBool());
  tmSelect(qs.value(INI_TM_SELECT, cv.tmSelect()).toBool());

  /* - TM2_ENABLE */
  tm2EnableEnabled(qs.value(INI_TM2_ENABLE_ENABLE, false).toBool());
  tm2Enable(qs.value(INI_TM2_ENABLE, cv.tm2Enable()).toBool());

  qs.endGroup();

#if 0
  DBGMSG("ThermalControl::Settings::tmSelectEnabled()                 <--" << tmSelectEnabled())
  DBGMSG("ThermalControl::Settings::tmSelect()                        <--" << tmSelect())
  DBGMSG("ThermalControl::Settings::tm2EnableEnabled()                <--" << tm2EnableEnabled())
  DBGMSG("ThermalControl::Settings::tm2Enable()                       <--" << tm2Enable())
  DBGMSG("ThermalControl::Settings::targetTemperature()               <--" << targetTemperature())
  DBGMSG("ThermalControl::Settings::targetTemperatureEnabled()        <--" << targetTemperatureEnabled())
  DBGMSG("ThermalControl::Settings::targetTemperatureOffset()         <--" << targetTemperatureOffset())
  DBGMSG("ThermalControl::Settings::targetTemperatureBatteryEnabled() <--" << targetTemperatureBatteryEnabled())
  DBGMSG("ThermalControl::Settings::targetTemperatureOffsetBattery()  <--" << targetTemperatureOffsetBattery())
#endif
}

void ThermalControl::Settings::save(QSettings& qs) {
  /* select the INI section for this processor */
  std::stringstream ss;
  ss << TabSettings::INI_GRP_PROCESSOR
     << tabMemberSettings().processorNumber().value;
  qs.beginGroup(ss.str().c_str());

  /* Save the settings: */

  qs.setValue(INI_TARGET_TEMPERATURE_AC,
      QVariant::fromValue<quint64>(
          targetTemperature() - targetTemperatureOffset()));

  qs.setValue(INI_TARGET_TEMPERATURE_BATT,
      QVariant::fromValue<quint64>(
          targetTemperature() - targetTemperatureOffsetBattery()));

  qs.setValue(INI_TARGET_TEMPERATURE_ENABLE,
      QVariant::fromValue<bool>(targetTemperatureEnabled()));

  qs.setValue(INI_TARGET_TEMPERATURE_ENABLE_BATT,
      QVariant::fromValue<bool>(targetTemperatureBatteryEnabled()));

  qs.setValue(INI_TM_SELECT_ENABLE, QVariant::fromValue<bool>(tmSelectEnabled()));
  qs.setValue(INI_TM_SELECT, QVariant::fromValue<bool>(tmSelect()));

  qs.setValue(INI_TM2_ENABLE_ENABLE, QVariant::fromValue<bool>(tm2EnableEnabled()));
  qs.setValue(INI_TM2_ENABLE, QVariant::fromValue<bool>(tm2Enable()));

  qs.endGroup();

#if 0
  DBGMSG("ThermalControl::Settings::tmSelectEnabled()                 -->" << tmSelectEnabled())
  DBGMSG("ThermalControl::Settings::tmSelect()                        -->" << tmSelect())
  DBGMSG("ThermalControl::Settings::tm2EnableEnabled()                -->" << tm2EnableEnabled())
  DBGMSG("ThermalControl::Settings::tm2Enable()                       -->" << tm2Enable())
  DBGMSG("ThermalControl::Settings::targetTemperature()               -->" << targetTemperature())
  DBGMSG("ThermalControl::Settings::targetTemperatureEnabled()        -->" << targetTemperatureEnabled())
  DBGMSG("ThermalControl::Settings::targetTemperatureOffset()         -->" << targetTemperatureOffset())
  DBGMSG("ThermalControl::Settings::targetTemperatureBatteryEnabled() -->" << targetTemperatureBatteryEnabled())
  DBGMSG("ThermalControl::Settings::targetTemperatureOffsetBattery()  -->" << targetTemperatureOffsetBattery())
#endif
}

