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
  * @file src/core-adjust-qt/SpeedControl.hpp
  * @brief A TabMemberWidget for adjusting processor speed related settings.
  *
  * @file src/core-adjust-qt/SpeedControl.cpp
  * @brief A TabMemberWidget for adjusting processor speed related settings (implementation).
  *
  * @class SpeedControl
  * @brief A TabMemberWidget for adjusting processor speed related settings.
  *
  * Modifies MSRs:
  *
  *   IA32_MISC_ENABLE
  *     - Enhanced Intel Speedstep Technology on/off.
  *     - Enhanced Intel Speedstep Technology Lock on/off.
  *     - IDA (Turbo Boost Technology) Disable on/off.
  *
  *   MSR_TURBO_ACTIVATION_RATIO
  *     - Adjust max. non-turbo ratio.
  *     - Lock max. non-turbo ratio.
  *
  *   MSR_TURBO_RATIO_LIMIT
  *   MSR_TURBO_RATIO_LIMIT1
  *   MSR_TURBO_RATIO_LIMIT2
  *   MSR_TURBO_RATIO_LIMIT3
  *     - Adjust Ratio Limits.
  *
  * @fn explicit SpeedControl::SpeedControl(const SingleCpuInfo& cpuInfo, const SingleCpuId& cpuId, TabMemberValues& tabValues, TabMemberSettings& tabSettings, QWidget* parent)
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
  * @class SpeedControl::Values
  * @brief Values to read from the processor for class SpeedControl.
  *
  * @fn bool SpeedControl::Values::eistEnable() const
  * @brief Get the mode of the Enhanced Intel SpeedStep Technology flag (bit 16).
  * @returns State of IA32_MISC_ENABLE.Enhanced_Intel_SpeedStep_Technology[16].
  *
  * @fn void SpeedControl::Values::eistEnable(bool state)
  * @brief Select the mode of the Enhanced Intel SpeedStep Technology flag (bit 16).
  * @param state The new state of Enhanced_Intel_SpeedStep_Technology.
  *
  * @fn bool SpeedControl::Values::eistLock() const
  * @brief Get the mode of the Enhanced Intel SpeedStep Technology Select Lock flag (bit 20).
  * @returns State of IA32_MISC_ENABLE.Enhanced_Intel_SpeedStep_Technology_Select_Lock[20].
  *
  * @fn void SpeedControl::Values::eistLock(bool state)
  * @brief Select the mode of the Enhanced Intel SpeedStep Technology Select Lock flag (bit 20).
  * @param state The new state of Enhanced_Intel_SpeedStep_Technology_Select_Lock.
  *
  * @fn bool SpeedControl::Values::tbtDisable() const
  * @brief Get the mode of the IDA (Turbo Boost Technology) Disable flag (bit 38).
  * @returns State of IA32_MISC_ENABLE.Turbo_Boost_Technology_Disable[38].
  *
  * @fn void SpeedControl::Values::tbtDisable(bool state)
  * @brief Select the mode of the IDA (Turbo Boost Technology) Disable flag (bit 38).
  * @param state The new state of Turbo_Boost_Technology_Disable.
  *
  * @fn uint8_t SpeedControl::Values::tbtActivationRatio() const
  * @brief Current MSR_TURBO_ACTIVATION_RATIO
  *
  * @fn void SpeedControl::Values::tbtActivationRatio(uint8_t ratio)
  * @brief Adjust MSR_TURBO_ACTIVATION_RATIO
  *
  * @fn bool SpeedControl::Values::tbtActivationRatioLock() const
  * @brief MSR_TURBO_ACTIVATION_RATIO locked?
  *
  * @fn void SpeedControl::Values::tbtActivationRatioLock(bool state)
  * @brief Lock MSR_TURBO_ACTIVATION_RATIO
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit1C() const
  * @brief The ratio limit when 1 core is active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit1C(uint8_t ratio)
  * @brief Set the ratio limit for when 1 core is active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit2C() const
  * @brief The ratio limit when 2 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit2C(uint8_t ratio)
  * @brief Set the ratio limit for when 2 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit3C() const
  * @brief The ratio limit when 3 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit3C(uint8_t ratio)
  * @brief Set the ratio limit for when 3 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit4C() const
  * @brief The ratio limit when 4 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit4C(uint8_t ratio)
  * @brief Set the ratio limit for when 4 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit5C() const
  * @brief The ratio limit when 5 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit5C(uint8_t ratio)
  * @brief Set the ratio limit for when 5 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit6C() const
  * @brief The ratio limit when 6 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit6C(uint8_t ratio)
  * @brief Set the ratio limit for when 6 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit7C() const
  * @brief The ratio limit when 7 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit7C(uint8_t ratio)
  * @brief Set the ratio limit for when 7 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit8C() const
  * @brief The ratio limit when 8 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit8C(uint8_t ratio)
  * @brief Set the ratio limit for when 8 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit9C() const
  * @brief The ratio limit when 9 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit9C(uint8_t ratio)
  * @brief Set the ratio limit for when 9 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit10C() const
  * @brief The ratio limit when 10 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit10C(uint8_t ratio)
  * @brief Set the ratio limit for when 10 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit11C() const
  * @brief The ratio limit when 11 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit11C(uint8_t ratio)
  * @brief Set the ratio limit for when 11 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit12C() const
  * @brief The ratio limit when 12 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit12C(uint8_t ratio)
  * @brief Set the ratio limit for when 12 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit13C() const
  * @brief The ratio limit when 13 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit13C(uint8_t ratio)
  * @brief Set the ratio limit for when 13 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit14C() const
  * @brief The ratio limit when 14 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit14C(uint8_t ratio)
  * @brief Set the ratio limit for when 14 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit15C() const
  * @brief The ratio limit when 15 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit15C(uint8_t ratio)
  * @brief Set the ratio limit for when 15 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit16C() const
  * @brief The ratio limit when 16 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit16C(uint8_t ratio)
  * @brief Set the ratio limit for when 16 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit17C() const
  * @brief The ratio limit when 17 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit17C(uint8_t ratio)
  * @brief Set the ratio limit for when 17 cores are active.
  *
  * @fn uint8_t SpeedControl::Values::tbtRatioLimit18C() const
  * @brief The ratio limit when 18 cores are active.
  *
  * @fn void SpeedControl::Values::tbtRatioLimit18C(uint8_t ratio)
  * @brief Set the ratio limit for when 18 cores are active.
  *
  * @fn bool SpeedControl::Values::tbtRatioLimitEnable() const
  * @brief Enable ratio limits? (getter)
  *
  * @fn void SpeedControl::Values::tbtRatioLimitEnable(bool state)
  * @brief Enable ratio limits? (setter)
  *
  *
  *
  * @class SpeedControl::Settings
  * @brief Settings for class SpeedControl.
  *
  * @fn virtual void SpeedControl::Settings::load(QSettings&, const SingleCpuInfo&, const TabMemberValues&)
  * @brief Load the settings for this TabMemberWidget from the INI file.
  *
  * @fn virtual void SpeedControl::Settings::save(QSettings&)
  * @brief Save the settings for this TabMemberWidget to the INI file.
  *
  * @fn bool SpeedControl::Settings::eistEnableEnabled() const
  * @brief Adjust EIST? (getter)
  *
  * @fn void SpeedControl::Settings::eistEnableEnabled(bool state)
  * @brief Adjust EIST? (setter)
  *
  * @fn bool SpeedControl::Settings::eistLockEnabled() const
  * @brief Adjust EIST Lock? (getter)
  *
  * @fn void SpeedControl::Settings::eistLockEnabled(bool state)
  * @brief Adjust EIST Lock? (getter)
  *
  * @fn bool SpeedControl::Settings::tbtDisableEnabled() const
  * @brief Disable IDA enabled? (getter)
  *
  * @fn void SpeedControl::Settings::tbtDisableEnabled(bool state)
  * @brief Disable IDA enabled? (setter)
  *
  * @fn bool SpeedControl::Settings::tbtActivationRatioEnabled() const
  * @brief Adjust MSR_TURBO_ACTIVATION_RATIO? (getter)
  *
  * @fn void SpeedControl::Settings::tbtActivationRatioEnabled(bool state)
  * @brief Adjust MSR_TURBO_ACTIVATION_RATIO? (setter)
  *
  * @fn bool SpeedControl::Settings::tbtActivationRatioLockEnabled() const
  * @brief Adjust core Ratio Limits? (getter)
  *
  * @fn void SpeedControl::Settings::tbtActivationRatioLockEnabled(bool state)
  * @brief Adjust core Ratio Limits? (setter)
  */
// STL
#include <sstream>
// Qt
#include <QApplication>
#include <QButtonGroup>
#include <QDebug>
#include <QGroupBox>
#include <QMessageBox>
#include <QVBoxLayout>
// App
#include "Msr.hpp"
#include "CpuId.hpp"
#include "CpuInfo.hpp"
#include "TabMember.hpp"
#include "SpeedControl.hpp"
#include "config.h"

