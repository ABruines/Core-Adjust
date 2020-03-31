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

#include <iomanip>
#include <bitset>
#include <QDebug>
#include "TabMember.hpp"
#include "MsrReadout.hpp"

const std::vector<MsrReadout::MsrWidget::Description>
MsrReadout::MsrWidget::Factory {
  { "Platform Id",            &MsrWidget::Construct<MsrPlatformId> },
  { "Platform Info",          &MsrWidget::Construct<MsrPlatformInfo> },
  { "Pkg C-State",            &MsrWidget::Construct<MsrPkgCstConfigControl> },
  { "Therm Ctl",              &MsrWidget::Construct<Ia32ClockModulation> },
  { "Therm2 Ctl",             &MsrWidget::Construct<MsrTherm2Ctl> },
  { "Misc Enable",            &MsrWidget::Construct<Ia32MiscEnable> },
  { "Temperature Target",     &MsrWidget::Construct<MsrTemperatureTarget> },
  { "Turbo Ratio Limit",      &MsrWidget::Construct<MsrTurboRatioLimit> },
  { "Energy Perf Bias",       &MsrWidget::Construct<MsrEnergyPerfBias> },
  { "Turbo Activation Ratio", &MsrWidget::Construct<MsrTurboActivationRatio> },
};

MsrReadout::MsrReadout(
    const SingleCpuInfo& c, const SingleCpuId& i,
    TabMemberValues& v, TabMemberSettings& s, QWidget *p)
    : TabMemberTemplate(c, i, v, s, p, false),
      tabs_(new QTabWidget()) {
  for (auto& product : MsrWidget::Factory) tabs_->addTab(
      product.construct(cpuInfo(), cpuId(), nullptr), product.name);
  auto* layout = new QVBoxLayout(this);
  layout->addSpacing(10);
  layout->addWidget(tabs_);
}

void MsrReadout::refresh() {
  /* refresh all tab widgets */
  int idx = 0;
  QWidget* w;
  while ((w = tabs_->widget(idx++)) != nullptr) {
    static_cast<MsrWidget*>(w)->refresh();
  }
}

/*
 * MsrWidget
 */

MsrReadout::MsrWidget::MsrWidget(
    const SingleCpuInfo& cpuInfo, const SingleCpuId& cpuId, QWidget* parent)
    : QWidget(parent), cpuInfo_(cpuInfo), cpuId_(cpuId) {

  /* layout for inheritor */
  layout_ = new QGridLayout();

  auto* hb = new QHBoxLayout();
  hb->addStretch(1);
  hb->addLayout(layout_);
  hb->addStretch(1);

  auto* vb = new QVBoxLayout();
  vb->addStretch(1);
  vb->addLayout(hb);
  vb->addStretch(1);

  /* Setup scroll widget/area */
  scroll_widget_ = new QWidget();
  scroll_widget_->setLayout(vb);
  scroll_area_ = new QScrollArea();
  scroll_area_->setWidget(scroll_widget_);
  scroll_area_->setWidgetResizable(true);

  /* main layout */
  auto* widget_layout = new QVBoxLayout(this);
  widget_layout->addWidget(scroll_area_);
}

QSize MsrReadout::MsrWidget::sizeHint() const {
  /* do not adjust global app width/height */
  /*auto&& size = scroll_widget_->sizeHint();*/
  return QSize(/*size.width() +*/ 50, /*size.height() +*/ 50);
}

/*
 * MSR_PLATFORM_ID
 */

MsrReadout::MsrPlatformId::MsrPlatformId(
    const SingleCpuInfo& cpuInfo,
    const SingleCpuId& cpuId,
    QWidget *parent)
    : MsrWidget(cpuInfo, cpuId, parent),
      msr_(cpuInfo.firstLogicalCpu()) {

  read_success_ = new QLabel("<font color='red'>(failed to read)</font>");
  value_ = new QLabel("0x0000000000000000");
  max_ratio_ = new QLabel("100");
  platform_id_ = new QLabel("000");

  layout_->addWidget(new QLabel("MSR_PLATFORM_ID"), 1, 0);
  layout_->addWidget(new QLabel("Raw value:"), 2, 0);
  layout_->addWidget(new QLabel("Maximum Qualified Ratio:"), 4, 0);
  layout_->addWidget(new QLabel("Platform Id:"), 5, 0);

  layout_->addWidget(read_success_, 1, 1);
  layout_->addWidget(value_, 2, 1);
  layout_->addWidget(max_ratio_, 4, 1);
  layout_->addWidget(platform_id_, 5, 1);

  layout_->setRowMinimumHeight(0, 10);
  layout_->setRowMinimumHeight(3, 10);
  layout_->setRowStretch(6, 1);
  layout_->setColumnStretch(3, 1);
}

void MsrReadout::MsrPlatformId::refresh() {
  std::stringstream ss;
  if (msr_.read() != 0) {
    read_success_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success_->setText("");
  }

  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr_.value;
  value_->setText(QString::fromStdString(ss.str()));

  max_ratio_->setText(QString::number(msr_.Maximum_Qualified_Ratio));

  ss.str(std::string());
  ss << std::bitset<3>(msr_.Platform_Id) << "b";
  platform_id_->setText(QString::fromStdString(ss.str()));
}

