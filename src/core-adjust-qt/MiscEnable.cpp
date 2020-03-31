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

/** @class MiscEnable
  * @brief TabMemberWidget to adjust IA32_MISC_ENABLE bits.
  *
  */
#include <QGridLayout>
#include <QMessageBox>
#include "MiscEnable.hpp"
#include "Msr.hpp"
#include "TabMember.hpp"

MiscEnable::MiscEnable(
    const SingleCpuInfo& info, const SingleCpuId& id, TabMemberValues& values,
    TabMemberSettings& data, QWidget* parent)
    : TabMemberTemplate(info, id, values, data, parent),
      shell_(this, "Adjust IA32_MISC_ENABLE",
          ShellCommand::EnableStd | ShellCommand::DisableAbortButton) {

  auto* grid = new QGridLayout();
  grid->setColumnStretch(0, 1);
  grid->setColumnStretch(4, 1);

  /* row0: Fast-Strings Enable on/off */
  fs_enable_name_ = new QLabel(tr("Fast-Strings"));
  fs_enable_on_ = new QRadioButton(tr("On"));
  fs_enable_off_ = new QRadioButton(tr("Off"));
  fs_enable_default_ = new QRadioButton(tr("Do not adjust"));
  fs_enable_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 0</nobr><br/>"
      "<nobr>Enable or disable 'Fast-Strings'."
      "</nobr></p>"));
  fs_enable_on_->setToolTip(tr("This option will set the bit to 1."));
  fs_enable_off_->setToolTip(tr("This option will reset the bit to 0."));
  fs_enable_default_->setToolTip(tr("This option will leave the bit as is."));
  fs_enable_current_ = new QLabel(tr("Not Supported"));
  fs_enable_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  fs_enable_grp_ = new QButtonGroup();
  fs_enable_grp_->addButton(fs_enable_on_, 1);
  fs_enable_grp_->addButton(fs_enable_off_, 2);
  fs_enable_grp_->addButton(fs_enable_default_, 3);
  grid->addWidget(fs_enable_name_, 0, 0);
  grid->addWidget(fs_enable_on_, 0, 1);
  grid->addWidget(fs_enable_off_, 0, 2);
  grid->addWidget(fs_enable_default_, 0, 3);
  grid->addWidget(fs_enable_current_, 0, 5);

  /* row1: Hardware Prefetcher Disable on/off */
  hwp_disable_name_ = new QLabel(tr("Hardware Prefetcher"));
  hwp_disable_on_ = new QRadioButton(tr("Off"));
  hwp_disable_off_ = new QRadioButton(tr("On"));
  hwp_disable_default_ = new QRadioButton(tr("Do not adjust"));
  hwp_disable_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 9</nobr><br/>"
      "<nobr>Enable or disable 'Hardware Prefetcher'."
      "</nobr></p>"));
  hwp_disable_on_->setToolTip(tr("This option will set the bit to 1."));
  hwp_disable_off_->setToolTip(tr("This option will reset the bit to 0."));
  hwp_disable_default_->setToolTip(tr("This option will leave the bit as is."));
  hwp_disable_current_ = new QLabel(tr("Not Supported"));
  hwp_disable_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  hwp_disable_grp_ = new QButtonGroup();
  hwp_disable_grp_->addButton(hwp_disable_on_, 1);
  hwp_disable_grp_->addButton(hwp_disable_off_, 2);
  hwp_disable_grp_->addButton(hwp_disable_default_, 3);
  grid->addWidget(hwp_disable_name_, 1, 0);
  grid->addWidget(hwp_disable_on_, 1, 2);
  grid->addWidget(hwp_disable_off_, 1, 1);
  grid->addWidget(hwp_disable_default_, 1, 3);
  grid->addWidget(hwp_disable_current_, 1, 5);

  /* row2: FERR# Multiplexing Enable on/off */
  ferr_enable_name_ = new QLabel(tr("FERR# Multiplexing"));
  ferr_enable_on_ = new QRadioButton(tr("On"));
  ferr_enable_off_ = new QRadioButton(tr("Off"));
  ferr_enable_default_ = new QRadioButton(tr("Do not adjust"));
  ferr_enable_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 10</nobr><br/>"
      "<nobr>Enable or disable 'FERR# Multiplexing'."
      "</nobr></p>"));
  ferr_enable_on_->setToolTip(tr("This option will set the bit to 1."));
  ferr_enable_off_->setToolTip(tr("This option will reset the bit to 0."));
  ferr_enable_default_->setToolTip(tr("This option will leave the bit as is."));
  ferr_enable_current_ = new QLabel(tr("Not Supported"));
  ferr_enable_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  ferr_enable_grp_ = new QButtonGroup();
  ferr_enable_grp_->addButton(ferr_enable_on_, 1);
  ferr_enable_grp_->addButton(ferr_enable_off_, 2);
  ferr_enable_grp_->addButton(ferr_enable_default_, 3);
  grid->addWidget(ferr_enable_name_, 2, 0);
  grid->addWidget(ferr_enable_on_, 2, 1);
  grid->addWidget(ferr_enable_off_, 2, 2);
  grid->addWidget(ferr_enable_default_, 2, 3);
  grid->addWidget(ferr_enable_current_, 2, 5);

  /* row3: ENABLE MONITOR FSM on/off */
  fsm_enable_name_ = new QLabel(tr("MONITOR FSM"));
  fsm_enable_on_ = new QRadioButton(tr("On"));
  fsm_enable_off_ = new QRadioButton(tr("Off"));
  fsm_enable_default_ = new QRadioButton(tr("Do not adjust"));
  fsm_enable_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 18</nobr><br/>"
      "<nobr>Enable or disable 'MONITOR FSM'."
      "</nobr></p>"));
  fsm_enable_on_->setToolTip(tr("This option will set the bit to 1."));
  fsm_enable_off_->setToolTip(tr("This option will reset the bit to 0."));
  fsm_enable_default_->setToolTip(tr("This option will leave the bit as is."));
  fsm_enable_current_ = new QLabel(tr("Not Supported"));
  fsm_enable_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  fsm_enable_grp_ = new QButtonGroup();
  fsm_enable_grp_->addButton(fsm_enable_on_, 1);
  fsm_enable_grp_->addButton(fsm_enable_off_, 2);
  fsm_enable_grp_->addButton(fsm_enable_default_, 3);
  grid->addWidget(fsm_enable_name_, 3, 0);
  grid->addWidget(fsm_enable_on_, 3, 1);
  grid->addWidget(fsm_enable_off_, 3, 2);
  grid->addWidget(fsm_enable_default_, 3, 3);
  grid->addWidget(fsm_enable_current_, 3, 5);

  /* row4: Adjacent Cache Line Prefetch Disable on/off */
  adj_disable_name_ = new QLabel(tr("Adjacent Cache Line Prefetch"));
  adj_disable_on_ = new QRadioButton(tr("Off"));
  adj_disable_off_ = new QRadioButton(tr("On"));
  adj_disable_default_ = new QRadioButton(tr("Do not adjust"));
  adj_disable_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 19</nobr><br/>"
      "<nobr>Enable or disable 'Adjacent Cache Line Prefetch'."
      "</nobr></p>"));
  adj_disable_on_->setToolTip(tr("This option will set the bit to 1."));
  adj_disable_off_->setToolTip(tr("This option will reset the bit to 0."));
  adj_disable_default_->setToolTip(tr("This option will leave the bit as is."));
  adj_disable_current_ = new QLabel(tr("Not Supported"));
  adj_disable_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  adj_disable_grp_ = new QButtonGroup();
  adj_disable_grp_->addButton(adj_disable_on_, 1);
  adj_disable_grp_->addButton(adj_disable_off_, 2);
  adj_disable_grp_->addButton(adj_disable_default_, 3);
  grid->addWidget(adj_disable_name_, 4, 0);
  grid->addWidget(adj_disable_on_, 4, 2);
  grid->addWidget(adj_disable_off_, 4, 1);
  grid->addWidget(adj_disable_default_, 4, 3);
  grid->addWidget(adj_disable_current_, 4, 5);

  /* row5: Limit CPUID Maxval on/off */
  cpuid_enable_name_ = new QLabel(tr("Limit CPUID Maxval"));
  cpuid_enable_on_ = new QRadioButton(tr("On"));
  cpuid_enable_off_ = new QRadioButton(tr("Off"));
  cpuid_enable_default_ = new QRadioButton(tr("Do not adjust"));
  cpuid_enable_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 22</nobr><br/>"
      "<nobr>Enable or disable 'Limit CPUID Maxval'."
      "</nobr></p>"));
  cpuid_enable_on_->setToolTip(tr("This option will set the bit to 1."));
  cpuid_enable_off_->setToolTip(tr("This option will reset the bit to 0."));
  cpuid_enable_default_->setToolTip(tr("This option will leave the bit as is."));
  cpuid_enable_current_ = new QLabel(tr("Not Supported"));
  cpuid_enable_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  cpuid_enable_grp_ = new QButtonGroup();
  cpuid_enable_grp_->addButton(cpuid_enable_on_, 1);
  cpuid_enable_grp_->addButton(cpuid_enable_off_, 2);
  cpuid_enable_grp_->addButton(cpuid_enable_default_, 3);
  grid->addWidget(cpuid_enable_name_, 5, 0);
  grid->addWidget(cpuid_enable_on_, 5, 1);
  grid->addWidget(cpuid_enable_off_, 5, 2);
  grid->addWidget(cpuid_enable_default_, 5, 3);
  grid->addWidget(cpuid_enable_current_, 5, 5);

  /* row6: xTPR Message Disable on/off */
  xtpr_disable_name_ = new QLabel(tr("xTPR Message"));
  xtpr_disable_on_ = new QRadioButton(tr("Off"));
  xtpr_disable_off_ = new QRadioButton(tr("On"));
  xtpr_disable_default_ = new QRadioButton(tr("Do not adjust"));
  xtpr_disable_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 23</nobr><br/>"
      "<nobr>Enable or disable 'xTPR Message'."
      "</nobr></p>"));
  xtpr_disable_on_->setToolTip(tr("This option will set the bit to 1."));
  xtpr_disable_off_->setToolTip(tr("This option will reset the bit to 0."));
  xtpr_disable_default_->setToolTip(tr("This option will leave the bit as is."));
  xtpr_disable_current_ = new QLabel(tr("Not Supported"));
  xtpr_disable_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  xtpr_disable_grp_ = new QButtonGroup();
  xtpr_disable_grp_->addButton(xtpr_disable_on_, 1);
  xtpr_disable_grp_->addButton(xtpr_disable_off_, 2);
  xtpr_disable_grp_->addButton(xtpr_disable_default_, 3);
  grid->addWidget(xtpr_disable_name_, 6, 0);
  grid->addWidget(xtpr_disable_on_, 6, 2);
  grid->addWidget(xtpr_disable_off_, 6, 1);
  grid->addWidget(xtpr_disable_default_, 6, 3);
  grid->addWidget(xtpr_disable_current_, 6, 5);

  /* row7: XD Bit Disable on/off */
  xd_disable_name_ = new QLabel(tr("XD Bit"));
  xd_disable_on_ = new QRadioButton(tr("Off"));
  xd_disable_off_ = new QRadioButton(tr("On"));
  xd_disable_default_ = new QRadioButton(tr("Do not adjust"));
  xd_disable_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 34</nobr><br/>"
      "<nobr>Enable or disable 'XD Bit'."
      "</nobr></p>"));
  xd_disable_on_->setToolTip(tr("This option will set the bit to 1."));
  xd_disable_off_->setToolTip(tr("This option will reset the bit to 0."));
  xd_disable_default_->setToolTip(tr("This option will leave the bit as is."));
  xd_disable_current_ = new QLabel(tr("Not Supported"));
  xd_disable_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  xd_disable_grp_ = new QButtonGroup();
  xd_disable_grp_->addButton(xd_disable_on_, 1);
  xd_disable_grp_->addButton(xd_disable_off_, 2);
  xd_disable_grp_->addButton(xd_disable_default_, 3);
  grid->addWidget(xd_disable_name_, 7, 0);
  grid->addWidget(xd_disable_on_, 7, 2);
  grid->addWidget(xd_disable_off_, 7, 1);
  grid->addWidget(xd_disable_default_, 7, 3);
  grid->addWidget(xd_disable_current_, 7, 5);

  /* row8: DCU Prefetcher Disable on/off */
  dcu_disable_name_ = new QLabel(tr("DCU Prefetcher"));
  dcu_disable_on_ = new QRadioButton(tr("Off"));
  dcu_disable_off_ = new QRadioButton(tr("On"));
  dcu_disable_default_ = new QRadioButton(tr("Do not adjust"));
  dcu_disable_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 37</nobr><br/>"
      "<nobr>Enable or disable 'DCU Prefetcher'."
      "</nobr></p>"));
  dcu_disable_on_->setToolTip(tr("This option will set the bit to 1."));
  dcu_disable_off_->setToolTip(tr("This option will reset the bit to 0."));
  dcu_disable_default_->setToolTip(tr("This option will leave the bit as is."));
  dcu_disable_current_ = new QLabel(tr("Not Supported"));
  dcu_disable_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  dcu_disable_grp_ = new QButtonGroup();
  dcu_disable_grp_->addButton(dcu_disable_on_, 1);
  dcu_disable_grp_->addButton(dcu_disable_off_, 2);
  dcu_disable_grp_->addButton(dcu_disable_default_, 3);
  grid->addWidget(dcu_disable_name_, 8, 0);
  grid->addWidget(dcu_disable_on_, 8, 2);
  grid->addWidget(dcu_disable_off_, 8, 1);
  grid->addWidget(dcu_disable_default_, 8, 3);
  grid->addWidget(dcu_disable_current_, 8, 5);

  /* row9: IP Prefetcher Disable on/off */
  ipp_disable_name_ = new QLabel(tr("IP Prefetcher"));
  ipp_disable_on_ = new QRadioButton(tr("Off"));
  ipp_disable_off_ = new QRadioButton(tr("On"));
  ipp_disable_default_ = new QRadioButton(tr("Do not adjust"));
  ipp_disable_name_->setToolTip(tr(
      "<p><nobr>IA32_MISC_ENABLE bit 39</nobr><br/>"
      "<nobr>Enable or disable 'IP Prefetcher'."
      "</nobr></p>"));
  ipp_disable_on_->setToolTip(tr("This option will set the bit to 1."));
  ipp_disable_off_->setToolTip(tr("This option will reset the bit to 0."));
  ipp_disable_default_->setToolTip(tr("This option will leave the bit as is."));
  ipp_disable_current_ = new QLabel(tr("Not Supported"));
  ipp_disable_current_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  ipp_disable_grp_ = new QButtonGroup();
  ipp_disable_grp_->addButton(ipp_disable_on_, 1);
  ipp_disable_grp_->addButton(ipp_disable_off_, 2);
  ipp_disable_grp_->addButton(ipp_disable_default_, 3);
  grid->addWidget(ipp_disable_name_, 10, 0);
  grid->addWidget(ipp_disable_on_, 10, 2);
  grid->addWidget(ipp_disable_off_, 10, 1);
  grid->addWidget(ipp_disable_default_, 10, 3);
  grid->addWidget(ipp_disable_current_, 10, 5);

  auto* b = new QVBoxLayout();