SpeedControl::SpeedControl(
    const SingleCpuInfo& info,
    const SingleCpuId& id,
    TabMemberValues& values,
    TabMemberSettings& data,
    QWidget *parent)
    : TabMemberTemplate(info, id, values, data, parent),
      shell_(this, "Adjust MSR settings", ShellCommand::EnableStd | ShellCommand::DisableAbortButton) {

  /* layout for gb_features groupbox */
  auto* grid_features = new QGridLayout();
  grid_features->setColumnStretch(0, 1);
  grid_features->setColumnStretch(4, 1);

  /* row0: EIST on/off */
  EIST_enable_name_ = new QLabel(tr("EIST (SpeedStep)"));
  EIST_enable_on_ = new QRadioButton(tr("On"));
  EIST_enable_off_ = new QRadioButton(tr("Off"));
  EIST_enable_default_ = new QRadioButton(tr("Do not adjust"));
  EIST_enable_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 16</nobr><br/>"
      "<nobr>Enable or disable 'Enhanced Intel SpeedStep Technology'."
      "</nobr></p>"));
  EIST_enable_on_->setToolTip(tr("This option will set the bit to 1."));
  EIST_enable_off_->setToolTip(tr("This option will reset the bit to 0."));
  EIST_enable_default_->setToolTip(tr("This option will leave the bit as is."));
  EIST_enable_current_ = new QLabel(tr("Not Supported"));
  EIST_enable_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  EIST_enable_grp_ = new QButtonGroup(this);
  EIST_enable_grp_->addButton(EIST_enable_on_, 1);
  EIST_enable_grp_->addButton(EIST_enable_off_, 2);
  EIST_enable_grp_->addButton(EIST_enable_default_, 3);
  grid_features->addWidget(EIST_enable_name_, 0, 0);
  grid_features->addWidget(EIST_enable_on_, 0, 1);
  grid_features->addWidget(EIST_enable_off_, 0, 2);
  grid_features->addWidget(EIST_enable_default_, 0, 3);
  grid_features->addWidget(EIST_enable_current_, 0, 5);

  /* row1: EIST Lock on/off */
  EIST_lock_name_ = new QLabel(tr("EIST Select Lock"));
  EIST_lock_on_ = new QRadioButton(tr("On"));
  EIST_lock_off_ = new QRadioButton(tr("Off"));
  EIST_lock_default_ = new QRadioButton(tr("Do not adjust"));
  EIST_lock_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 20</nobr></p>"
      "<p>This bit must be set before an Enhanced Intel SpeedStep "
      "Technology transition is requested, This bit is cleared on reset.</p>"));
  EIST_lock_on_->setToolTip(tr("This option will set the bit to 1."));
  EIST_lock_off_->setToolTip(tr("This option will reset the bit to 0."));
  EIST_lock_off_->setEnabled(false);
  EIST_lock_default_->setToolTip(tr("This option will leave the bit as is."));
  EIST_lock_current_ = new QLabel(tr("Not Supported"));
  EIST_lock_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  EIST_lock_grp_ = new QButtonGroup(this);
  EIST_lock_grp_->addButton(EIST_lock_on_, 1);
  EIST_lock_grp_->addButton(EIST_lock_off_, 2);
  EIST_lock_grp_->addButton(EIST_lock_default_, 3);
  grid_features->addWidget(EIST_lock_name_, 1, 0);
  grid_features->addWidget(EIST_lock_on_, 1, 1);
  grid_features->addWidget(EIST_lock_off_, 1, 2);
  grid_features->addWidget(EIST_lock_default_, 1, 3);
  grid_features->addWidget(EIST_lock_current_, 1, 5);

  /* row2: TBT on/off */
  IDA_disable_name_ = new QLabel(tr("Turbo Boost Technology"));
  IDA_disable_on_ = new QRadioButton(tr("Off"));
  IDA_disable_off_ = new QRadioButton(tr("On"));
  IDA_disable_default_ = new QRadioButton(tr("Do not adjust"));
  IDA_disable_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 38</nobr><br/>"
      "<nobr>Enable or disable IDA (Turbo Boost Technology).</nobr></p>"));
  IDA_disable_on_->setToolTip(tr("This option will set the bit to 1."));
  IDA_disable_off_->setToolTip(tr("This option will reset the bit to 0."));
  IDA_disable_default_->setToolTip(tr("This option will leave the bit as it is."));
  IDA_disable_current_ = new QLabel(tr("Disabled"));
  IDA_disable_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  IDA_disable_grp_ = new QButtonGroup(this);
  IDA_disable_grp_->addButton(IDA_disable_on_, 1);
  IDA_disable_grp_->addButton(IDA_disable_off_, 2);
  IDA_disable_grp_->addButton(IDA_disable_default_, 3);
  grid_features->addWidget(IDA_disable_name_, 2, 0);
  grid_features->addWidget(IDA_disable_on_, 2, 2);
  grid_features->addWidget(IDA_disable_off_, 2, 1);
  grid_features->addWidget(IDA_disable_default_, 2, 3);
  grid_features->addWidget(IDA_disable_current_, 2, 5);

  /* row3: spacer */
  grid_features->setRowMinimumHeight(3, 10);

  /* row4: Adjust max. non-turbo ratio */
  Max_nonturbo_ratio_enable_ = new QCheckBox(tr("Adjust max. non-turbo ratio"));
  Max_nonturbo_ratio_ = new QSpinBox();
  Max_nonturbo_ratio_->setRange(0, 255);
  Max_nonturbo_ratio_enable_->setToolTip(tr(
      "<p><nobr>MSR_TURBO_ACTIVATION_RATIO bits 7:0.</nobr><br/>"
      "<nobr>Adjust the maximum non-turbo ratio.</nobr></p>"));
  Max_nonturbo_ratio_current_ = new QLabel(tr("Locked @ XX"));
  Max_nonturbo_ratio_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  grid_features->addWidget(Max_nonturbo_ratio_enable_, 4, 0);
  grid_features->addWidget(Max_nonturbo_ratio_, 4, 1, 1, 3);
  grid_features->addWidget(Max_nonturbo_ratio_current_, 4, 5, 2, 1);

  /* row5: Lock max. non-turbo ratio */
  Max_nonturbo_ratio_lock_name_ = new QLabel(tr("Lock max. non-turbo ratio"));
  Max_nonturbo_ratio_lock_on_ = new QRadioButton(tr("On"));
  Max_nonturbo_ratio_lock_off_ = new QRadioButton(tr("Off"));
  Max_nonturbo_ratio_lock_default_ = new QRadioButton(tr("Do not adjust"));
  Max_nonturbo_ratio_lock_name_->setToolTip(tr(
      "<p>"
      "<nobr>MSR_TURBO_ACTIVATION_RATIO bit 31.</nobr><br/>"
      "When this bit is set, the content of this register is locked "
      "until a (cold!) reset.</p>"));
  Max_nonturbo_ratio_lock_on_->setToolTip(tr("This option will set the bit to 1."));
  Max_nonturbo_ratio_lock_off_->setToolTip(tr("This option will reset the bit to 0."));
  Max_nonturbo_ratio_lock_off_->setEnabled(false);
  Max_nonturbo_ratio_lock_default_->setToolTip(tr("This option will leave the bit as is."));
  Max_nonturbo_ratio_lock_grp_ = new QButtonGroup(this);
  Max_nonturbo_ratio_lock_grp_->addButton(Max_nonturbo_ratio_lock_on_, 1);
  Max_nonturbo_ratio_lock_grp_->addButton(Max_nonturbo_ratio_lock_off_, 2);
  Max_nonturbo_ratio_lock_grp_->addButton(Max_nonturbo_ratio_lock_default_, 3);
  grid_features->addWidget(Max_nonturbo_ratio_lock_name_, 5, 0);
  grid_features->addWidget(Max_nonturbo_ratio_lock_on_, 5, 1);
  grid_features->addWidget(Max_nonturbo_ratio_lock_off_, 5, 2);
  grid_features->addWidget(Max_nonturbo_ratio_lock_default_, 5, 3);

  /* GroupBox: processor features */
  auto* gb_features = new QGroupBox(tr("Adjust Processor Features"));
  gb_features->setLayout(grid_features);
  gb_features->setFlat(true);

  /* complete layout: processor features */
  auto* vb_features = new QVBoxLayout();
  vb_features->addWidget(gb_features);
  vb_features->addStretch(1);

  /* Adjust Ratio Limits */

  /* tab: core 1-8 */
  auto* tab_core1_8 = new QWidget();
  auto* label_core1_8 = new QLabel("Ratio Limit for:");
  auto* label_current1_8 = new QLabel("Current Ratio Limit:");
  auto* grid_core1_8 = new QGridLayout();
  grid_core1_8->setRowStretch(9, 1);
  grid_core1_8->setColumnStretch(5, 1);
  grid_core1_8->setColumnMinimumWidth(2, 40);
  grid_core1_8->setColumnMinimumWidth(0, 120);
  grid_core1_8->addWidget(label_core1_8, 0, 0);
  grid_core1_8->addWidget(label_current1_8, 0, 3, 1, 3);
  for (int i = 1; i < 9; ++i) {
    if (i == 1) {
      label_[0] = new QLabel("1 active core");
    }
    else {
      label_[i - 1] = new QLabel(std::move(QString("%1 active cores").arg(i)));
    }
    spinner_[i - 1] = new QSpinBox();
    current_[i - 1] = new QLabel("100");
    arrow_[i - 1] = new QLabel(QString::fromUtf8(u8"\u21D2"));
    note_[i - 1] = new QLabel("XXXXX MHz");
    grid_core1_8->addWidget(label_[i - 1],   i, 0);
    grid_core1_8->addWidget(spinner_[i - 1], i, 1);
    grid_core1_8->addWidget(current_[i - 1], i, 3);
    grid_core1_8->addWidget(arrow_[i - 1],   i, 4);
    grid_core1_8->addWidget(note_[i - 1],    i, 5);
  }
  tab_core1_8->setLayout(grid_core1_8);

  /* tab: core 9-16 */
  auto* tab_core9_16 = new QWidget();
  auto* label_core9_16 = new QLabel("Ratio Limit for:");
  auto* label_current9_16 = new QLabel("Current Ratio Limit:");
  auto* grid_core9_16 = new QGridLayout();
  grid_core9_16->setRowStretch(9, 1);
  grid_core9_16->setColumnStretch(5, 1);
  grid_core9_16->setColumnMinimumWidth(2, 40);
  grid_core9_16->setColumnMinimumWidth(0, 120);
  grid_core9_16->addWidget(label_core9_16, 0, 0);
  grid_core9_16->addWidget(label_current9_16, 0, 3, 1, 3);
  for (int i = 9; i < 17; ++i) {
    label_[i - 1] = new QLabel(std::move(QString("%1 active cores").arg(i)));
    spinner_[i - 1] = new QSpinBox();
    current_[i - 1] = new QLabel("100");
    arrow_[i - 1] = new QLabel(QString::fromUtf8(u8"\u21D2"));
    note_[i - 1] = new QLabel("XXXXX MHz");
    grid_core9_16->addWidget(label_[i - 1],   i - 8, 0);
    grid_core9_16->addWidget(spinner_[i - 1], i - 8, 1);
    grid_core9_16->addWidget(current_[i - 1], i - 8, 3);
    grid_core9_16->addWidget(arrow_[i - 1],   i - 8, 4);
    grid_core9_16->addWidget(note_[i - 1],    i - 8, 5);
  }
  tab_core9_16->setLayout(grid_core9_16);

  /* tab: core 17-18 */
  auto* tab_core17_18 = new QWidget();
  auto* label_core17_18 = new QLabel("Ratio Limit for:");
  auto* label_current17_18 = new QLabel("Current Ratio Limit:");
  auto* grid_core17_18 = new QGridLayout();
  grid_core17_18->setRowStretch(9, 1);
  grid_core17_18->setColumnStretch(5, 1);
  grid_core17_18->setColumnMinimumWidth(2, 40);
  grid_core17_18->setColumnMinimumWidth(0, 120);
  grid_core17_18->addWidget(label_core17_18, 0, 0);
  grid_core17_18->addWidget(label_current17_18, 0, 3, 1, 3);
  for (int i = 17; i < 19; ++i) {
    label_[i - 1] = new QLabel(std::move(QString("%1 active cores").arg(i)));
    spinner_[i - 1] = new QSpinBox();
    current_[i - 1] = new QLabel("100");
    arrow_[i - 1] = new QLabel(QString::fromUtf8(u8"\u21D2"));
    note_[i - 1] = new QLabel("XXXXX MHz");
    grid_core17_18->addWidget(label_[i - 1],  i - 16, 0);
    grid_core17_18->addWidget(spinner_[i - 1], i - 16, 1);
    grid_core17_18->addWidget(current_[i - 1], i - 16, 3);
    grid_core17_18->addWidget(arrow_[i - 1],   i - 16, 4);
    grid_core17_18->addWidget(note_[i - 1],    i - 16, 5);
  }
  tab_core17_18->setLayout(grid_core17_18);

  for (int i = 0; i < 18; ++i) {
    label_[i]->setEnabled(false);
    spinner_[i]->setEnabled(false);
    current_[i]->hide();
    arrow_[i]->hide();
    note_[i]->hide();
  }

  tab_core1_8->setToolTip(
      "<p><nobr>MSR_TURBO_RATIO_LIMIT bits 63:0</nobr></p>");
  tab_core9_16->setToolTip(
      "<p><nobr>MSR_TURBO_RATIO_LIMIT1 bits 63:0</nobr></p>");
  tab_core17_18->setToolTip(
      "<p><nobr>MSR_TURBO_RATIO_LIMIT2 bits 15:0</nobr></p>");

  /* tab widget: ratio limits */
  tabs_ratio_limits_ = new QTabWidget();
  tabs_ratio_limits_->addTab(tab_core1_8, "Core 1-8");
  tabs_ratio_limits_->addTab(tab_core9_16, "Core 9-16");
  tabs_ratio_limits_->addTab(tab_core17_18, "Core 17-18");

  /* checkbox: enable ratio limits */
  auto* hb_enable_ratio_limit = new QHBoxLayout();
  enable_ratio_limit_ = new QCheckBox("Enable custom Turbo Boost Ratio Limits.");
  enable_ratio_limit_current_ = new QLabel("(Enabled by BIOS)"); // if cfg != current @ startup
  hb_enable_ratio_limit->addWidget(enable_ratio_limit_);
  hb_enable_ratio_limit->addWidget(enable_ratio_limit_current_);

  /* layout for gb_ratio groupbox */
  auto* gbl_ratio = new QVBoxLayout();
  gbl_ratio->addWidget(tabs_ratio_limits_);
  gbl_ratio->addLayout(hb_enable_ratio_limit);

  /* GroupBox: ratio limits */
  auto* gb_ratio = new QGroupBox("Adjust Turbo Boost Ratio Limits");
  gb_ratio->setLayout(gbl_ratio);
  gb_ratio->setFlat(true);

  /* complete layout: ratio limits */
  auto* vb_ratio = new QVBoxLayout();
  vb_ratio->addWidget(gb_ratio);
  vb_ratio->addStretch(1);

  /* Add the inner widgets to the layout of the scroll widget */

  scroll_layout_->addStretch(1);
  scroll_layout_->addLayout(vb_features);
  scroll_layout_->addStretch(1);
  scroll_layout_->addLayout(vb_ratio);
  scroll_layout_->addStretch(1);

  /* Signals */

  connect(EIST_enable_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(EIST_lock_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(IDA_disable_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(Max_nonturbo_ratio_enable_, SIGNAL(stateChanged(int)),
      this, SLOT(Max_nonturbo_ratio_enable_changed(int)));

  connect(Max_nonturbo_ratio_, SIGNAL(valueChanged(int)),
      this, SLOT(Max_nonturbo_ratio_changed(int)));

  connect(Max_nonturbo_ratio_lock_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(enable_ratio_limit_, SIGNAL(stateChanged(int)),
      this, SLOT(enable_ratio_limit_changed(int)));

  for (int i = 0; i < 18; ++i) {
    connect(spinner_[i], SIGNAL(valueChanged(int)),
        this, SLOT(spinner_changed(int)));
  }

  connect(&shell_, SIGNAL(finished()), &shell_, SLOT(accept()));
}

bool SpeedControl::read(std::ostringstream& ss) {
  bool retv = true;

  /* Set defaults */
  tabValues().eistEnable(false);
  tabValues().eistLock(false);
  tabValues().tbtDisable(false);
  tabValues().tbtActivationRatio(0);
  tabValues().tbtActivationRatioLock(false);
  tabValues().tbtRatioLimit1C(0);
  tabValues().tbtRatioLimit2C(0);
  tabValues().tbtRatioLimit3C(0);
  tabValues().tbtRatioLimit4C(0);
  tabValues().tbtRatioLimit5C(0);
  tabValues().tbtRatioLimit6C(0);
  tabValues().tbtRatioLimit7C(0);
  tabValues().tbtRatioLimit8C(0);
  tabValues().tbtRatioLimit9C(0);
  tabValues().tbtRatioLimit10C(0);
  tabValues().tbtRatioLimit11C(0);
  tabValues().tbtRatioLimit12C(0);
  tabValues().tbtRatioLimit13C(0);
  tabValues().tbtRatioLimit14C(0);
  tabValues().tbtRatioLimit15C(0);
  tabValues().tbtRatioLimit16C(0);
  tabValues().tbtRatioLimit17C(0);
  tabValues().tbtRatioLimit18C(0);

  /*
   * Read the values for this tab from the processor
   */

  IA32_MISC_ENABLE ia32_misc_enable(cpuInfo().firstLogicalCpu());
  if (ia32_misc_enable.read()) {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, Could not read IA32_MISC_ENABLE!</b></p>");
    setEnabled(false);
  }
  tabValues().eistEnable(ia32_misc_enable.EIST_Enable != 0);
  tabValues().eistLock(ia32_misc_enable.EIST_Select_Lock != 0);
  tabValues().tbtDisable(ia32_misc_enable.IDA_Disable != 0);

  MSR_TURBO_ACTIVATION_RATIO msr_turbo_activation_ratio(cpuInfo().firstLogicalCpu());
  if (msr_turbo_activation_ratio.read()) {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, Could not read MSR_TURBO_ACTIVATION_RATIO!</b></p>");
    setEnabled(false);
  }
  tabValues().tbtActivationRatio(msr_turbo_activation_ratio.MAX_NON_TURBO_RATIO != 0);
  tabValues().tbtActivationRatioLock(msr_turbo_activation_ratio.TURBO_ACTIVATION_RATIO_Lock != 0);

  MSR_TURBO_RATIO_LIMIT msr_turbo_ratio_limit(cpuInfo().firstLogicalCpu());
  if (msr_turbo_ratio_limit.read()) {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, Could not read MSR_TURBO_RATIO_LIMIT!</b></p>");
    setEnabled(false);
  }
  tabValues().tbtRatioLimit1C(msr_turbo_ratio_limit.Ratio_Limit_1C);
  tabValues().tbtRatioLimit2C(msr_turbo_ratio_limit.Ratio_Limit_2C);
  tabValues().tbtRatioLimit3C(msr_turbo_ratio_limit.Ratio_Limit_3C);
  tabValues().tbtRatioLimit4C(msr_turbo_ratio_limit.Ratio_Limit_4C);
  tabValues().tbtRatioLimit5C(msr_turbo_ratio_limit.Ratio_Limit_5C);
  tabValues().tbtRatioLimit6C(msr_turbo_ratio_limit.Ratio_Limit_6C);
  tabValues().tbtRatioLimit7C(msr_turbo_ratio_limit.Ratio_Limit_7C);
  tabValues().tbtRatioLimit8C(msr_turbo_ratio_limit.Ratio_Limit_8C);

  MSR_TURBO_RATIO_LIMIT1 msr_turbo_ratio_limit1(cpuInfo().firstLogicalCpu());
  if (msr_turbo_ratio_limit1.read() && cpuInfo().cores() > 8) {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, Could not read MSR_TURBO_RATIO_LIMIT1!</b></p>");
    setEnabled(false);
  }
  tabValues().tbtRatioLimit9C(msr_turbo_ratio_limit1.Ratio_Limit_9C);
  tabValues().tbtRatioLimit10C(msr_turbo_ratio_limit1.Ratio_Limit_10C);
  tabValues().tbtRatioLimit11C(msr_turbo_ratio_limit1.Ratio_Limit_11C);
  tabValues().tbtRatioLimit12C(msr_turbo_ratio_limit1.Ratio_Limit_12C);
  tabValues().tbtRatioLimit13C(msr_turbo_ratio_limit1.Ratio_Limit_13C);
  tabValues().tbtRatioLimit14C(msr_turbo_ratio_limit1.Ratio_Limit_14C);
  tabValues().tbtRatioLimit15C(msr_turbo_ratio_limit1.Ratio_Limit_15C);
  tabValues().tbtRatioLimit16C(msr_turbo_ratio_limit1.Ratio_Limit_16C);

  MSR_TURBO_RATIO_LIMIT2 msr_turbo_ratio_limit2(cpuInfo().firstLogicalCpu());
  if (msr_turbo_ratio_limit2.read() && cpuInfo().cores() > 16) {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, Could not read MSR_TURBO_RATIO_LIMIT2!</b></p>");
    setEnabled(false);
  }
  tabValues().tbtRatioLimit17C(msr_turbo_ratio_limit2.Ratio_Limit_17C);
  tabValues().tbtRatioLimit18C(msr_turbo_ratio_limit2.Ratio_Limit_18C);

  /* Exit if an error occurred when reading the MSRs */
  if (!isEnabled()) return true;

  /*
   * Compare the newly read values against the desired values
   */

  if (tabSettings().eistEnableEnabled()) {
    if (tabSettings().eistEnable() != tabValues().eistEnable()) {
      ss << "<li><nobr>'<b>EIST (SpeedStep) Enable</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().eistEnable() << "</b> instead of <b>"
         << tabSettings().eistEnable() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().eistLockEnabled()) {
    if (tabSettings().eistLock() != tabValues().eistLock()) {
      ss << "<li><nobr>'<b>EIST (SpeedStep) Select Lock</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().eistLock() << "</b> instead of <b>"
         << tabSettings().eistLock() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().tbtDisableEnabled()) {
    if (tabSettings().tbtDisable() != tabValues().tbtDisable()) {
      ss << "<li><nobr>'<b>Turbo Boost Technology Disable</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtDisable() << "</b> instead of <b>"
         << tabSettings().tbtDisable() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().tbtActivationRatioEnabled()) {
    if (tabSettings().tbtActivationRatio() != tabValues().tbtActivationRatio()) {
      ss << "<li><nobr>'<b>Max. non─turbo ratio</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtActivationRatio() << "</b> instead of <b>"
         << tabSettings().tbtActivationRatio()<< "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().tbtActivationRatioLockEnabled()) {
    if (tabSettings().tbtActivationRatioLock() != tabValues().tbtActivationRatioLock()) {
      ss << "<li><nobr>'<b>Lock max. non─turbo ratio</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtActivationRatioLock() << "</b> instead of <b>"
         << tabSettings().tbtActivationRatioLock()<< "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().tbtRatioLimitEnable()) {
    if (tabSettings().tbtRatioLimit1C() != tabValues().tbtRatioLimit1C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit1C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit1C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit2C() != tabValues().tbtRatioLimit2C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit2C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit2C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit3C() != tabValues().tbtRatioLimit3C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit3C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit3C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit4C() != tabValues().tbtRatioLimit4C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit4C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit4C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit5C() != tabValues().tbtRatioLimit5C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit5C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit5C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit6C() != tabValues().tbtRatioLimit6C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit6C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit6C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit7C() != tabValues().tbtRatioLimit7C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit7C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit7C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit8C() != tabValues().tbtRatioLimit8C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit8C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit8C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit9C() != tabValues().tbtRatioLimit9C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit9C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit9C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit10C() != tabValues().tbtRatioLimit10C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit10C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit10C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit11C() != tabValues().tbtRatioLimit11C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit11C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit11C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit12C() != tabValues().tbtRatioLimit12C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit12C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit12C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit13C() != tabValues().tbtRatioLimit13C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit13C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit13C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit14C() != tabValues().tbtRatioLimit14C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit14C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit14C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit15C() != tabValues().tbtRatioLimit15C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit15C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit15C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit16C() != tabValues().tbtRatioLimit16C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit16C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit16C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit17C() != tabValues().tbtRatioLimit17C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit17C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit17C() << "</b>.</nobr></li>";
      retv = false;
    }

    if (tabSettings().tbtRatioLimit18C() != tabValues().tbtRatioLimit18C()) {
      ss << "<li><nobr>'<b>Ratio_Limit_1C</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().tbtRatioLimit18C() << "</b> instead of <b>"
         << tabSettings().tbtRatioLimit18C() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  /*
   * Return false if the newly read values differ from the desired values.
   */
  return retv;
}

void SpeedControl::load() {
  /* EIST on/off */
  EIST_enable_grp_->blockSignals(true);
  if (tabSettings().eistEnableEnabled()) {
    if (tabSettings().eistEnable()) EIST_enable_on_->setChecked(true);
    else EIST_enable_off_->setChecked(true);
  }
  else {
    EIST_enable_default_->setChecked(true);
  }
  EIST_enable_grp_->blockSignals(false);

  /* EIST Lock on/off */
  EIST_lock_grp_->blockSignals(true);
  if (tabSettings().eistLockEnabled()) {
    if (tabSettings().eistLock()) EIST_lock_on_->setChecked(true);
    else EIST_lock_off_->setChecked(true);
  }
  else {
    EIST_lock_default_->setChecked(true);
  }
  EIST_lock_grp_->blockSignals(false);

  /* TBT on/off */
  IDA_disable_grp_->blockSignals(true);
  if (tabSettings().tbtDisableEnabled()) {
    if (tabSettings().tbtDisable()) IDA_disable_on_->setChecked(true);
    else IDA_disable_off_->setChecked(true);
  }
  else {
    IDA_disable_default_->setChecked(true);
  }
  IDA_disable_grp_->blockSignals(false);

  /* Adjust max. non-turbo ratio */
  Max_nonturbo_ratio_enable_->blockSignals(true);
  Max_nonturbo_ratio_->blockSignals(true);
  Max_nonturbo_ratio_->setValue(tabSettings().tbtActivationRatio());
  if (tabSettings().tbtActivationRatioEnabled()) {
    Max_nonturbo_ratio_enable_->setChecked(true);
    Max_nonturbo_ratio_->setEnabled(true);
  }
  else {
    Max_nonturbo_ratio_enable_->setChecked(false);
    Max_nonturbo_ratio_->setEnabled(false);
  }
  Max_nonturbo_ratio_enable_->blockSignals(false);
  Max_nonturbo_ratio_->blockSignals(false);

  /* Lock max. non-turbo ratio */
  Max_nonturbo_ratio_lock_grp_->blockSignals(true);
  if (tabSettings().tbtActivationRatioLockEnabled()) {
    if (tabSettings().tbtActivationRatioLock()) Max_nonturbo_ratio_lock_on_->setChecked(true);
    else Max_nonturbo_ratio_lock_off_->setChecked(true);
  }
  else {
    Max_nonturbo_ratio_lock_default_->setChecked(true);
  }
  Max_nonturbo_ratio_lock_grp_->blockSignals(false);

  /* Adjust Ratio Limits */
  for (unsigned int i = 0; i < cpuInfo().cores(); ++i) {
    switch (i) {
      case 0: spinner_[i]->setValue(tabSettings().tbtRatioLimit1C()); break;
      case 1: spinner_[i]->setValue(tabSettings().tbtRatioLimit2C()); break;
      case 2: spinner_[i]->setValue(tabSettings().tbtRatioLimit3C()); break;
      case 3: spinner_[i]->setValue(tabSettings().tbtRatioLimit4C()); break;
      case 4: spinner_[i]->setValue(tabSettings().tbtRatioLimit5C()); break;
      case 5: spinner_[i]->setValue(tabSettings().tbtRatioLimit6C()); break;
      case 6: spinner_[i]->setValue(tabSettings().tbtRatioLimit7C()); break;
      case 7: spinner_[i]->setValue(tabSettings().tbtRatioLimit8C()); break;
      case 8: spinner_[i]->setValue(tabSettings().tbtRatioLimit9C()); break;
      case 9: spinner_[i]->setValue(tabSettings().tbtRatioLimit10C()); break;
      case 10: spinner_[i]->setValue(tabSettings().tbtRatioLimit11C()); break;
      case 11: spinner_[i]->setValue(tabSettings().tbtRatioLimit12C()); break;
      case 12: spinner_[i]->setValue(tabSettings().tbtRatioLimit13C()); break;
      case 13: spinner_[i]->setValue(tabSettings().tbtRatioLimit14C()); break;
      case 14: spinner_[i]->setValue(tabSettings().tbtRatioLimit15C()); break;
      case 15: spinner_[i]->setValue(tabSettings().tbtRatioLimit16C()); break;
      case 16: spinner_[i]->setValue(tabSettings().tbtRatioLimit17C()); break;
      case 17: spinner_[i]->setValue(tabSettings().tbtRatioLimit18C()); break;
    }
  }

  /* Adjust Ratio Limits Enabled */
  enable_ratio_limit_->blockSignals(true);
  if (tabSettings().tbtRatioLimitEnable()) {
    enable_ratio_limit_->setChecked(true);
    tabs_ratio_limits_->setEnabled(true);
  }
  else {
    enable_ratio_limit_->setChecked(false);
    tabs_ratio_limits_->setEnabled(false);
  }
  enable_ratio_limit_->blockSignals(false);
}

void SpeedControl::store() {
  /* Store current UI values to the global TabMemberSettings instance */
  store(tabSettings());
}

void SpeedControl::store(Settings& data) {
  /* Store current UI values to the specified TabMemberSettings instance */

  /* EIST on/off */
  if (EIST_enable_default_->isChecked()) {
    data.eistEnableEnabled(false);
    data.eistEnable(tabValues().eistEnable());
  }
  else {
    data.eistEnableEnabled(true);
    if (EIST_enable_on_->isChecked()) {
      data.eistEnable(true);
    }
    else {
      data.eistEnable(false);
    }
  }

  /* EIST Lock on/off */
  if (EIST_lock_default_->isChecked()) {
    data.eistLockEnabled(false);
    data.eistLock(tabValues().eistLock());
  }
  else {
    data.eistLockEnabled(true);
    if (EIST_lock_on_->isChecked()) {
      data.eistLock(true);
    }
    else {
      data.eistLock(false);
    }
  }

  /* TBT on/off */
  if (IDA_disable_default_->isChecked()) {
    data.tbtDisableEnabled(false);
    data.tbtDisable(tabValues().tbtDisable());
  }
  else {
    data.tbtDisableEnabled(true);
    if (IDA_disable_on_->isChecked()) {
      data.tbtDisable(true);
    }
    else {
      data.tbtDisable(false);
    }
  }

  /* Adjust max. non-turbo ratio */
  if (Max_nonturbo_ratio_enable_->isChecked()) {
    data.tbtActivationRatioEnabled(true);
    data.tbtActivationRatio(static_cast<uint8_t>(Max_nonturbo_ratio_->value()));
  }
  else {
    data.tbtActivationRatioEnabled(false);
    data.tbtActivationRatio(tabValues().tbtActivationRatio());
  }

  /* Lock max. non-turbo ratio */
  if (Max_nonturbo_ratio_lock_default_->isChecked()) {
    data.tbtActivationRatioLockEnabled(false);
    data.tbtActivationRatioLock(tabValues().tbtActivationRatioLock());
  }
  else {
    data.tbtActivationRatioLockEnabled(true);
    if (Max_nonturbo_ratio_lock_on_->isChecked()) {
      data.tbtActivationRatioLock(true);
    }
    else {
      data.tbtActivationRatioLock(false);
    }
  }

  /* Adjust Ratio Limits + */
  /* Adjust Ratio Limits Enabled */
  if (enable_ratio_limit_->isChecked()) {
    data.tbtRatioLimitEnable(true);
    for (unsigned int i = 0; i < cpuInfo().cores(); ++i) {
      switch (i) {
        case 0: data.tbtRatioLimit1C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 1: data.tbtRatioLimit2C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 2: data.tbtRatioLimit3C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 3: data.tbtRatioLimit4C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 4: data.tbtRatioLimit5C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 5: data.tbtRatioLimit6C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 6: data.tbtRatioLimit7C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 7: data.tbtRatioLimit8C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 8: data.tbtRatioLimit9C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 9: data.tbtRatioLimit10C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 10: data.tbtRatioLimit11C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 11: data.tbtRatioLimit12C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 12: data.tbtRatioLimit13C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 13: data.tbtRatioLimit14C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 14: data.tbtRatioLimit15C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 15: data.tbtRatioLimit16C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 16: data.tbtRatioLimit17C(static_cast<uint8_t>(spinner_[i]->value())); break;
        case 17: data.tbtRatioLimit18C(static_cast<uint8_t>(spinner_[i]->value())); break;
      }
    }
  }
  else {
    data.tbtRatioLimitEnable(false);
    for (unsigned int i = 0; i < cpuInfo().cores(); ++i) {
      switch (i) {
        case 0: data.tbtRatioLimit1C(tabValues().tbtRatioLimit1C()); break;
        case 1: data.tbtRatioLimit2C(tabValues().tbtRatioLimit2C()); break;
        case 2: data.tbtRatioLimit3C(tabValues().tbtRatioLimit3C()); break;
        case 3: data.tbtRatioLimit4C(tabValues().tbtRatioLimit4C()); break;
        case 4: data.tbtRatioLimit5C(tabValues().tbtRatioLimit5C()); break;
        case 5: data.tbtRatioLimit6C(tabValues().tbtRatioLimit6C()); break;
        case 6: data.tbtRatioLimit7C(tabValues().tbtRatioLimit7C()); break;
        case 7: data.tbtRatioLimit8C(tabValues().tbtRatioLimit8C()); break;
        case 8: data.tbtRatioLimit9C(tabValues().tbtRatioLimit9C()); break;
        case 9: data.tbtRatioLimit10C(tabValues().tbtRatioLimit10C()); break;
        case 10: data.tbtRatioLimit11C(tabValues().tbtRatioLimit11C()); break;
        case 11: data.tbtRatioLimit12C(tabValues().tbtRatioLimit12C()); break;
        case 12: data.tbtRatioLimit13C(tabValues().tbtRatioLimit13C()); break;
        case 13: data.tbtRatioLimit14C(tabValues().tbtRatioLimit14C()); break;
        case 14: data.tbtRatioLimit15C(tabValues().tbtRatioLimit15C()); break;
        case 15: data.tbtRatioLimit16C(tabValues().tbtRatioLimit16C()); break;
        case 16: data.tbtRatioLimit17C(tabValues().tbtRatioLimit17C()); break;
        case 17: data.tbtRatioLimit18C(tabValues().tbtRatioLimit18C()); break;
      }
    }
  }
}

void SpeedControl::refresh()
{
  /* Exit if we are disabled */
  if (!isEnabled()) return;

  /*
   * Adjust Processors Features
   */

  IA32_MISC_ENABLE ia32_misc_enable(cpuInfo().firstLogicalCpu());

  if (ia32_misc_enable.read() == 0) {
    if (ia32_misc_enable.EIST_Enable != 0) {
      EIST_enable_current_->setText("<font color='green'>Enabled</font>");
    }
    else {
      EIST_enable_current_->setText("<font color='red'>Disabled</font>");
    }
    if (ia32_misc_enable.EIST_Select_Lock != 0) {
      EIST_lock_current_->setText("<font color='red'>Locked</font>");
    }
    else {
      EIST_lock_current_->setText("<font color='green'>Unlocked</font>");
    }
    if (ia32_misc_enable.IDA_Disable != 0) {
      IDA_disable_current_->setText("<font color='red'>Disabled</font>");
    }
    else {
      IDA_disable_current_->setText("<font color='green'>Enabled</font>");
    }
  }
  else {
    /* could not read IA32_MISC_ENABLE msr */
    EIST_enable_name_->setEnabled(false);
    EIST_enable_on_->setEnabled(false);
    EIST_enable_off_->setEnabled(false);
    EIST_enable_default_->setEnabled(false);
    EIST_enable_current_->setText("????");
    EIST_enable_current_->setEnabled(false);
    EIST_lock_name_->setEnabled(false);
    EIST_lock_on_->setEnabled(false);
    EIST_lock_default_->setEnabled(false);
    EIST_lock_current_->setText("????");
    EIST_lock_current_->setEnabled(false);
    IDA_disable_name_->setEnabled(false);
    IDA_disable_on_->setEnabled(false);
    IDA_disable_off_->setEnabled(false);
    IDA_disable_default_->setEnabled(false);
    IDA_disable_current_->setText("????");
    IDA_disable_current_->setEnabled(false);
  }

  /*
   * Adjust max. non-turbo ratio
   */

  MSR_TURBO_ACTIVATION_RATIO
      msr_turbo_activation_ratio(cpuInfo().firstLogicalCpu());

  if (cpuInfo().isHaswellOrLater() && msr_turbo_activation_ratio.read() == 0) {
    std::stringstream ss;
    if (msr_turbo_activation_ratio.TURBO_ACTIVATION_RATIO_Lock != 0) {
      ss << "<font color='red'>Locked</font> @ ";
    }
    else {
      ss << "<font color='green'>Unlocked</font> @ ";
    }
    ss << msr_turbo_activation_ratio.MAX_NON_TURBO_RATIO;
    Max_nonturbo_ratio_current_->setText(QString::fromStdString(ss.str()));
  }
  else {
    /* Earlier then Haswell (or an Atom processor),
     * or failed to read the MSR_TURBO_ACTIVATION_RATIO msr. */
    Max_nonturbo_ratio_lock_name_->setEnabled(false);
    Max_nonturbo_ratio_lock_on_->setEnabled(false);
    Max_nonturbo_ratio_lock_off_->setEnabled(false);
    Max_nonturbo_ratio_lock_default_->setEnabled(false);
    Max_nonturbo_ratio_current_->setEnabled(false);
    Max_nonturbo_ratio_current_->setText("????");
    Max_nonturbo_ratio_enable_->setEnabled(false);
    Max_nonturbo_ratio_->setEnabled(false);
  }

  /*
   * Adjust Turbo Boost Ratio Limits
   */

  MSR_PLATFORM_INFO msr_platform_info(cpuInfo().firstLogicalCpu());

  if (cpuInfo().isHaswellOrLater() && msr_platform_info.read() == 0) {

    uint64_t minimum_ratio = msr_platform_info.Maximum_Efficiency_Ratio;
    if (cpuInfo().isHaswell()) {
      minimum_ratio = msr_platform_info.Minimum_Operating_Ratio;
    }

    size_t ncore = (cpuInfo().cores() > 18) ? 18 : cpuInfo().cores();

    // 0 = no semaphore, 2 = MSR_TURBO_RATIO_LIMIT2, 3 = MSR_TURBO_RATIO_LIMIT3
    int use_ratio_semaphore = 0;

    MSR_TURBO_RATIO_LIMIT msr_turbo_ratio_limit(cpuInfo().firstLogicalCpu());
    MSR_TURBO_RATIO_LIMIT1 msr_turbo_ratio_limit1(cpuInfo().firstLogicalCpu());
    MSR_TURBO_RATIO_LIMIT2 msr_turbo_ratio_limit2(cpuInfo().firstLogicalCpu());
    MSR_TURBO_RATIO_LIMIT3 msr_turbo_ratio_limit3(cpuInfo().firstLogicalCpu());

    /* Always read core 1 - 8 ratio msr */
    msr_turbo_ratio_limit.read();

    /* Read core 9 - 16 ratio msr if we have that many cores */
    if (ncore > 8) msr_turbo_ratio_limit1.read();

    /* Always read core 17+18 ratio msr, if we can read then use its semaphore bit */
    if (msr_turbo_ratio_limit2.read() == 0) use_ratio_semaphore = 2;

    /* Only for family/model 06_56H and 06_4FH (Broadwell) */
    if (cpuInfo().model() == 0x56 || cpuInfo().model() == 0x4F) {
      if (msr_turbo_ratio_limit3.read() == 0) use_ratio_semaphore = 3;
    }

    if (use_ratio_semaphore) {
      int s = 0;
      if (use_ratio_semaphore == 2) {
        s = msr_turbo_ratio_limit2.Ratio_Limit_Semaphore;
      }
      else {
        s = msr_turbo_ratio_limit3.Ratio_Limit_Semaphore;
      }
      if (s) {
        enable_ratio_limit_current_->setText("<font color='green'>Enabled</font>");
      }
      else {
        enable_ratio_limit_current_->setText("<font color='red'>Disabled</font>");
      }
    }
    else {
      enable_ratio_limit_current_->setText("");
    }

    for (size_t i = 1; i <= ncore; ++i) {
      label_[i - 1]->setEnabled(true);
      spinner_[i - 1]->setEnabled(true);
      spinner_[i - 1]->setRange(static_cast<int>(minimum_ratio), 255);
      current_[i - 1]->show();
      arrow_[i - 1]->show();
      note_[i - 1]->show();

      /* core 1 - 8 */
      if (i < 9) {
        std::stringstream ss;
        switch (i) {
          case 1:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit.Ratio_Limit_1C));
            ss << (msr_turbo_ratio_limit.Ratio_Limit_1C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 2:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit.Ratio_Limit_2C));
            ss << (msr_turbo_ratio_limit.Ratio_Limit_2C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 3:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit.Ratio_Limit_3C));
            ss << (msr_turbo_ratio_limit.Ratio_Limit_3C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 4:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit.Ratio_Limit_4C));
            ss << (msr_turbo_ratio_limit.Ratio_Limit_4C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 5:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit.Ratio_Limit_5C));
            ss << (msr_turbo_ratio_limit.Ratio_Limit_5C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 6:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit.Ratio_Limit_6C));
            ss << (msr_turbo_ratio_limit.Ratio_Limit_6C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 7:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit.Ratio_Limit_7C));
            ss << (msr_turbo_ratio_limit.Ratio_Limit_7C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 8:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit.Ratio_Limit_8C));
            ss << (msr_turbo_ratio_limit.Ratio_Limit_8C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
        }
      }
      /* core 9 - 16 */
      if (i > 8 && i < 17) {
        std::stringstream ss;
        switch (i) {
          case 9:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit1.Ratio_Limit_9C));
            ss << (msr_turbo_ratio_limit1.Ratio_Limit_9C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 10:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit1.Ratio_Limit_10C));
            ss << (msr_turbo_ratio_limit1.Ratio_Limit_10C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 11:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit1.Ratio_Limit_11C));
            ss << (msr_turbo_ratio_limit1.Ratio_Limit_11C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 12:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit1.Ratio_Limit_12C));
            ss << (msr_turbo_ratio_limit1.Ratio_Limit_12C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 13:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit1.Ratio_Limit_13C));
            ss << (msr_turbo_ratio_limit1.Ratio_Limit_13C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 14:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit1.Ratio_Limit_14C));
            ss << (msr_turbo_ratio_limit1.Ratio_Limit_14C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 15:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit1.Ratio_Limit_15C));
            ss << (msr_turbo_ratio_limit1.Ratio_Limit_15C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 16:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit1.Ratio_Limit_16C));
            ss << (msr_turbo_ratio_limit1.Ratio_Limit_16C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
        }
      }
      /* core 17 - 18 */
      if (i > 16 && i < 19) {
        std::stringstream ss;
        switch (i) {
          case 17:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit2.Ratio_Limit_17C));
            ss << (msr_turbo_ratio_limit2.Ratio_Limit_17C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
          case 18:
            current_[i - 1]->setText(QString::number(msr_turbo_ratio_limit2.Ratio_Limit_18C));
            ss << (msr_turbo_ratio_limit2.Ratio_Limit_18C * 100) << " MHz";
            note_[i - 1]->setText(QString::fromStdString(ss.str()));
            break;
        }
      }
    }

  }
  else {
    /* Earlier then Haswell (or an Atom processor),
     * or failed to read the MSR_PLATFORM_INFO msr. */
  }
}