/*
 * MSR_PLATFORM_INFO
 */

MsrReadout::MsrPlatformInfo::MsrPlatformInfo(
  const SingleCpuInfo& cpuInfo,
  const SingleCpuId& cpuId,
  QWidget *parent)
  : MsrWidget(cpuInfo, cpuId, parent),
    msr_(cpuInfo.firstLogicalCpu()) {

  read_success_ = new QLabel("<font color='red'>(failed to read)</font>");
  value_ = new QLabel("0x0000000000000000");
  max_non_turbo_ = new QLabel("255");
  ppin_cap_ = new QLabel("0");
  has_ratio_limit_ = new QLabel("0");
  has_tdp_limit_ = new QLabel("0");
  has_tj_offset_ = new QLabel("0");
  has_lpm_support_ = new QLabel("0");
  nr_tdp_levels_ = new QLabel("255");
  max_eff_ratio_ = new QLabel("255");
  min_ratio_ = new QLabel("255");

  layout_->addWidget(new QLabel("MSR_PLATFORM_INFO"), 1, 0);
  layout_->addWidget(new QLabel("Raw value:"), 2, 0);
  layout_->addWidget(new QLabel("Maximum Non-Turbo Ratio:"), 4, 0);
  layout_->addWidget(new QLabel("PPIN_CAP:"), 5, 0);
  layout_->addWidget(new QLabel("Programmable Ratio Limit for Turbo Mode:"), 6, 0);
  layout_->addWidget(new QLabel("Programmable TDP Limit for Turbo Mode:"), 7, 0);
  layout_->addWidget(new QLabel("Programmable TJ OFFSET:"), 8, 0);
  layout_->addWidget(new QLabel("Low Power Mode Support:"), 9, 0);
  layout_->addWidget(new QLabel("Number of ConfigTDP Levels:"), 10, 0);
  layout_->addWidget(new QLabel("Maximum Efficiency Ratio:"), 11, 0);
  layout_->addWidget(new QLabel("Minimum Operating Ratio:"), 12, 0);

  layout_->addWidget(read_success_, 1, 1);
  layout_->addWidget(value_, 2, 1);
  layout_->addWidget(max_non_turbo_, 4, 1);
  layout_->addWidget(ppin_cap_, 5, 1);
  layout_->addWidget(has_ratio_limit_, 6, 1);
  layout_->addWidget(has_tdp_limit_, 7, 1);
  layout_->addWidget(has_tj_offset_, 8, 1);
  layout_->addWidget(has_lpm_support_, 9, 1);
  layout_->addWidget(nr_tdp_levels_, 10, 1);
  layout_->addWidget(max_eff_ratio_, 11, 1);
  layout_->addWidget(min_ratio_, 12, 1);

  layout_->setRowMinimumHeight(0, 10);
  layout_->setRowMinimumHeight(3, 10);
  layout_->setRowStretch(13, 1);
  layout_->setColumnStretch(3, 1);
}

void MsrReadout::MsrPlatformInfo::refresh() {
  if (msr_.read() != 0) {
    read_success_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success_->setText("");
  }

  std::stringstream ss;
  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr_.value;

  value_->setText(QString::fromStdString(ss.str()));
  max_non_turbo_->setText(QString::number(msr_.Maximum_Non_Turbo_Ratio));
  ppin_cap_->setText(QString::number(msr_.PPIN_CAP));
  has_ratio_limit_->setText(QString::number(msr_.Programmable_Ratio_Limit_For_Turbo_Mode));
  has_tdp_limit_->setText(QString::number(msr_.Programmable_TDP_Limit_For_Turbo_Mode));
  has_tj_offset_->setText(QString::number(msr_.Programmable_TJ_OFFSET));
  has_lpm_support_->setText(QString::number(msr_.Low_Power_Mode_Support));
  nr_tdp_levels_->setText(QString::number(msr_.Number_Of_ConfigTDP_Levels));
  max_eff_ratio_->setText(QString::number(msr_.Maximum_Efficiency_Ratio));
  min_ratio_->setText(QString::number(msr_.Minimum_Operating_Ratio));
}

/*
 * MSR_PKG_CST_CONFIG_CONTROL
 */