//  b->addSpacing(20);
  b->addStretch(1);
  b->addLayout(grid);
  b->addStretch(1);

  /* GroupBox: processor features */
  auto* gb = new QGroupBox(tr("Adjust Processor Features"));
  gb->setLayout(b);
  gb->setFlat(true);

  /* complete layout: processor features */
  auto* vb = new QVBoxLayout();
  vb->addWidget(gb);

  /* Add the inner widgets to the layout of the scroll widget */
  scroll_layout_->addLayout(vb);

  /* Signals */

  connect(fs_enable_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(hwp_disable_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(ferr_enable_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(fsm_enable_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(adj_disable_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(cpuid_enable_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(xtpr_disable_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(xd_disable_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(dcu_disable_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(ipp_disable_grp_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(toggledSlot(int,bool)));

  connect(&shell_, SIGNAL(finished()), &shell_, SLOT(accept()));
}

bool MiscEnable::read(std::ostringstream& ss) {
  bool retv = true;

  /* Set defaults */
  tabValues().fastStringsEnable(true);
  tabValues().hwPrefetcherDisable(false);
  tabValues().ferrMultiplexingEnable(false);
  tabValues().fsmMonitorEnable(true);
  tabValues().adjCacheLinePrefetchDisable(false);
  tabValues().cpuidMaxval(false);
  tabValues().xtprMessageDisable(true);
  tabValues().xdBitDisable(false);
  tabValues().dcuPrefetcherDisable(false);
  tabValues().ipPrefetcherDisable(false);

  /*
   * Read the values for this tab from the processor
   */

  IA32_MISC_ENABLE ia32_misc_enable(cpuInfo().firstLogicalCpu());
  if (ia32_misc_enable.read()) {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, Could not read IA32_MISC_ENABLE!</b></p>");
    setEnabled(false);
  }

  tabValues().fastStringsEnable(ia32_misc_enable.Fast_Strings_Enable != 0);
  tabValues().hwPrefetcherDisable(ia32_misc_enable.Hardware_Prefetcher_Disable != 0);
  tabValues().ferrMultiplexingEnable(ia32_misc_enable.FERR_Multiplexing_Enable != 0);
  tabValues().fsmMonitorEnable(ia32_misc_enable.ENABLE_MONITOR_FSM != 0);
  tabValues().adjCacheLinePrefetchDisable(ia32_misc_enable.Adjacent_Cache_Line_Prefetch_Disable != 0);
  tabValues().cpuidMaxval(ia32_misc_enable.Limit_CPUID_Maxval != 0);
  tabValues().xtprMessageDisable(ia32_misc_enable.xTPR_Message_Disable != 0);
  tabValues().xdBitDisable(ia32_misc_enable.XD_Bit_Disable != 0);
  tabValues().dcuPrefetcherDisable(ia32_misc_enable.DCU_Prefetcher_Disable != 0);
  tabValues().ipPrefetcherDisable(ia32_misc_enable.IP_Prefetcher_Disable != 0);

  /* Exit if an error occurred when reading the MSR */
  if (!isEnabled()) return true;

  /*
   * Compare the newly read values against the desired values
   */

  if (tabSettings().fastStringsEnableEnabled()) {
    if (tabSettings().fastStringsEnable() != tabValues().fastStringsEnable()) {
      ss << "<li><nobr>'<b>Fast-Strings Enable</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().fastStringsEnable() << "</b> instead of <b>"
         << tabSettings().fastStringsEnable() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().hwPrefetcherDisableEnabled()) {
    if (tabSettings().hwPrefetcherDisable() != tabValues().hwPrefetcherDisable()) {
      ss << "<li><nobr>'<b>Hardware Prefetcher Disable</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().hwPrefetcherDisable() << "</b> instead of <b>"
         << tabSettings().hwPrefetcherDisable() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().ferrMultiplexingEnableEnabled()) {
    if (tabSettings().ferrMultiplexingEnable() != tabValues().ferrMultiplexingEnable()) {
      ss << "<li><nobr>'<b>FERR# Multiplexing Enable</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().ferrMultiplexingEnable() << "</b> instead of <b>"
         << tabSettings().ferrMultiplexingEnable() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().fsmMonitorEnableEnabled()) {
    if (tabSettings().fsmMonitorEnable() != tabValues().fsmMonitorEnable()) {
      ss << "<li><nobr>'<b>ENABLE MONITOR FSM</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().fsmMonitorEnable() << "</b> instead of <b>"
         << tabSettings().fsmMonitorEnable() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().adjCacheLinePrefetchDisableEnabled()) {
    if (tabSettings().adjCacheLinePrefetchDisable() != tabValues().adjCacheLinePrefetchDisable()) {
      ss << "<li><nobr>'<b>Adjacent Cache Line Prefetch Disable</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().adjCacheLinePrefetchDisable() << "</b> instead of <b>"
         << tabSettings().adjCacheLinePrefetchDisable() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().cpuidMaxvalEnabled()) {
    if (tabSettings().cpuidMaxval() != tabValues().cpuidMaxval()) {
      ss << "<li><nobr>'<b>Limit CPUID Maxval</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().cpuidMaxval() << "</b> instead of <b>"
         << tabSettings().cpuidMaxval() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().xtprMessageDisableEnabled()) {
    if (tabSettings().xtprMessageDisable() != tabValues().xtprMessageDisable()) {
      ss << "<li><nobr>'<b>xTPR Message Disable</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().xtprMessageDisable() << "</b> instead of <b>"
         << tabSettings().xtprMessageDisable() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().xdBitDisableEnabled()) {
    if (tabSettings().xdBitDisable() != tabValues().xdBitDisable()) {
      ss << "<li><nobr>'<b>XD Bit Disable</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().xdBitDisable() << "</b> instead of <b>"
         << tabSettings().xdBitDisable() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().dcuPrefetcherDisableEnabled()) {
    if (tabSettings().dcuPrefetcherDisable() != tabValues().dcuPrefetcherDisable()) {
      ss << "<li><nobr>'<b>DCU Prefetcher Disable</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().dcuPrefetcherDisable() << "</b> instead of <b>"
         << tabSettings().dcuPrefetcherDisable() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  if (tabSettings().ipPrefetcherDisableEnabled()) {
    if (tabSettings().ipPrefetcherDisable() != tabValues().ipPrefetcherDisable()) {
      ss << "<li><nobr>'<b>IP Prefetcher Disable</b>' for processor "
         << cpuInfo().physicalId().value << " is <b>"
         << tabValues().ipPrefetcherDisable() << "</b> instead of <b>"
         << tabSettings().ipPrefetcherDisable() << "</b>.</nobr></li>";
      retv = false;
    }
  }

  /*
   * Return false if the newly read values differ from the desired values.
   */
  return retv;
}

void MiscEnable::load() {
  /* Fast_Strings_Enable */
  fs_enable_grp_->blockSignals(true);
  if (tabSettings().fastStringsEnableEnabled()) {
    if (tabSettings().fastStringsEnable()) fs_enable_on_->setChecked(true);
    else fs_enable_off_->setChecked(true);
  }
  else {
    fs_enable_default_->setChecked(true);
  }
  fs_enable_grp_->blockSignals(false);

  /* Hardware_Prefetcher_Disable */
  hwp_disable_grp_->blockSignals(true);
  if (tabSettings().hwPrefetcherDisableEnabled()) {
    if (tabSettings().hwPrefetcherDisable()) hwp_disable_on_->setChecked(true);
    else hwp_disable_off_->setChecked(true);
  }
  else {
    hwp_disable_default_->setChecked(true);
  }
  hwp_disable_grp_->blockSignals(false);

  /* FERR_Multiplexing_Enable */
  ferr_enable_grp_->blockSignals(true);
  if (tabSettings().ferrMultiplexingEnableEnabled()) {
    if (tabSettings().ferrMultiplexingEnable()) ferr_enable_on_->setChecked(true);
    else ferr_enable_off_->setChecked(true);
  }
  else {
    ferr_enable_default_->setChecked(true);
  }
  ferr_enable_grp_->blockSignals(false);

  /* ENABLE_MONITOR_FSM */
  fsm_enable_grp_->blockSignals(true);
  if (tabSettings().fsmMonitorEnableEnabled()) {
    if (tabSettings().fsmMonitorEnable()) fsm_enable_on_->setChecked(true);
    else fsm_enable_off_->setChecked(true);
  }
  else {
    fsm_enable_default_->setChecked(true);
  }
  fsm_enable_grp_->blockSignals(false);

  /* Adjacent_Cache_Line_Prefetch_Disable */
  adj_disable_grp_->blockSignals(true);
  if (tabSettings().adjCacheLinePrefetchDisableEnabled()) {
    if (tabSettings().adjCacheLinePrefetchDisable()) adj_disable_on_->setChecked(true);
    else adj_disable_off_->setChecked(true);
  }
  else {
    adj_disable_default_->setChecked(true);
  }
  adj_disable_grp_->blockSignals(false);

  /* Limit_CPUID_Maxval */
  cpuid_enable_grp_->blockSignals(true);
  if (tabSettings().cpuidMaxvalEnabled()) {
    if (tabSettings().cpuidMaxval()) cpuid_enable_on_->setChecked(true);
    else cpuid_enable_off_->setChecked(true);
  }
  else {
    cpuid_enable_default_->setChecked(true);
  }
  cpuid_enable_grp_->blockSignals(false);

  /* xTPR_Message_Disable */
  xtpr_disable_grp_->blockSignals(true);
  if (tabSettings().xtprMessageDisableEnabled()) {
    if (tabSettings().xtprMessageDisable()) xtpr_disable_on_->setChecked(true);
    else xtpr_disable_off_->setChecked(true);
  }
  else {
    xtpr_disable_default_->setChecked(true);
  }
  xtpr_disable_grp_->blockSignals(false);

  /* XD_Bit_Disable */
  xd_disable_grp_->blockSignals(true);
  if (tabSettings().xdBitDisableEnabled()) {
    if (tabSettings().xdBitDisable()) xd_disable_on_->setChecked(true);
    else xd_disable_off_->setChecked(true);
  }
  else {
    xd_disable_default_->setChecked(true);
  }
  xd_disable_grp_->blockSignals(false);

  /* DCU_Prefetcher_Disable */
  dcu_disable_grp_->blockSignals(true);
  if (tabSettings().dcuPrefetcherDisableEnabled()) {
    if (tabSettings().dcuPrefetcherDisable()) dcu_disable_on_->setChecked(true);
    else dcu_disable_off_->setChecked(true);
  }
  else {
    dcu_disable_default_->setChecked(true);
  }
  dcu_disable_grp_->blockSignals(false);

  /* IP_Prefetcher_Disable */
  ipp_disable_grp_->blockSignals(true);
  if (tabSettings().ipPrefetcherDisableEnabled()) {
    if (tabSettings().ipPrefetcherDisable()) ipp_disable_on_->setChecked(true);
    else ipp_disable_off_->setChecked(true);
  }
  else {
    ipp_disable_default_->setChecked(true);
  }
  ipp_disable_grp_->blockSignals(false);
}

void MiscEnable::store() {
  /* Store current UI values to the global TabMemberSettings instance */
  store(tabSettings());
}

void MiscEnable::store(Settings& data) {
  /* Store current UI values to the specified TabMemberSettings instance */

  /* Fast_Strings_Enable */
  if (fs_enable_default_->isChecked()) {
    data.fastStringsEnableEnabled(false);
    data.fastStringsEnable(tabValues().fastStringsEnable());
  }
  else {
    data.fastStringsEnableEnabled(true);
    if (fs_enable_on_->isChecked()) {
      data.fastStringsEnable(true);
    }
    else {
      data.fastStringsEnable(false);
    }
  }

  /* Hardware_Prefetcher_Disable */
  if (hwp_disable_default_->isChecked()) {
    data.hwPrefetcherDisableEnabled(false);
    data.hwPrefetcherDisable(tabValues().hwPrefetcherDisable());
  }
  else {
    data.hwPrefetcherDisableEnabled(true);
    if (hwp_disable_on_->isChecked()) {
      data.hwPrefetcherDisable(true);
    }
    else {
      data.hwPrefetcherDisable(false);
    }
  }

  /* FERR_Multiplexing_Enable */
  if (ferr_enable_default_->isChecked()) {
    data.ferrMultiplexingEnableEnabled(false);
    data.ferrMultiplexingEnable(tabValues().ferrMultiplexingEnable());
  }
  else {
    data.ferrMultiplexingEnableEnabled(true);
    if (ferr_enable_on_->isChecked()) {
      data.ferrMultiplexingEnable(true);
    }
    else {
      data.ferrMultiplexingEnable(false);
    }
  }

  /* ENABLE_MONITOR_FSM */
  if (fsm_enable_default_->isChecked()) {
    data.fsmMonitorEnableEnabled(false);
    data.fsmMonitorEnable(tabValues().fsmMonitorEnable());
  }
  else {
    data.fsmMonitorEnableEnabled(true);
    if (fsm_enable_on_->isChecked()) {
      data.fsmMonitorEnable(true);
    }
    else {
      data.fsmMonitorEnable(false);
    }
  }

  /* Adjacent_Cache_Line_Prefetch_Disable */
  if (adj_disable_default_->isChecked()) {
    data.adjCacheLinePrefetchDisableEnabled(false);
    data.adjCacheLinePrefetchDisable(tabValues().adjCacheLinePrefetchDisable());
  }
  else {
    data.adjCacheLinePrefetchDisableEnabled(true);
    if (adj_disable_on_->isChecked()) {
      data.adjCacheLinePrefetchDisable(true);
    }
    else {
      data.adjCacheLinePrefetchDisable(false);
    }
  }

  /* Limit_CPUID_Maxval */
  if (cpuid_enable_default_->isChecked()) {
    data.cpuidMaxvalEnabled(false);
    data.cpuidMaxval(tabValues().cpuidMaxval());
  }
  else {
    data.cpuidMaxvalEnabled(true);
    if (cpuid_enable_on_->isChecked()) {
      data.cpuidMaxval(true);
    }
    else {
      data.cpuidMaxval(false);
    }
  }

  /* xTPR_Message_Disable */
  if (xtpr_disable_default_->isChecked()) {
    data.xtprMessageDisableEnabled(false);
    data.xtprMessageDisable(tabValues().xtprMessageDisable());
  }
  else {
    data.xtprMessageDisableEnabled(true);
    if (xtpr_disable_on_->isChecked()) {
      data.xtprMessageDisable(true);
    }
    else {
      data.xtprMessageDisable(false);
    }
  }

  /* XD_Bit_Disable */
  if (xd_disable_default_->isChecked()) {
    data.xdBitDisableEnabled(false);
    data.xdBitDisable(tabValues().xdBitDisable());
  }
  else {
    data.xdBitDisableEnabled(true);
    if (xd_disable_on_->isChecked()) {
      data.xdBitDisable(true);
    }
    else {
      data.xdBitDisable(false);
    }
  }

  /* DCU_Prefetcher_Disable */
  if (dcu_disable_default_->isChecked()) {
    data.dcuPrefetcherDisableEnabled(false);
    data.dcuPrefetcherDisable(tabValues().dcuPrefetcherDisable());
  }
  else {
    data.dcuPrefetcherDisableEnabled(true);
    if (dcu_disable_on_->isChecked()) {
      data.dcuPrefetcherDisable(true);
    }
    else {
      data.dcuPrefetcherDisable(false);
    }
  }

  /* IP_Prefetcher_Disable */
  if (ipp_disable_default_->isChecked()) {
    data.ipPrefetcherDisableEnabled(false);
    data.ipPrefetcherDisable(tabValues().ipPrefetcherDisable());
  }
  else {
    data.ipPrefetcherDisableEnabled(true);
    if (ipp_disable_on_->isChecked()) {
      data.ipPrefetcherDisable(true);
    }
    else {
      data.ipPrefetcherDisable(false);
    }
  }
}

void MiscEnable::refresh()
{
  /* Exit if we are disabled */
  if (!isEnabled()) return;

  /*
   * Adjust Processors Features
   */

  IA32_MISC_ENABLE ia32_misc_enable(cpuInfo().firstLogicalCpu());

  if (ia32_misc_enable.read() == 0) {
    if (ia32_misc_enable.Fast_Strings_Enable != 0) {
      fs_enable_current_->setText("<font color='green'>Enabled</font>");
    }
    else {
      fs_enable_current_->setText("<font color='red'>Disabled</font>");
    }
    if (ia32_misc_enable.Hardware_Prefetcher_Disable != 0) {
      hwp_disable_current_->setText("<font color='red'>Disabled</font>");
    }
    else {
      hwp_disable_current_->setText("<font color='green'>Enabled</font>");
    }
    if (ia32_misc_enable.FERR_Multiplexing_Enable != 0) {
      ferr_enable_current_->setText("<font color='green'>Enabled</font>");
    }
    else {
      ferr_enable_current_->setText("<font color='red'>Disabled</font>");
    }
    if (ia32_misc_enable.ENABLE_MONITOR_FSM != 0) {
      fsm_enable_current_->setText("<font color='green'>Enabled</font>");
    }
    else {
      fsm_enable_current_->setText("<font color='red'>Disabled</font>");
    }
    if (ia32_misc_enable.Adjacent_Cache_Line_Prefetch_Disable != 0) {
      adj_disable_current_->setText("<font color='red'>Disabled</font>");
    }
    else {
      adj_disable_current_->setText("<font color='green'>Enabled</font>");
    }
    if (ia32_misc_enable.Limit_CPUID_Maxval != 0) {
      cpuid_enable_current_->setText("<font color='green'>Enabled</font>");
    }
    else {
      cpuid_enable_current_->setText("<font color='red'>Disabled</font>");
    }
    if (ia32_misc_enable.xTPR_Message_Disable != 0) {
      xtpr_disable_current_->setText("<font color='red'>Disabled</font>");
    }
    else {
      xtpr_disable_current_->setText("<font color='green'>Enabled</font>");
    }
    if (ia32_misc_enable.XD_Bit_Disable != 0) {
      xd_disable_current_->setText("<font color='red'>Disabled</font>");
    }
    else {
      xd_disable_current_->setText("<font color='green'>Enabled</font>");
    }
    if (ia32_misc_enable.DCU_Prefetcher_Disable != 0) {
      dcu_disable_current_->setText("<font color='red'>Disabled</font>");
    }
    else {
      dcu_disable_current_->setText("<font color='green'>Enabled</font>");
    }
    if (ia32_misc_enable.IP_Prefetcher_Disable != 0) {
      ipp_disable_current_->setText("<font color='red'>Disabled</font>");
    }
    else {
      ipp_disable_current_->setText("<font color='green'>Enabled</font>");
    }
  }
  else {
    /* could not read IA32_MISC_ENABLE msr */
    setEnabled(false);
  }
}

bool MiscEnable::apply() {
  /* Exit if we are disabled */
  if (!isEnabled()) return true;

  bool doRun = false;

  /* Assemble the command to execute. */
  std::vector<std::string> cmd {
    TabSettings::ScriptPath, "-v",
    "-p", std::to_string(cpuInfo().physicalId().value)
  };

  /* Fast_Strings_Enable */
  if (tabSettings().fastStringsEnableEnabled()) {
    cmd.emplace_back((tabSettings().fastStringsEnable()) ? "--fs-enable" : "--fs-disable");
    doRun = true;
  }

  /* Hardware_Prefetcher_Disable */
  if (tabSettings().hwPrefetcherDisableEnabled()) {
    cmd.emplace_back((tabSettings().hwPrefetcherDisable()) ? "--hwp-disable" : "--hwp-enable");
    doRun = true;
  }

  /* FERR_Multiplexing_Enable */
  if (tabSettings().ferrMultiplexingEnableEnabled()) {
    cmd.emplace_back((tabSettings().ferrMultiplexingEnable()) ? "--ferr-enable" : "--ferr-disable");
    doRun = true;
  }

  /* ENABLE_MONITOR_FSM */
  if (tabSettings().fsmMonitorEnableEnabled()) {
    cmd.emplace_back((tabSettings().fsmMonitorEnable()) ? "--fsm-enable" : "--fsm-disable");
    doRun = true;
  }

  /* Adjacent_Cache_Line_Prefetch_Disable */
  if (tabSettings().adjCacheLinePrefetchDisableEnabled()) {
    cmd.emplace_back((tabSettings().adjCacheLinePrefetchDisable()) ? "--adj-clp-disable" : "--adj-clp-enable");
    doRun = true;
  }

  /* Limit_CPUID_Maxval */
  if (tabSettings().cpuidMaxvalEnabled()) {
    cmd.emplace_back((tabSettings().cpuidMaxval()) ? "--cpuid-max-enable" : "--cpuid-max-disable");
    doRun = true;
  }

  /* xTPR_Message_Disable */
  if (tabSettings().xtprMessageDisableEnabled()) {
    cmd.emplace_back((tabSettings().xtprMessageDisable()) ? "--xtpr-msg-disable" : "--xtpr-msg-enable");
    doRun = true;
  }

  /* XD_Bit_Disable */
  if (tabSettings().xdBitDisableEnabled()) {
    cmd.emplace_back((tabSettings().xdBitDisable()) ? "--xd-bit-disable" : "--xd-bit-enable");
    doRun = true;
  }

  /* DCU_Prefetcher_Disable */
  if (tabSettings().dcuPrefetcherDisableEnabled()) {
    cmd.emplace_back((tabSettings().dcuPrefetcherDisable()) ? "--dcup-disable" : "--dcup-enable");
    doRun = true;
  }

  /* IP_Prefetcher_Disable */
  if (tabSettings().ipPrefetcherDisableEnabled()) {
    cmd.emplace_back((tabSettings().ipPrefetcherDisable()) ? "--ipp-disable" : "--ipp-enable");
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

bool MiscEnable::compare() {
  /* Store the current UI values to a temporary object */
  Settings data(tabSettings());
  store(data);

  /* Compare the relevant values in the temporary object against the
   * global TabMemberSettings instance and return false if they differ. */

  /* Fast_Strings_Enable */
  if (data.fastStringsEnableEnabled() != tabSettings().fastStringsEnableEnabled()) {
    DBGMSG("MiscEnable::Settings::fastStringsEnableEnabled() does not match")
    return false;
  }
  if (data.fastStringsEnable() != tabSettings().fastStringsEnable()) {
    DBGMSG("MiscEnable::Settings::fastStringsEnable() does not match")
    return false;
  }

  /* Hardware_Prefetcher_Disable */
  if (data.hwPrefetcherDisableEnabled() != tabSettings().hwPrefetcherDisableEnabled()) {
    DBGMSG("MiscEnable::Settings::hwPrefetcherDisableEnabled() does not match")
    return false;
  }
  if (data.hwPrefetcherDisable() != tabSettings().hwPrefetcherDisable()) {
    DBGMSG("MiscEnable::Settings::hwPrefetcherDisable() does not match")
    return false;
  }

  /* FERR_Multiplexing_Enable */
  if (data.ferrMultiplexingEnableEnabled() != tabSettings().ferrMultiplexingEnableEnabled()) {
    DBGMSG("MiscEnable::Settings::ferrMultiplexingEnableEnabled() does not match")
    return false;
  }
  if (data.ferrMultiplexingEnable() != tabSettings().ferrMultiplexingEnable()) {
    DBGMSG("MiscEnable::Settings::ferrMultiplexingEnable() does not match")
    return false;
  }

  /* ENABLE_MONITOR_FSM */
  if (data.fsmMonitorEnableEnabled() != tabSettings().fsmMonitorEnableEnabled()) {
    DBGMSG("MiscEnable::Settings::fsmMonitorEnableEnabled() does not match")
    return false;
  }
  if (data.fsmMonitorEnable() != tabSettings().fsmMonitorEnable()) {
    DBGMSG("MiscEnable::Settings::fsmMonitorEnable() does not match")
    return false;
  }

  /* Adjacent_Cache_Line_Prefetch_Disable */
  if (data.adjCacheLinePrefetchDisableEnabled() != tabSettings().adjCacheLinePrefetchDisableEnabled()) {
    DBGMSG("MiscEnable::Settings::adjCacheLinePrefetchDisableEnabled() does not match")
    return false;
  }
  if (data.adjCacheLinePrefetchDisable() != tabSettings().adjCacheLinePrefetchDisable()) {
    DBGMSG("MiscEnable::Settings::adjCacheLinePrefetchDisable() does not match")
    return false;
  }

  /* Limit_CPUID_Maxval */
  if (data.cpuidMaxvalEnabled() != tabSettings().cpuidMaxvalEnabled()) {
    DBGMSG("MiscEnable::Settings::cpuidMaxvalEnabled() does not match")
    return false;
  }
  if (data.cpuidMaxval() != tabSettings().cpuidMaxval()) {
    DBGMSG("MiscEnable::Settings::cpuidMaxval() does not match")
    return false;
  }

  /* xTPR_Message_Disable */
  if (data.xtprMessageDisableEnabled() != tabSettings().xtprMessageDisableEnabled()) {
    DBGMSG("MiscEnable::Settings::xtprMessageDisableEnabled() does not match")
    return false;
  }
  if (data.xtprMessageDisable() != tabSettings().xtprMessageDisable()) {
    DBGMSG("MiscEnable::Settings::xtprMessageDisable() does not match")
    return false;
  }

  /* XD_Bit_Disable */
  if (data.xdBitDisableEnabled() != tabSettings().xdBitDisableEnabled()) {
    DBGMSG("MiscEnable::Settings::xdBitDisableEnabled() does not match")
    return false;
  }
  if (data.xdBitDisable() != tabSettings().xdBitDisable()) {
    DBGMSG("MiscEnable::Settings::xdBitDisable() does not match")
    return false;
  }

  /* DCU_Prefetcher_Disable */
  if (data.dcuPrefetcherDisableEnabled() != tabSettings().dcuPrefetcherDisableEnabled()) {
    DBGMSG("MiscEnable::Settings::dcuPrefetcherDisableEnabled() does not match")
    return false;
  }
  if (data.dcuPrefetcherDisable() != tabSettings().dcuPrefetcherDisable()) {
    DBGMSG("MiscEnable::Settings::dcuPrefetcherDisable() does not match")
    return false;
  }

  /* IP_Prefetcher_Disable */
  if (data.ipPrefetcherDisableEnabled() != tabSettings().ipPrefetcherDisableEnabled()) {
    DBGMSG("MiscEnable::Settings::ipPrefetcherDisableEnabled() does not match")
    return false;
  }
  if (data.ipPrefetcherDisable() != tabSettings().ipPrefetcherDisable()) {
    DBGMSG("MiscEnable::Settings::ipPrefetcherDisable() does not match")
    return false;
  }

  return true;
}

/* SLOTS */

void MiscEnable::toggledSlot(int, bool checked) {
  if (checked) {
    emit valueChanged(this);
  }
}


void MiscEnable::Settings::load(QSettings& qs, const TabMemberValues& tv) {
  QString s(TabSettings::INI_GRP_PROCESSOR);
  s += QString::number(tabMemberSettings().processorNumber().value);
  qs.beginGroup(s);

  fastStringsEnableEnabled(qs.value(INI_FS_ENABLE_ENABLE, false).toBool());
  fastStringsEnable(qs.value(INI_FS_ENABLE, tv.fastStringsEnable()).toBool());

  hwPrefetcherDisableEnabled(qs.value(INI_HWP_DISABLE_ENABLE, false).toBool());
  hwPrefetcherDisable(qs.value(INI_HWP_DISABLE, tv.hwPrefetcherDisable()).toBool());

  ferrMultiplexingEnableEnabled(qs.value(INI_FMPX_ENABLE_ENABLE, false).toBool());
  ferrMultiplexingEnable(qs.value(INI_FMPX_ENABLE, tv.ferrMultiplexingEnable()).toBool());

  fsmMonitorEnableEnabled(qs.value(INI_FSM_ENABLE_ENABLE, false).toBool());
  fsmMonitorEnable(qs.value(INI_FSM_ENABLE, tv.fsmMonitorEnable()).toBool());

  adjCacheLinePrefetchDisableEnabled(qs.value(INI_ACLP_DISABLE_ENABLE, false).toBool());
  adjCacheLinePrefetchDisable(qs.value(INI_ACLP_DISABLE, tv.adjCacheLinePrefetchDisable()).toBool());

  cpuidMaxvalEnabled(qs.value(INI_CPUID_MAXVAL_ENABLE, false).toBool());
  cpuidMaxval(qs.value(INI_CPUID_MAXVAL, tv.cpuidMaxval()).toBool());

  xtprMessageDisableEnabled(qs.value(INI_XTPRM_DISABLE_ENABLE, false).toBool());
  xtprMessageDisable(qs.value(INI_XTPRM_DISABLE, tv.xtprMessageDisable()).toBool());

  xdBitDisableEnabled(qs.value(INI_XDBIT_DISABLE_ENABLE, false).toBool());
  xdBitDisable(qs.value(INI_XDBIT_DISABLE, tv.xdBitDisable()).toBool());

  dcuPrefetcherDisableEnabled(qs.value(INI_DCUP_DISABLE_ENABLE, false).toBool());
  dcuPrefetcherDisable(qs.value(INI_DCUP_DISABLE, tv. dcuPrefetcherDisable()).toBool());

  ipPrefetcherDisableEnabled(qs.value(INI_IPP_DISABLE_ENABLE, false).toBool());
  ipPrefetcherDisable(qs.value(INI_IPP_DISABLE, tv.ipPrefetcherDisable()).toBool());

  qs.endGroup();
}

void MiscEnable::Settings::save(QSettings& qs) {
  QString s(TabSettings::INI_GRP_PROCESSOR);
  s += QString::number(tabMemberSettings().processorNumber().value);
  qs.beginGroup(s);

  qs.setValue(INI_FS_ENABLE_ENABLE, QVariant::fromValue<bool>(fastStringsEnableEnabled()));
  qs.setValue(INI_FS_ENABLE, QVariant::fromValue<bool>(fastStringsEnable()));

  qs.setValue(INI_HWP_DISABLE_ENABLE, QVariant::fromValue<bool>(hwPrefetcherDisableEnabled()));
  qs.setValue(INI_HWP_DISABLE, QVariant::fromValue<bool>(hwPrefetcherDisable()));

  qs.setValue(INI_FMPX_ENABLE_ENABLE, QVariant::fromValue<bool>(ferrMultiplexingEnableEnabled()));
  qs.setValue(INI_FMPX_ENABLE, QVariant::fromValue<bool>(ferrMultiplexingEnable()));

  qs.setValue(INI_FSM_ENABLE_ENABLE, QVariant::fromValue<bool>(fsmMonitorEnableEnabled()));
  qs.setValue(INI_FSM_ENABLE, QVariant::fromValue<bool>(fsmMonitorEnable()));

  qs.setValue(INI_ACLP_DISABLE_ENABLE, QVariant::fromValue<bool>(adjCacheLinePrefetchDisableEnabled()));
  qs.setValue(INI_ACLP_DISABLE, QVariant::fromValue<bool>(adjCacheLinePrefetchDisable()));

  qs.setValue(INI_CPUID_MAXVAL_ENABLE, QVariant::fromValue<bool>(cpuidMaxvalEnabled()));
  qs.setValue(INI_CPUID_MAXVAL, QVariant::fromValue<bool>(cpuidMaxval()));

  qs.setValue(INI_XTPRM_DISABLE_ENABLE, QVariant::fromValue<bool>(xtprMessageDisableEnabled()));
  qs.setValue(INI_XTPRM_DISABLE, QVariant::fromValue<bool>(xtprMessageDisable()));

  qs.setValue(INI_XDBIT_DISABLE_ENABLE, QVariant::fromValue<bool>(xdBitDisableEnabled()));
  qs.setValue(INI_XDBIT_DISABLE, QVariant::fromValue<bool>(xdBitDisable()));

  qs.setValue(INI_DCUP_DISABLE_ENABLE, QVariant::fromValue<bool>(dcuPrefetcherDisableEnabled()));
  qs.setValue(INI_DCUP_DISABLE, QVariant::fromValue<bool>(dcuPrefetcherDisable()));

  qs.setValue(INI_IPP_DISABLE_ENABLE, QVariant::fromValue<bool>(ipPrefetcherDisableEnabled()));
  qs.setValue(INI_IPP_DISABLE, QVariant::fromValue<bool>(ipPrefetcherDisable()));

  qs.endGroup();
}