bool SpeedControl::apply() {
  /* Exit if we are disabled */
  if (!isEnabled()) return true;

  bool doRun = false;

  /* Assemble the command to execute. */
  std::vector<std::string> cmd {
    TabSettings::ScriptPath, "-v",
    "-p", std::to_string(cpuInfo().physicalId().value)
  };

  /* EIST on/off */
  if (tabSettings().eistEnableEnabled()) {
    cmd.emplace_back((tabSettings().eistEnable()) ? "--eist-enable" : "--eist-disable");
    doRun = true;
  }

  /* EIST Lock on/off */
  if (tabSettings().eistLockEnabled()) {
    if (tabSettings().eistLock()) {
      cmd.emplace_back("--eist-lock");
      doRun = true;
    }
  }

  /* TBT on/off */
  if (tabSettings().tbtDisableEnabled()) {
    cmd.emplace_back((tabSettings().tbtDisable()) ? "--tbt-disable" : "--tbt-enable");
    doRun = true;
  }

  /* Adjust max. non-turbo ratio */
  if (tabSettings().tbtActivationRatioEnabled()) {
    cmd.emplace_back("--tbt-activation-ratio");
    cmd.emplace_back(std::to_string(tabSettings().tbtActivationRatio()));
    doRun = true;
  }

  /* Lock max. non-turbo ratio */
  if (tabSettings().tbtActivationRatioLockEnabled()) {
    if (tabSettings().tbtActivationRatioLock()) {
      cmd.emplace_back("--tbt-activation-ratio-lock");
      doRun = true;
    }
  }

  /* Adjust Ratio Limits -- MSR_TURBO_RATIO_LIMIT*/
  if (tabSettings().tbtRatioLimitEnable()) {
    for (unsigned int core = 1; core <= cpuInfo().cores(); ++core) {
      switch (core) {
        case 1:
          cmd.emplace_back("-1c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit1C()));
          break;
        case 2:
          cmd.emplace_back("-2c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit2C()));
          break;
        case 3:
          cmd.emplace_back("-3c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit3C()));
          break;
        case 4:
          cmd.emplace_back("-4c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit4C()));
          break;
        case 5:
          cmd.emplace_back("-5c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit5C()));
          break;
        case 6:
          cmd.emplace_back("-6c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit6C()));
          break;
        case 7:
          cmd.emplace_back("-7c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit7C()));
          break;
        case 8:
          cmd.emplace_back("-8c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit8C()));
          break;
        case 9:
          cmd.emplace_back("-9c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit9C()));
          break;
        case 10:
          cmd.emplace_back("-10c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit10C()));
          break;
        case 11:
          cmd.emplace_back("-11c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit11C()));
          break;
        case 12:
          cmd.emplace_back("-12c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit12C()));
          break;
        case 13:
          cmd.emplace_back("-13c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit13C()));
          break;
        case 14:
          cmd.emplace_back("-14c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit14C()));
          break;
        case 15:
          cmd.emplace_back("-15c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit15C()));
          break;
        case 16:
          cmd.emplace_back("-16c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit16C()));
          break;
        case 17:
          cmd.emplace_back("-17c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit17C()));
          break;
        case 18:
          cmd.emplace_back("-18c");
          cmd.emplace_back(std::to_string(tabSettings().tbtRatioLimit18C()));
          break;
      }
    }
    doRun = true;
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

bool SpeedControl::compare()
{
  /* Store the current UI values to a temporary object */
  Settings data(tabSettings());
  store(data);

  /* Compare the relevant values in the temporary object against the
   * global TabMemberSettings instance and return false if they differ. */

  /* EIST on/off */
  if (data.eistEnableEnabled() != tabSettings().eistEnableEnabled()) {
    DBGMSG("SpeedControl::Settings::eistEnableEnabled() does not match")
    return false;
  }
  if (data.eistEnable() != tabSettings().eistEnable()) {
    DBGMSG("SpeedControl::Settings::eistEnable() does not match")
    return false;
  }

  /* EIST Lock on/off */
  if (data.eistLockEnabled() != tabSettings().eistLockEnabled()) {
    DBGMSG("SpeedControl::Settings::eistLockEnabled() does not match")
    return false;
  }
  if (data.eistLock() != tabSettings().eistLock()) {
    DBGMSG("SpeedControl::Settings::eistLock() does not match")
    return false;
  }

  /* TBT on/off */
  if (data.tbtDisableEnabled() != tabSettings().tbtDisableEnabled()) {
    DBGMSG("SpeedControl::Settings::tbtDisableEnabled() does not match")
    return false;
  }
  if (data.tbtDisable() != tabSettings().tbtDisable()) {
    DBGMSG("SpeedControl::Settings::tbtDisable() does not match")
    return false;
  }

  /* Adjust max. non-turbo ratio */
  if (data.tbtActivationRatioEnabled() != tabSettings().tbtActivationRatioEnabled()) {
    DBGMSG("SpeedControl::Settings::tbtActivationRatioEnabled() does not match")
    return false;
  }
  if (data.tbtActivationRatio() != tabSettings().tbtActivationRatio()) {
    DBGMSG("SpeedControl::Settings::tbtActivationRatio() does not match")
    return false;
  }

  /* Lock max. non-turbo ratio */
  if (data.tbtActivationRatioLockEnabled() != tabSettings().tbtActivationRatioLockEnabled()) {
    DBGMSG("SpeedControl::Settings::tbtActivationRatioLockEnabled() does not match")
    return false;
  }
  if (data.tbtActivationRatioLock() != tabSettings().tbtActivationRatioLock()) {
    DBGMSG("SpeedControl::Settings::tbtActivationRatioLock() does not match")
    return false;
  }

  /* Adjust Ratio Limits + */
  /* Adjust Ratio Limits Enabled */
  if (data.tbtRatioLimitEnable() != tabSettings().tbtRatioLimitEnable()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimitEnable() does not match")
    return false;
  }
  if (data.tbtRatioLimit1C() != tabSettings().tbtRatioLimit1C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit1C() does not match")
    return false;
  }
  if (data.tbtRatioLimit2C() != tabSettings().tbtRatioLimit2C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit2C() does not match")
    return false;
  }
  if (data.tbtRatioLimit3C() != tabSettings().tbtRatioLimit3C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit3C() does not match")
    return false;
  }
  if (data.tbtRatioLimit4C() != tabSettings().tbtRatioLimit4C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit4C() does not match")
    return false;
  }
  if (data.tbtRatioLimit5C() != tabSettings().tbtRatioLimit5C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit5C() does not match")
    return false;
  }
  if (data.tbtRatioLimit6C() != tabSettings().tbtRatioLimit6C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit6C() does not match")
    return false;
  }
  if (data.tbtRatioLimit7C() != tabSettings().tbtRatioLimit7C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit7C() does not match")
    return false;
  }
  if (data.tbtRatioLimit8C() != tabSettings().tbtRatioLimit8C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit8C() does not match")
    return false;
  }
  if (data.tbtRatioLimit9C() != tabSettings().tbtRatioLimit9C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit9C() does not match")
    return false;
  }
  if (data.tbtRatioLimit10C() != tabSettings().tbtRatioLimit10C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit10C() does not match")
    return false;
  }
  if (data.tbtRatioLimit11C() != tabSettings().tbtRatioLimit11C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit11C() does not match")
    return false;
  }
  if (data.tbtRatioLimit12C() != tabSettings().tbtRatioLimit12C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit12C() does not match")
    return false;
  }
  if (data.tbtRatioLimit13C() != tabSettings().tbtRatioLimit13C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit13C() does not match")
    return false;
  }
  if (data.tbtRatioLimit14C() != tabSettings().tbtRatioLimit14C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit14C() does not match")
    return false;
  }
  if (data.tbtRatioLimit15C() != tabSettings().tbtRatioLimit15C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit15C() does not match")
    return false;
  }
  if (data.tbtRatioLimit16C() != tabSettings().tbtRatioLimit16C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit16C() does not match")
    return false;
  }
  if (data.tbtRatioLimit17C() != tabSettings().tbtRatioLimit17C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit17C() does not match")
    return false;
  }
  if (data.tbtRatioLimit18C() != tabSettings().tbtRatioLimit18C()) {
    DBGMSG("SpeedControl::Settings::tbtRatioLimit18C() does not match")
    return false;
  }

  DBGMSG("SpeedControl::compare(): Settings match!")
  return true;
}

/* SLOTS */

void SpeedControl::toggledSlot(int, bool checked) {
  if (checked) {
    emit valueChanged(this);
  }
}

void SpeedControl::Max_nonturbo_ratio_enable_changed(int state) {
  Max_nonturbo_ratio_->setEnabled((state) ? true : false);
  emit valueChanged(this);
}

void SpeedControl::Max_nonturbo_ratio_changed(int) {
  emit valueChanged(this);
}

void SpeedControl::enable_ratio_limit_changed(int state) {
  tabs_ratio_limits_->setEnabled((state) ? true : false);
  emit valueChanged(this);
}

void SpeedControl::spinner_changed(int) {
  emit valueChanged(this);
}

/*
 * SpeedControl::Settings
 */

void SpeedControl::Settings::load(
    QSettings& qs, const TabMemberValues& cv) {

  /* select the INI section for this processor */
  std::stringstream ss;
  ss << TabSettings::INI_GRP_PROCESSOR << cpuInfo().physicalId().value;
  qs.beginGroup(ss.str().c_str());

  /* Load the settings or set a default value: */

  eistEnableEnabled(qs.value(INI_EIST_ENABLE_ENABLE, false).toBool());
  eistEnable(qs.value(INI_EIST_ENABLE, cv.eistEnable()).toBool());

  eistLockEnabled(qs.value(INI_EIST_LOCK_ENABLE, false).toBool());
  eistLock(qs.value(INI_EIST_LOCK, cv.eistLock()).toBool());

  tbtDisableEnabled(qs.value(INI_TBT_DISABLE_ENABLE, false).toBool());
  tbtDisable(qs.value(INI_TBT_DISABLE, cv.tbtDisable()).toBool());

  tbtActivationRatioEnabled(qs.value(INI_TURBO_ACTIVATION_RATIO_ENABLE, false).toBool());
  tbtActivationRatio(static_cast<uint8_t>(qs.value(INI_TURBO_ACTIVATION_RATIO, cv.tbtActivationRatio()).toUInt()));

  tbtActivationRatioLockEnabled(qs.value(INI_TURBO_ACTIVATION_RATIO_LOCK_ENABLE, false).toBool());
  tbtActivationRatioLock(qs.value(INI_TURBO_ACTIVATION_RATIO_LOCK, cv.tbtActivationRatioLock()).toBool());

  tbtRatioLimit1C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_1C, cv.tbtRatioLimit1C()).toUInt()));
  tbtRatioLimit2C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_2C, cv.tbtRatioLimit2C()).toUInt()));
  tbtRatioLimit3C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_3C, cv.tbtRatioLimit3C()).toUInt()));
  tbtRatioLimit4C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_4C, cv.tbtRatioLimit4C()).toUInt()));
  tbtRatioLimit6C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_6C, cv.tbtRatioLimit6C()).toUInt()));
  tbtRatioLimit7C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_7C, cv.tbtRatioLimit7C()).toUInt()));
  tbtRatioLimit8C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_8C, cv.tbtRatioLimit8C()).toUInt()));
  tbtRatioLimit9C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_9C, cv.tbtRatioLimit9C()).toUInt()));
  tbtRatioLimit10C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_10C, cv.tbtRatioLimit10C()).toUInt()));
  tbtRatioLimit11C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_11C, cv.tbtRatioLimit11C()).toUInt()));
  tbtRatioLimit12C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_12C, cv.tbtRatioLimit12C()).toUInt()));
  tbtRatioLimit13C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_13C, cv.tbtRatioLimit13C()).toUInt()));
  tbtRatioLimit14C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_14C, cv.tbtRatioLimit14C()).toUInt()));
  tbtRatioLimit15C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_15C, cv.tbtRatioLimit15C()).toUInt()));
  tbtRatioLimit16C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_16C, cv.tbtRatioLimit16C()).toUInt()));
  tbtRatioLimit17C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_17C, cv.tbtRatioLimit17C()).toUInt()));
  tbtRatioLimit18C(static_cast<uint8_t>(qs.value(INI_TURBO_RATIO_LIMIT_18C, cv.tbtRatioLimit18C()).toUInt()));

  tbtRatioLimitEnable(qs.value(INI_TURBO_RATIO_LIMIT_ENABLE, false).toBool());

  qs.endGroup();