MsrReadout::MsrPkgCstConfigControl::MsrPkgCstConfigControl(
  const SingleCpuInfo& cpuInfo,
  const SingleCpuId& cpuId,
  QWidget *parent)
  : MsrWidget(cpuInfo, cpuId, parent),
    msr_(cpuInfo.firstLogicalCpu()) {

  read_success_ = new QLabel("<font color='red'>(failed to read)</font>");
  value_ = new QLabel("0x0000000000000000");
  c_state_limit_ = new QLabel("C0/C1");
  mwait_redirection_ = new QLabel("0");
  cfg_lock_ = new QLabel("0");
  c3_auto_demote_ = new QLabel("0");
  c1_auto_demote_ = new QLabel("0");
  enable_c3_undemote_ = new QLabel("0");
  enable_c1_undemote_ = new QLabel("0");

  layout_->addWidget(new QLabel("MSR_PKG_CST_CONFIG_CONTROL"), 1, 0);
  layout_->addWidget(new QLabel("Raw value:"), 2, 0);
  layout_->addWidget(new QLabel("Package C-State Limit:"), 4, 0);
  layout_->addWidget(new QLabel("I/O MWAIT Redirection Enable:"), 5, 0);
  layout_->addWidget(new QLabel("CFG Lock:"), 6, 0);
  layout_->addWidget(new QLabel("C3 State Auto Demotion Enable:"), 7, 0);
  layout_->addWidget(new QLabel("C1 State Auto Demotion Enable:"), 8, 0);
  layout_->addWidget(new QLabel("Enable C3 Undemotion:"), 9, 0);
  layout_->addWidget(new QLabel("Enable C1 Undemotion:"), 10, 0);

  layout_->addWidget(read_success_, 1, 1);
  layout_->addWidget(value_, 2, 1);
  layout_->addWidget(c_state_limit_, 4, 1);
  layout_->addWidget(mwait_redirection_, 5, 1);
  layout_->addWidget(cfg_lock_, 6, 1);
  layout_->addWidget(c3_auto_demote_, 7, 1);
  layout_->addWidget(c1_auto_demote_, 8, 1);
  layout_->addWidget(enable_c3_undemote_, 9, 1);
  layout_->addWidget(enable_c1_undemote_, 10, 1);

  layout_->setRowMinimumHeight(0, 10);
  layout_->setRowMinimumHeight(3, 10);
  layout_->setRowStretch(11, 1);
  layout_->setColumnStretch(3, 1);
}

void MsrReadout::MsrPkgCstConfigControl::refresh() {
  if (msr_.read() != 0) {
    read_success_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success_->setText("");
  }

  std::stringstream ss;
  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr_.value;

  value_->setText(QString::fromStdString(ss.str()));
  switch (msr_.Package_C_State_Limit) {
    case 0: c_state_limit_->setText("C0/C1"); break;
    case 1: c_state_limit_->setText("C2"); break;
    case 2: c_state_limit_->setText("C3"); break;
    case 3: c_state_limit_->setText("C6"); break;
    case 4: c_state_limit_->setText("C7"); break;
    case 5: c_state_limit_->setText("C7s"); break;
    case 6: c_state_limit_->setText("C8"); break;
    case 7: c_state_limit_->setText("C9"); break;
    case 8: c_state_limit_->setText("C10"); break;
    default: c_state_limit_->setText("???"); break;
  }
  mwait_redirection_->setText(QString::number(msr_.IO_MWAIT_Redirection_Enable));
  cfg_lock_->setText(QString::number(msr_.CFG_Lock));
  c3_auto_demote_->setText(QString::number(msr_.C3_State_Auto_Demotion_Enable));
  c1_auto_demote_->setText(QString::number(msr_.C1_State_Auto_Demotion_Enable));
  enable_c3_undemote_->setText(QString::number(msr_.Enable_C3_Undemotion));
  enable_c1_undemote_->setText(QString::number(msr_.Enable_C1_Undemotion));
}

/*
 * IA32_CLOCK_MODULATION
 */

MsrReadout::Ia32ClockModulation::Ia32ClockModulation(
  const SingleCpuInfo& cpuInfo,
  const SingleCpuId& cpuId,
  QWidget *parent)
  : MsrWidget(cpuInfo, cpuId, parent),
    msr_(cpuInfo.firstLogicalCpu()) {

  read_success_ = new QLabel("<font color='red'>(failed to read)</font>");
  value_ = new QLabel("0x0000000000000000");
  extended_ = new QLabel("Yes, CPUID.06H.EAX[5] == 1");
  dutycycle_ = new QLabel("0010b == 12.5% (default)");
  enabled_ = new QLabel("0");

  layout_->addWidget(new QLabel("IA32_CLOCK_MODULATION"), 1, 0);
  layout_->addWidget(new QLabel("Raw value:"), 2, 0);
  layout_->addWidget(new QLabel("Ext. On-Demand Clock Modulation Duty Cycle:"), 4, 0);
  layout_->addWidget(new QLabel("On-Demand Clock Modulation Duty Cycle:"), 5, 0);
  layout_->addWidget(new QLabel("On-Demand Clock Modulation Enable:"), 6, 0);

  layout_->addWidget(read_success_, 1, 1);
  layout_->addWidget(value_, 2, 1);
  layout_->addWidget(extended_, 4, 1);
  layout_->addWidget(dutycycle_, 5, 1);
  layout_->addWidget(enabled_, 6, 1);

  layout_->setRowMinimumHeight(0, 10);
  layout_->setRowMinimumHeight(3, 10);
  layout_->setRowStretch(7, 1);
  layout_->setColumnStretch(3, 1);
}

void MsrReadout::Ia32ClockModulation::refresh() {
  if (msr_.read() != 0) {
    read_success_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success_->setText("");
  }

  std::stringstream ss;
  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr_.value;

  value_->setText(QString::fromStdString(ss.str()));
  if ((cpuId_.EAX_06H.EAX & 1<<5) == 0){
    extended_->setText("CPUID.06H.EAX[5] == 0");
    switch (msr_.OnDemand_ClockModulation_DutyCycle) {
      case 0: dutycycle_->setText("000b == Reserved"); break;
      case 1: dutycycle_->setText("001b == 12.5% (default)"); break;
      case 2: dutycycle_->setText("010b == 25%"); break;
      case 3: dutycycle_->setText("011b == 37.5%"); break;
      case 4: dutycycle_->setText("100b == 50%"); break;
      case 5: dutycycle_->setText("101b == 62.5%"); break;
      case 6: dutycycle_->setText("110b == 75%"); break;
      case 7: dutycycle_->setText("111b == 87.5%"); break;
    }
  }
  else {
    extended_->setText("CPUID.06H.EAX[5] == 1");
    switch ((msr_.OnDemand_ClockModulation_DutyCycle << 1) | msr_.Extended_OnDemand_ClockModulation_DutyCycle) {
      case 0:  dutycycle_->setText("0000b == Reserved"); break;
      case 1:  dutycycle_->setText("0001b == 6.25%"); break;
      case 2:  dutycycle_->setText("0010b == 12.5% (default)"); break;
      case 3:  dutycycle_->setText("0011b == 18.75%"); break;
      case 4:  dutycycle_->setText("0100b == 25%"); break;
      case 5:  dutycycle_->setText("0101b == 31.25"); break;
      case 6:  dutycycle_->setText("0110b == 37.5%"); break;
      case 7:  dutycycle_->setText("0111b == 43.75%"); break;
      case 8:  dutycycle_->setText("1000b == 50%"); break;
      case 9:  dutycycle_->setText("1001b == 56.25%"); break;
      case 10: dutycycle_->setText("1010b == 62.5%"); break;
      case 11: dutycycle_->setText("1011b == 68.75%"); break;
      case 12: dutycycle_->setText("1100b == 75%"); break;
      case 13: dutycycle_->setText("1101b == 81.25%"); break;
      case 14: dutycycle_->setText("1110b == 87.5%"); break;
      case 15: dutycycle_->setText("1111b == 93.75%"); break;
    }
  }
  enabled_->setText(QString::number(msr_.OnDemand_ClockModulation_Enable));
}

/*
 * MSR_THERM2_CTL
 */

MsrReadout::MsrTherm2Ctl::MsrTherm2Ctl(
  const SingleCpuInfo& cpuInfo,
  const SingleCpuId& cpuId,
  QWidget *parent)
  : MsrWidget(cpuInfo, cpuId, parent),
    msr_(cpuInfo.firstLogicalCpu()) {

  read_success_ = new QLabel("<font color='red'>(failed to read)</font>");
  value_ = new QLabel("0x0000000000000000");
  tm2_transition_target_ = new QLabel("0x0000");
  tm_select_ = new QLabel("TM1");

  layout_->addWidget(new QLabel("MSR_THERM2_CTL"), 1, 0);
  layout_->addWidget(new QLabel("Raw value:"), 2, 0);
  layout_->addWidget(new QLabel("TM2 transition target:"), 4, 0);
  layout_->addWidget(new QLabel("TM Select:"), 5, 0);

  layout_->addWidget(read_success_, 1, 1);
  layout_->addWidget(value_, 2, 1);
  layout_->addWidget(tm2_transition_target_, 4, 1);
  layout_->addWidget(tm_select_, 5, 1);

  layout_->setRowMinimumHeight(0, 10);
  layout_->setRowMinimumHeight(3, 10);
  layout_->setRowStretch(6, 1);
  layout_->setColumnStretch(3, 1);
}

void MsrReadout::MsrTherm2Ctl::refresh() {
  if (msr_.read() != 0) {
    read_success_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success_->setText("");
  }

  std::stringstream ss;
  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr_.value;

  value_->setText(QString::fromStdString(ss.str()));

  ss.str(std::string());
  ss << "0x" << std::setw(4) << std::setfill('0')
     << std::uppercase << std::hex << msr_.TM2_transistion_target;
  tm2_transition_target_->setText(QString::fromStdString(ss.str()));

  if (msr_.TM_SELECT == 0) {
    tm_select_->setText("0 == TM1");
  }
  else {
    tm_select_->setText("1 == TM2");
  }
}

/*
 * IA32_MISC_ENABLE
 */