#if 0
  DBGMSG("SpeedControl::Settings::eistEnableEnabled()                 <--" << eistEnableEnabled())
  DBGMSG("SpeedControl::Settings::eistEnable()                        <--" << eistEnable())
  DBGMSG("SpeedControl::Settings::eistLockEnabled()                   <--" << eistLockEnabled())
  DBGMSG("SpeedControl::Settings::eistLock()                          <--" << eistLock())
  DBGMSG("SpeedControl::Settings::tbtDisableEnabled()                 <--" << tbtDisableEnabled())
  DBGMSG("SpeedControl::Settings::tbtDisable()                        <--" << tbtDisable())
  DBGMSG("SpeedControl::Settings::tbtActivationRatioEnabled()         <--" << tbtActivationRatioEnabled())
  DBGMSG("SpeedControl::Settings::tbtActivationRatio()                <--" << tbtActivationRatio())
  DBGMSG("SpeedControl::Settings::tbtActivationRatioLockEnabled()     <--" << tbtActivationRatioLockEnabled())
  DBGMSG("SpeedControl::Settings::tbtActivationRatioLock()            <--" << tbtActivationRatioLock())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit1C()                   <--" << tbtRatioLimit1C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit2C()                   <--" << tbtRatioLimit2C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit3C()                   <--" << tbtRatioLimit3C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit4C()                   <--" << tbtRatioLimit4C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit5C()                   <--" << tbtRatioLimit5C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit6C()                   <--" << tbtRatioLimit6C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit7C()                   <--" << tbtRatioLimit7C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit8C()                   <--" << tbtRatioLimit8C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit9C()                   <--" << tbtRatioLimit9C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit10C()                  <--" << tbtRatioLimit10C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit11C()                  <--" << tbtRatioLimit11C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit12C()                  <--" << tbtRatioLimit12C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit13C()                  <--" << tbtRatioLimit13C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit14C()                  <--" << tbtRatioLimit14C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit15C()                  <--" << tbtRatioLimit15C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit16C()                  <--" << tbtRatioLimit16C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit17C()                  <--" << tbtRatioLimit17C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit18C()                  <--" << tbtRatioLimit18C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimitEnable()               <--" << tbtRatioLimitEnable())