MsrReadout::Ia32MiscEnable::Ia32MiscEnable(
  const SingleCpuInfo& cpuInfo,
  const SingleCpuId& cpuId,
  QWidget *parent)
  : MsrWidget(cpuInfo, cpuId, parent),
    msr_(cpuInfo.firstLogicalCpu()) {

  read_success_ = new QLabel("<font color='red'>(failed to read)</font>");
  value_ = new QLabel("0x0000000000000000");
  fast_strings_enable_ = new QLabel("0");
  tcc_enable_ = new QLabel("0");
  perfmon_available_ = new QLabel("0");
  prefetch_disable_ = new QLabel("0");
  ferr_mux_enable_ = new QLabel("0");
  bts_unavailable_ = new QLabel("0");
  ev_based_sampling_ = new QLabel("0");
  tm2_enable_ = new QLabel("0");
  eist_enable_ = new QLabel("0");
  fsm_enable_ = new QLabel("0");
  cache_prefetch_disable_ = new QLabel("0");
  eist_select_lock_ = new QLabel("0");
  cpuid_limit_ = new QLabel("0");
  xptr_msg_disable_ = new QLabel("0");
  xd_bit_disable_ = new QLabel("0");
  dcu_prefetch_disable_ = new QLabel("0");
  ida_disable_ = new QLabel("0");
  ip_prefetch_disable_ = new QLabel("0");

  layout_->addWidget(new QLabel("IA32_MISC_ENABLE"), 1, 0);
  layout_->addWidget(new QLabel("Raw value:"), 2, 0);
  layout_->addWidget(new QLabel("Fast-Strings Enable:"), 4, 0);
  layout_->addWidget(new QLabel("Automatic Thermal Control Circuit Enable:"), 5, 0);
  layout_->addWidget(new QLabel("Performance Monitoring Available:"), 6, 0);
  layout_->addWidget(new QLabel("Hardware Prefetcher Disable:"), 7, 0);
  layout_->addWidget(new QLabel("FERR# Multiplexing Enable:"), 8, 0);
  layout_->addWidget(new QLabel("Branch Trace Storage Unavailable:"), 9, 0);
  layout_->addWidget(new QLabel("Processor Event Based Sampling:"), 10, 0);
  layout_->addWidget(new QLabel("TM2 Enable:"), 11, 0);
  layout_->addWidget(new QLabel("Enhanced Intel SpeedStep Technology Enable:"), 12, 0);
  layout_->addWidget(new QLabel("Enable Monitor FSM:"), 13, 0);
  layout_->addWidget(new QLabel("Adjacent Cache Line Prefetch Disable:"), 14, 0);
  layout_->addWidget(new QLabel("Enhanced Intel SpeedStep Technology Select Lock:"), 15, 0);
  layout_->addWidget(new QLabel("Limit CPUID Maxval:"), 16, 0);
  layout_->addWidget(new QLabel("xTPR Message Disable:"), 17, 0);
  layout_->addWidget(new QLabel("XD Bit Disable:"), 18, 0);
  layout_->addWidget(new QLabel("DCU Prefetcher Disable:"), 19, 0);
  layout_->addWidget(new QLabel("IDA (Turbo Boost Technology) Disable:"), 20, 0);
  layout_->addWidget(new QLabel("IP Prefetcher Disable:"), 21, 0);

  layout_->addWidget(read_success_, 1, 1);
  layout_->addWidget(value_, 2, 1);
  layout_->addWidget(fast_strings_enable_, 4, 1);
  layout_->addWidget(tcc_enable_, 5, 1);
  layout_->addWidget(perfmon_available_, 6, 1);
  layout_->addWidget(prefetch_disable_, 7, 1);
  layout_->addWidget(ferr_mux_enable_, 8, 1);
  layout_->addWidget(bts_unavailable_, 9, 1);
  layout_->addWidget(ev_based_sampling_, 10, 1);
  layout_->addWidget(tm2_enable_, 11, 1);
  layout_->addWidget(eist_enable_, 12, 1);
  layout_->addWidget(fsm_enable_, 13, 1);
  layout_->addWidget(cache_prefetch_disable_, 14, 1);
  layout_->addWidget(eist_select_lock_, 15, 1);
  layout_->addWidget(cpuid_limit_, 16, 1);
  layout_->addWidget(xptr_msg_disable_, 17, 1);
  layout_->addWidget(xd_bit_disable_, 18, 1);
  layout_->addWidget(dcu_prefetch_disable_, 19, 1);
  layout_->addWidget(ida_disable_, 20, 1);
  layout_->addWidget(ip_prefetch_disable_, 21, 1);

  layout_->setRowMinimumHeight(0, 10);
  layout_->setRowMinimumHeight(3, 10);
  layout_->setRowStretch(22, 1);
  layout_->setColumnStretch(3, 1);
}

void MsrReadout::Ia32MiscEnable::refresh() {
  if (msr_.read() != 0) {
    read_success_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success_->setText("");
  }

  std::stringstream ss;
  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr_.value;

  value_->setText(QString::fromStdString(ss.str()));
  fast_strings_enable_->setText(QString::number(msr_.Fast_Strings_Enable));
  tcc_enable_->setText(QString::number(msr_.Automatic_Thermal_Control_Circuit_Enable));
  perfmon_available_->setText(QString::number(msr_.Performance_Monitoring_Available));
  prefetch_disable_->setText(QString::number(msr_.Hardware_Prefetcher_Disable));
  ferr_mux_enable_->setText(QString::number(msr_.FERR_Multiplexing_Enable));
  bts_unavailable_->setText(QString::number(msr_.Branch_Trace_Storage_Unavailable));
  ev_based_sampling_->setText(QString::number(msr_.Processor_Event_Based_Sampling));
  tm2_enable_->setText(QString::number(msr_.TM2_ENABLE));
  eist_enable_->setText(QString::number(msr_.EIST_Enable));
  fsm_enable_->setText(QString::number(msr_.ENABLE_MONITOR_FSM));
  cache_prefetch_disable_->setText(QString::number(msr_.Adjacent_Cache_Line_Prefetch_Disable));
  eist_select_lock_->setText(QString::number(msr_.EIST_Select_Lock));
  cpuid_limit_->setText(QString::number(msr_.Limit_CPUID_Maxval));
  xptr_msg_disable_->setText(QString::number(msr_.xTPR_Message_Disable));
  xd_bit_disable_->setText(QString::number(msr_.XD_Bit_Disable));
  dcu_prefetch_disable_->setText(QString::number(msr_.DCU_Prefetcher_Disable));
  ida_disable_->setText(QString::number(msr_.IDA_Disable));
  ip_prefetch_disable_->setText(QString::number(msr_.IP_Prefetcher_Disable));
}

/*
 * MSR_TEMPERATURE_TARGET
 */

MsrReadout::MsrTemperatureTarget::MsrTemperatureTarget(
  const SingleCpuInfo& cpuInfo,
  const SingleCpuId& cpuId,
  QWidget *parent)
  : MsrWidget(cpuInfo, cpuId, parent),
    msr_(cpuInfo.firstLogicalCpu()) {

  read_success_ = new QLabel("<font color='red'>(failed to read)</font>");
  value_ = new QLabel("0x0000000000000000");
  temperature_target_ = new QLabel("100");
  target_offset_ = new QLabel("00");

  layout_->addWidget(new QLabel("MSR_TEMPERATURE_TARGET"), 1, 0);
  layout_->addWidget(new QLabel("Raw value:"), 2, 0);
  layout_->addWidget(new QLabel("PROCHOT# temperature:"), 4, 0);
  layout_->addWidget(new QLabel("PROCHOT# offset:"), 5, 0);

  layout_->addWidget(read_success_, 1, 1);
  layout_->addWidget(value_, 2, 1);
  layout_->addWidget(temperature_target_, 4, 1);
  layout_->addWidget(target_offset_, 5, 1);

  layout_->setRowMinimumHeight(0, 10);
  layout_->setRowMinimumHeight(3, 10);
  layout_->setRowStretch(6, 1);
  layout_->setColumnStretch(3, 1);
}

void MsrReadout::MsrTemperatureTarget::refresh() {
  if (msr_.read() != 0) {
    read_success_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success_->setText("");
  }

  std::stringstream ss;
  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr_.value;

  value_->setText(QString::fromStdString(ss.str()));
  temperature_target_->setText(QString::number(msr_.Temperature_Target));

  if (cpuInfo_.isSilvermontOrLater() || cpuInfo_.isXeonPhi()) {
    target_offset_->setText(QString::number(msr_.Target_Offset_29_24));
  }
  else  {
    target_offset_->setText(QString::number(msr_.Target_Offset_27_24));
  }
}

/*
 * MSR_TURBO_RATIO_LIMIT
 */