#endif
}

void SpeedControl::Settings::save(QSettings& qs) {

  /* select the INI section for this processor */
  std::stringstream ss;
  ss << TabSettings::INI_GRP_PROCESSOR
     << tabMemberSettings().processorNumber().value;
  qs.beginGroup(ss.str().c_str());

  /* Save the settings: */

  qs.setValue(INI_EIST_ENABLE_ENABLE, QVariant::fromValue<bool>(eistEnableEnabled()));
  qs.setValue(INI_EIST_ENABLE, QVariant::fromValue<bool>(eistEnable()));

  qs.setValue(INI_EIST_LOCK_ENABLE, QVariant::fromValue<bool>(eistLockEnabled()));
  qs.setValue(INI_EIST_LOCK, QVariant::fromValue<bool>(eistLock()));

  qs.setValue(INI_TBT_DISABLE_ENABLE, QVariant::fromValue<bool>(tbtDisableEnabled()));
  qs.setValue(INI_TBT_DISABLE, QVariant::fromValue<bool>(tbtDisable()));

  qs.setValue(INI_TURBO_ACTIVATION_RATIO_ENABLE, QVariant::fromValue<bool>(tbtActivationRatioEnabled()));
  qs.setValue(INI_TURBO_ACTIVATION_RATIO, QVariant::fromValue<unsigned int>(tbtActivationRatio()));

  qs.setValue(INI_TURBO_ACTIVATION_RATIO_LOCK_ENABLE, QVariant::fromValue<bool>(tbtActivationRatioLockEnabled()));
  qs.setValue(INI_TURBO_ACTIVATION_RATIO_LOCK, QVariant::fromValue<bool>(tbtActivationRatioLock()));

  qs.setValue(INI_TURBO_RATIO_LIMIT_1C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit1C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_2C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit2C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_3C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit3C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_4C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit4C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_5C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit5C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_6C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit6C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_7C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit7C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_8C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit8C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_9C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit9C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_10C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit10C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_11C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit11C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_12C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit12C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_13C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit13C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_14C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit14C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_15C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit15C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_16C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit16C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_17C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit17C()));
  qs.setValue(INI_TURBO_RATIO_LIMIT_18C,
      QVariant::fromValue<unsigned int>(tbtRatioLimit18C()));

  qs.setValue(INI_TURBO_RATIO_LIMIT_ENABLE,
      QVariant::fromValue<bool>(tbtRatioLimitEnable()));

  qs.endGroup();