MsrReadout::MsrTurboRatioLimit::MsrTurboRatioLimit(
  const SingleCpuInfo& cpuInfo,
  const SingleCpuId& cpuId,
  QWidget *parent)
  : MsrWidget(cpuInfo, cpuId, parent),
    msr_(cpuInfo.firstLogicalCpu()),
    msr1_(cpuInfo.firstLogicalCpu()),
    msr2_(cpuInfo.firstLogicalCpu()),
    msr3_(cpuInfo.firstLogicalCpu()) {

  read_success_ = new QLabel("<font color='red'>(failed to read)</font>");
  value_ = new QLabel("0x0000000000000000");
  ratio_limit_1c_ = new QLabel("255");
  ratio_limit_2c_ = new QLabel("255");
  ratio_limit_3c_ = new QLabel("255");
  ratio_limit_4c_ = new QLabel("255");
  ratio_limit_5c_ = new QLabel("255");
  ratio_limit_6c_ = new QLabel("255");
  ratio_limit_7c_ = new QLabel("255");
  ratio_limit_8c_ = new QLabel("255");

  layout_->addWidget(new QLabel("MSR_TURBO_RATIO_LIMIT"), 1, 0);
  layout_->addWidget(new QLabel("Raw value:"), 2, 0);
  layout_->addWidget(new QLabel("Ratio Limit 1C:"), 4, 0);
  layout_->addWidget(new QLabel("Ratio Limit 2C:"), 5, 0);
  layout_->addWidget(new QLabel("Ratio Limit 3C:"), 6, 0);
  layout_->addWidget(new QLabel("Ratio Limit 4C:"), 7, 0);
  layout_->addWidget(new QLabel("Ratio Limit 5C:"), 8, 0);
  layout_->addWidget(new QLabel("Ratio Limit 6C:"), 9, 0);
  layout_->addWidget(new QLabel("Ratio Limit 7C:"), 10, 0);
  layout_->addWidget(new QLabel("Ratio Limit 8C:"), 11, 0);

  layout_->addWidget(read_success_, 1, 1);
  layout_->addWidget(value_, 2, 1);
  layout_->addWidget(ratio_limit_1c_, 4, 1);
  layout_->addWidget(ratio_limit_2c_, 5, 1);
  layout_->addWidget(ratio_limit_3c_, 6, 1);
  layout_->addWidget(ratio_limit_4c_, 7, 1);
  layout_->addWidget(ratio_limit_5c_, 8, 1);
  layout_->addWidget(ratio_limit_6c_, 9, 1);
  layout_->addWidget(ratio_limit_7c_, 10, 1);
  layout_->addWidget(ratio_limit_8c_, 11, 1);

  read_success1_ = new QLabel("<font color='red'>(failed to read)</font>");
  value1_ = new QLabel("0x0000000000000000");
  ratio_limit_9c_ = new QLabel("255");
  ratio_limit_10c_ = new QLabel("255");
  ratio_limit_11c_ = new QLabel("255");
  ratio_limit_12c_ = new QLabel("255");
  ratio_limit_13c_ = new QLabel("255");
  ratio_limit_14c_ = new QLabel("255");
  ratio_limit_15c_ = new QLabel("255");
  ratio_limit_16c_ = new QLabel("255");


  layout_->addWidget(new QLabel("MSR_TURBO_RATIO_LIMIT1"), 13, 0);
  layout_->addWidget(new QLabel("Raw value:"), 14, 0);
  layout_->addWidget(new QLabel("Ratio Limit 9C:"), 16, 0);
  layout_->addWidget(new QLabel("Ratio Limit 10C:"), 17, 0);
  layout_->addWidget(new QLabel("Ratio Limit 11C:"), 18, 0);
  layout_->addWidget(new QLabel("Ratio Limit 12C:"), 19, 0);
  layout_->addWidget(new QLabel("Ratio Limit 13C:"), 20, 0);
  layout_->addWidget(new QLabel("Ratio Limit 14C:"), 21, 0);
  layout_->addWidget(new QLabel("Ratio Limit 15C:"), 22, 0);
  layout_->addWidget(new QLabel("Ratio Limit 16C:"), 23, 0);

  layout_->addWidget(read_success1_, 13, 1);
  layout_->addWidget(value1_, 14, 1);
  layout_->addWidget(ratio_limit_9c_, 16, 1);
  layout_->addWidget(ratio_limit_10c_, 17, 1);
  layout_->addWidget(ratio_limit_11c_, 18, 1);
  layout_->addWidget(ratio_limit_12c_, 19, 1);
  layout_->addWidget(ratio_limit_13c_, 20, 1);
  layout_->addWidget(ratio_limit_14c_, 21, 1);
  layout_->addWidget(ratio_limit_15c_, 22, 1);
  layout_->addWidget(ratio_limit_16c_, 23, 1);

  read_success2_ = new QLabel("<font color='red'>(failed to read)</font>");
  value2_ = new QLabel("0x0000000000000000");
  ratio_limit_17c_ = new QLabel("255");
  ratio_limit_18c_ = new QLabel("255");
  ratio_sema2_ = new QLabel("0");

  layout_->addWidget(new QLabel("MSR_TURBO_RATIO_LIMIT2"), 25, 0);
  layout_->addWidget(new QLabel("Raw value:"), 26, 0);
  layout_->addWidget(new QLabel("Ratio Limit 17C:"), 28, 0);
  layout_->addWidget(new QLabel("Ratio Limit 18C:"), 29, 0);
  layout_->addWidget(new QLabel("Ratio Limit Semaphore:"), 30, 0);

  layout_->addWidget(read_success2_, 25, 1);
  layout_->addWidget(value2_, 26, 1);
  layout_->addWidget(ratio_limit_17c_, 28, 1);
  layout_->addWidget(ratio_limit_18c_, 29, 1);
  layout_->addWidget(ratio_sema2_, 30, 1);

  read_success3_ = new QLabel("<font color='red'>(failed to read)</font>");
  value3_ = new QLabel("0x0000000000000000");
  ratio_sema3_ = new QLabel("0");

  layout_->addWidget(new QLabel("MSR_TURBO_RATIO_LIMIT3"), 32, 0);
  layout_->addWidget(new QLabel("Raw value:"), 33, 0);
  layout_->addWidget(new QLabel("Ratio Limit Semaphore:"), 35, 0);

  layout_->addWidget(read_success3_, 32, 1);
  layout_->addWidget(value3_, 33, 1);
  layout_->addWidget(ratio_sema3_, 35, 1);

  layout_->setRowMinimumHeight(0, 10);
  layout_->setRowMinimumHeight(3, 10);
  layout_->setRowMinimumHeight(12, 20);
  layout_->setRowMinimumHeight(15, 10);
  layout_->setRowMinimumHeight(24, 20);
  layout_->setRowMinimumHeight(27, 10);
  layout_->setRowMinimumHeight(31, 20);
  layout_->setRowMinimumHeight(34, 10);
  layout_->setRowStretch(36, 1);
  layout_->setColumnStretch(3, 1);
}

void MsrReadout::MsrTurboRatioLimit::refresh() {
  if (msr_.read() != 0) {
    read_success_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success_->setText("");
  }

  std::stringstream ss;
  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr_.value;

  value_->setText(QString::fromStdString(ss.str()));
  ratio_limit_1c_->setText(QString::number(msr_.Ratio_Limit_1C));
  ratio_limit_2c_->setText(QString::number(msr_.Ratio_Limit_2C));
  ratio_limit_3c_->setText(QString::number(msr_.Ratio_Limit_3C));
  ratio_limit_4c_->setText(QString::number(msr_.Ratio_Limit_4C));
  ratio_limit_5c_->setText(QString::number(msr_.Ratio_Limit_5C));
  ratio_limit_6c_->setText(QString::number(msr_.Ratio_Limit_6C));
  ratio_limit_7c_->setText(QString::number(msr_.Ratio_Limit_7C));
  ratio_limit_8c_->setText(QString::number(msr_.Ratio_Limit_8C));

  if (msr1_.read() != 0) {
    read_success1_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success1_->setText("");
  }

  ss.str(std::string());
  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr1_.value;

  value1_->setText(QString::fromStdString(ss.str()));
  ratio_limit_9c_->setText(QString::number(msr1_.Ratio_Limit_9C));
  ratio_limit_10c_->setText(QString::number(msr1_.Ratio_Limit_10C));
  ratio_limit_11c_->setText(QString::number(msr1_.Ratio_Limit_11C));
  ratio_limit_12c_->setText(QString::number(msr1_.Ratio_Limit_12C));
  ratio_limit_13c_->setText(QString::number(msr1_.Ratio_Limit_13C));
  ratio_limit_14c_->setText(QString::number(msr1_.Ratio_Limit_14C));
  ratio_limit_15c_->setText(QString::number(msr1_.Ratio_Limit_15C));
  ratio_limit_16c_->setText(QString::number(msr1_.Ratio_Limit_16C));

  if (msr2_.read() != 0) {
    read_success2_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success2_->setText("");
  }

  ss.str(std::string());
  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr2_.value;

  value2_->setText(QString::fromStdString(ss.str()));
  ratio_limit_17c_->setText(QString::number(msr2_.Ratio_Limit_17C));
  ratio_limit_18c_->setText(QString::number(msr2_.Ratio_Limit_18C));
  ratio_sema2_->setText(QString::number(msr2_.Ratio_Limit_Semaphore));

  if (msr3_.read() != 0) {
    read_success3_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success3_->setText("");
  }

  ss.str(std::string());
  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr3_.value;

  value3_->setText(QString::fromStdString(ss.str()));
  ratio_sema3_->setText(QString::number(msr3_.Ratio_Limit_Semaphore));
}

/*
 * IA32_ENERGY_PERF_BIAS
 */

MsrReadout::MsrEnergyPerfBias::MsrEnergyPerfBias(
  const SingleCpuInfo& cpuInfo,
  const SingleCpuId& cpuId,
  QWidget *parent)
  : MsrWidget(cpuInfo, cpuId, parent),
    msr_(cpuInfo.firstLogicalCpu()) {

  read_success_ = new QLabel("<font color='red'>(failed to read)</font>");
  value_ = new QLabel("0x0000000000000000");
  power_policy_preference_ = new QLabel("15");

  layout_->addWidget(new QLabel("IA32_ENERGY_PERF_BIAS"), 1, 0);
  layout_->addWidget(new QLabel("Raw value:"), 2, 0);
  layout_->addWidget(new QLabel("Power Policy Preference:"), 4, 0);

  layout_->addWidget(read_success_, 1, 1);
  layout_->addWidget(value_, 2, 1);
  layout_->addWidget(power_policy_preference_, 4, 1);

  layout_->setRowMinimumHeight(0, 10);
  layout_->setRowMinimumHeight(3, 10);
  layout_->setRowStretch(6, 1);
  layout_->setColumnStretch(3, 1);
}

void MsrReadout::MsrEnergyPerfBias::refresh() {
  if (msr_.read() != 0) {
    read_success_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success_->setText("");
  }

  std::stringstream ss;
  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr_.value;

  value_->setText(QString::fromStdString(ss.str()));
  power_policy_preference_->setText(QString::number(msr_.Power_Policy_Preference));
}

/*
 * MSR_TURBO_ACTIVATION_RATIO
 */

MsrReadout::MsrTurboActivationRatio::MsrTurboActivationRatio(
  const SingleCpuInfo& cpuInfo,
  const SingleCpuId& cpuId,
  QWidget *parent)
  : MsrWidget(cpuInfo, cpuId, parent),
    msr_(cpuInfo.firstLogicalCpu()) {

  read_success_ = new QLabel("<font color='red'>(failed to read)</font>");
  value_ = new QLabel("0x0000000000000000");
  max_non_turbo_ratio_ = new QLabel("255");
  max_non_turbo_lock_ = new QLabel("0");

  layout_->addWidget(new QLabel("MSR_TURBO_ACTIVATION_RATIO"), 1, 0);
  layout_->addWidget(new QLabel("Raw value:"), 2, 0);
  layout_->addWidget(new QLabel("Maximum non-turbo ratio:"), 4, 0);
  layout_->addWidget(new QLabel("Lock:"), 5, 0);

  layout_->addWidget(read_success_, 1, 1);
  layout_->addWidget(value_, 2, 1);
  layout_->addWidget(max_non_turbo_ratio_, 4, 1);
  layout_->addWidget(max_non_turbo_lock_, 5, 1);

  layout_->setRowMinimumHeight(0, 10);
  layout_->setRowMinimumHeight(3, 10);
  layout_->setRowStretch(6, 1);
  layout_->setColumnStretch(3, 1);
}

void MsrReadout::MsrTurboActivationRatio::refresh() {
  if (msr_.read() != 0) {
    read_success_->setText("<font color='red'>(failed to read)</font>");
  }
  else {
    read_success_->setText("");
  }

  std::stringstream ss;
  ss << "0x" << std::setw(16) << std::setfill('0')
     << std::uppercase << std::hex << msr_.value;

  value_->setText(QString::fromStdString(ss.str()));
  max_non_turbo_ratio_->setText(QString::number(msr_.MAX_NON_TURBO_RATIO));
  max_non_turbo_lock_->setText(QString::number(msr_.TURBO_ACTIVATION_RATIO_Lock));
}