#if 0
  DBGMSG("SpeedControl::Settings::eistEnableEnabled()                 -->" << eistEnableEnabled())
  DBGMSG("SpeedControl::Settings::eistEnable()                        -->" << eistEnable())
  DBGMSG("SpeedControl::Settings::eistLockEnabled()                   -->" << eistLockEnabled())
  DBGMSG("SpeedControl::Settings::eistLock()                          -->" << eistLock())
  DBGMSG("SpeedControl::Settings::tbtDisableEnabled()                 -->" << tbtDisableEnabled())
  DBGMSG("SpeedControl::Settings::tbtDisable()                        -->" << tbtDisable())
  DBGMSG("SpeedControl::Settings::tbtActivationRatioEnabled()         -->" << tbtActivationRatioEnabled())
  DBGMSG("SpeedControl::Settings::tbtActivationRatio()                -->" << tbtActivationRatio())
  DBGMSG("SpeedControl::Settings::tbtActivationRatioLockEnabled()     -->" << tbtActivationRatioLockEnabled())
  DBGMSG("SpeedControl::Settings::tbtActivationRatioLock()            -->" << tbtActivationRatioLock())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit1C()                   -->" << tbtRatioLimit1C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit2C()                   -->" << tbtRatioLimit2C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit3C()                   -->" << tbtRatioLimit3C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit4C()                   -->" << tbtRatioLimit4C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit5C()                   -->" << tbtRatioLimit5C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit6C()                   -->" << tbtRatioLimit6C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit7C()                   -->" << tbtRatioLimit7C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit8C()                   -->" << tbtRatioLimit8C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit9C()                   -->" << tbtRatioLimit9C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit10C()                  -->" << tbtRatioLimit10C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit11C()                  -->" << tbtRatioLimit11C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit12C()                  -->" << tbtRatioLimit12C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit13C()                  -->" << tbtRatioLimit13C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit14C()                  -->" << tbtRatioLimit14C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit15C()                  -->" << tbtRatioLimit15C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit16C()                  -->" << tbtRatioLimit16C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit17C()                  -->" << tbtRatioLimit17C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimit18C()                  -->" << tbtRatioLimit18C())
  DBGMSG("SpeedControl::Settings::tbtRatioLimitEnable()               -->" << tbtRatioLimitEnable())
#endif
}

