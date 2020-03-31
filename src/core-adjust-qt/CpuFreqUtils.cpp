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
  * @file src/core-adjust-qt/CpuFreqUtils.hpp
  * @brief A TabMemberWidget for adjusting Frequency Scaling settings
  *
  * @file src/core-adjust-qt/CpuFreqUtils.cpp
  * @brief A TabMemberWidget for adjusting Frequency Scaling settings (implementation)
  */
// STL
#include <climits>
#include <chrono>
#include <fstream>
#include <future>
#include <stdexcept>
#include <thread>
// Qt
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QProgressDialog>
// App
#include "CpuFreqUtils.hpp"
#include "Dbg.hpp"
#include "Shell.hpp"
#include "TabMember.hpp"

/*
 * class CpuFreqUtils
 */

/**
  * @param c
  * Reference to the global CpuInfo instance.
  * @param i
  * Reference to the global CpuId instance.
  * @param v
  * Reference to our TabMemberValues instance.
  * @param s
  * Reference to our TabMemberSettings instance.
  * @param p
  * Pointer to the parent widget.
  */
CpuFreqUtils::CpuFreqUtils(
  const CpuInfo& c, const CpuId& i, TabValues& v, TabSettings& s, QWidget* p)
  : TabMemberTemplateAllCpus(c, i, v, s, p, false),
    shell_(this, "Adjust Frequency Scaling Settings",
        ShellCommand::EnableStd | ShellCommand::DisableAbortButton) {

  /* Create a tab for every processor */

  auto* tabs = new QTabWidget();

  for (PhysCpuNr p(0); p.value < cpuInfo().size(); ++p) {
    processors_.push_back(new Processor(
        *this,
        cpuInfo().at(p),
        cpuId().at(p),
        tabValues().at(p),
        tabSettings().at(p)
    ));
    QString qs("Processor ");
    qs.reserve(10);
    qs += QString::number(p.value);
    tabs->addTab(processors_.back(), qs);
  }

  auto* tabs_layout = new QVBoxLayout();
  tabs_layout->addWidget(tabs);

  std::string str("Adjust ");
  str.reserve(80);
  std::string drv_name;
  if (getDriverName(drv_name)) drv_name += "(null)";
  else str += drv_name;
  str += " settings";
  auto* box = new QGroupBox(QString::fromStdString(str));
  box->setLayout(tabs_layout);
  box->setFlat(true);

  /* Widget main layout */
  auto* layout = new QVBoxLayout(this);
  layout->addWidget(box);

  connect(&shell_, SIGNAL(finished()), &shell_, SLOT(accept()));
}

/** @brief Load the values for the UI items from the Settings instance. */
void CpuFreqUtils::load() {
  for (auto* processor : processors_) processor->load();
}

/** @brief Store the values for the UI items in the Settings instance. */
void CpuFreqUtils::store() {
  for (auto* processor : processors_) processor->store();
}

/** @brief Refresh the current values displayed on this tab. */
void CpuFreqUtils::refresh() {
  /* get ac/dc status */
  switch(xxx::shell_command(TabSettings::Cmd_OnAcPower)) {
    case 0: /* on ac power */
    default: /* unknown power state */
      acdc_ = true;
      break;
    case 1: /* on battery */
      acdc_ = false;
      break;
  }
  for (auto* processor : processors_) processor->refresh();
}

/** @brief (Re)Read the TabValues for this tab from the processor/system/...
  * @return False if the new TabValues differ from the current settings. */
bool CpuFreqUtils::read(std::ostringstream& err) {
  /* get ac/dc status */
  switch(xxx::shell_command(TabSettings::Cmd_OnAcPower)) {
    case 0: /* on ac power */
    default: /* unknown power state */
      acdc_ = true;
      break;
    case 1: /* on battery */
      acdc_ = false;
      break;
  }
  /* read all processors */
  bool retv = true;
  for (auto* processor : processors_) {
    if (processor->read(err) != true) retv = false;
  }
  return retv;
}

/** @brief Apply the stored (processor) values displayed on this tab. */
bool CpuFreqUtils::apply() {
  auto cmd = GenerateShellCmd(cpuInfo(), tabSettings());
  if (cmd.size()) return shell_.run(std::move(cmd));
  return true;
}

/** @brief Generate the shell command for applying the current settings. */
std::vector<std::string> CpuFreqUtils::GenerateShellCmd(
    const CpuInfo& ci, const TabSettings& ts) {
  std::vector<std::string> cmd { TabSettings::ScriptPath, "-v" };
  bool do_exec = false;

  /* get ac/dc status */
  bool acdc = false;
  switch(xxx::shell_command(TabSettings::Cmd_OnAcPower)) {
    case 0: /* on ac power */
    default: /* unknown power state */
      acdc = true;
      break;
    case 1: /* on battery */
      acdc = false;
      break;
  }

  /* Loop over all physical processors */
  for (PhysCpuNr p(0); p.value < ci.size(); ++p) {
    const CpuFreqUtils::Settings& settings = ts[p];
    /* Adjust settings? */
    if (settings.gFrequencyScalingEnabled()) {
      /* Adjust per-processor or per-thread? */
      if (settings.gFrequencyScalingPerCpuEnabled()) {
        /* Adjust per-thread, loop over all logical cpus: */
        for (auto& cpu : ci[p].getLogicalCpu()) {
          /* Apply adjustments for this thread */
          if (settings.frequencyScalingEnabled(cpu)) {
            do_exec = true;
            cmd.emplace_back("--fscale");
            cmd.emplace_back(std::to_string(cpu.value));
            if (acdc == true) {
              /* on AC power */
              /* Apply AC settings */
              cmd.emplace_back(settings.frequencyScalingGovernorAc(cpu));
              if (settings.frequencyScalingGovernorAc(cpu).compare("userspace") == 0) {
                cmd.emplace_back(std::to_string(
                    settings.frequencyScalingFreqAc(cpu)));
              }
              else {
                cmd.emplace_back(std::to_string(
                    settings.frequencyScalingMinFreqAc(cpu)));
                cmd.emplace_back(std::to_string(
                    settings.frequencyScalingMaxFreqAc(cpu)));
              }
            }
            else {
              /* on DC power */
              if (settings.frequencyScalingBatteryEnabled(cpu)) {
                /* Apply DC settings */
                cmd.emplace_back(settings.frequencyScalingGovernorDc(cpu));
                if (settings.frequencyScalingGovernorDc(cpu).compare("userspace") == 0) {
                  cmd.emplace_back(std::to_string(
                      settings.frequencyScalingFreqDc(cpu)));
                }
                else {
                  cmd.emplace_back(std::to_string(
                      settings.frequencyScalingMinFreqDc(cpu)));
                  cmd.emplace_back(std::to_string(
                      settings.frequencyScalingMaxFreqDc(cpu)));
                }
              }
              else {
                /* Apply AC settings */
                cmd.emplace_back(settings.frequencyScalingGovernorAc(cpu));
                if (settings.frequencyScalingGovernorAc(cpu).compare("userspace") == 0) {
                  cmd.emplace_back(std::to_string(
                      settings.frequencyScalingFreqAc(cpu)));
                }
                else {
                  cmd.emplace_back(std::to_string(
                      settings.frequencyScalingMinFreqAc(cpu)));
                  cmd.emplace_back(std::to_string(
                      settings.frequencyScalingMaxFreqAc(cpu)));
                }
              }
            }
          }
        }
      }
      else {
        /* Adjust per-processor, apply settings of the first logical cpu to all cpus: */
        do_exec = true;
        auto cpu = ci[p].firstLogicalCpu();
        for (auto& target : ci[p].getLogicalCpu()) {
          cmd.emplace_back("--fscale");
          cmd.emplace_back(std::to_string(target.value));
          if (acdc == true) {
            /* on AC power */
            /* Apply AC settings */
            cmd.emplace_back(settings.frequencyScalingGovernorAc(cpu));
            if (settings.frequencyScalingGovernorAc(cpu).compare("userspace") == 0) {
              cmd.emplace_back(std::to_string(
                  settings.frequencyScalingFreqAc(cpu)));
            }
            else {
              cmd.emplace_back(std::to_string(
                  settings.frequencyScalingMinFreqAc(cpu)));
              cmd.emplace_back(std::to_string(
                  settings.frequencyScalingMaxFreqAc(cpu)));
            }
          }
          else {
            /* on DC power */
            if (settings.gFrequencyScalingBatteryEnabled()) {
              /* Apply DC settings */
              cmd.emplace_back(settings.frequencyScalingGovernorDc(cpu));
              if (settings.frequencyScalingGovernorDc(cpu).compare("userspace") == 0) {
                cmd.emplace_back(std::to_string(
                    settings.frequencyScalingFreqDc(cpu)));
              }
              else {
                cmd.emplace_back(std::to_string(
                    settings.frequencyScalingMinFreqDc(cpu)));
                cmd.emplace_back(std::to_string(
                    settings.frequencyScalingMaxFreqDc(cpu)));
              }
            }
            else {
              /* Apply AC settings */
              cmd.emplace_back(settings.frequencyScalingGovernorAc(cpu));
              if (settings.frequencyScalingGovernorAc(cpu).compare("userspace") == 0) {
                cmd.emplace_back(std::to_string(
                    settings.frequencyScalingFreqAc(cpu)));
              }
              else {
                cmd.emplace_back(std::to_string(
                    settings.frequencyScalingMinFreqAc(cpu)));
                cmd.emplace_back(std::to_string(
                    settings.frequencyScalingMaxFreqAc(cpu)));
              }
            }
          }
        }
      }
    }
  }
  if (!do_exec) cmd.clear();
  return cmd;
}

/** @brief Compare the current widget values against the current TabValues
  * @return True if the comparison is equal */
bool CpuFreqUtils::compare() {
  if (!isEnabled()) {
    DBGMSG("CpuFreqUtils::compare(): Tab was disabled by exception...")
    return true;
  }

  /* get ac/dc status */
  switch(xxx::shell_command(TabSettings::Cmd_OnAcPower)) {
    case 0: /* on ac power */
    default: /* unknown power state */
      acdc_ = true;
      break;
    case 1: /* on battery */
      acdc_ = false;
      break;
  }

  /* compare all processors, return false if any returned false */
  bool retv = true;
  for (auto* processor : processors_) {
    if (processor->compare() != true) retv = false;
  }
  return retv;
}

/** @brief Get the cpufreq driver name.
  * @param out [out] The driver name.
  * @return Zero on success, non-zero on error */
int CpuFreqUtils::getDriverName(std::string& out) {
  return xxx::shell_command(
      { Settings::CPUFREQ_INFO, "--driver" },
      [&out](auto, auto in) {
        in.pop_back();
        out = std::move(in);
        return 0;
      }
  );
}

/** @brief Emits the TabMemberWidget::valueChanged() signal. */
void CpuFreqUtils::valueChangedSlot() {
  emit valueChanged(this);
}

/*
 * class CpuFreqUtils::Processor
 */

CpuFreqUtils::Processor::Processor(
    CpuFreqUtils& _cpuFreqUtils,
    const SingleCpuInfo& _cpuInfo, const SingleCpuId&,
    TabMemberValues& _tabValues, TabMemberSettings& _tabSettings,
    QWidget* parent)
    : QWidget(parent),
      cpuFreqUtils_(_cpuFreqUtils),
      cpuInfo_(_cpuInfo),
      tabValues_(_tabValues),
      tabSettings_(_tabSettings) {
  layout_ = new QVBoxLayout(this);
  layout_->addSpacing(10);
  addWidgets();
}

void CpuFreqUtils::Processor::addWidgets() {
  /* Create a tab for every cpu + one for all cpus: */
  tabs_ = new QTabWidget();
  threads_.clear();

  /* - Add a tab that adjusts all cpus for this processor. */
  threads_.push_back(std::move(new Thread(cpuFreqUtils(), *this,
      LogicalCpuNr(ULONG_MAX), cpuInfo(), tabValues(), tabSettings())));
  tabs_->addTab(threads_.back(), "All");

  /* - Add a tab that adjusts every cpu for this processor. */
  for (auto& l : cpuInfo().getLogicalCpu()) {
    threads_.push_back(std::move(new Thread(
        cpuFreqUtils(), *this, l, cpuInfo(), tabValues(), tabSettings())));
    tabs_->addTab(threads_.back(), std::move(QString("cpu%1").arg(l.value)));
  }

  layout_->addWidget(tabs_);
}

void CpuFreqUtils::Processor::removeWidgets() {
  for (size_t i = 0; i < threads_.size(); ++i) {
    tabs_->removeTab(static_cast<int>(i));
  }
  for (auto* thread : threads_) {
    thread->setParent(nullptr);
    delete thread;
  }
  layout_->removeWidget(tabs_);
  tabs_->setParent(nullptr);
  delete tabs_;
}

void CpuFreqUtils::Processor::load() {
  for (auto* thread : threads_) thread->load();
}

void CpuFreqUtils::Processor::store() {
  for (auto* thread : threads_) thread->store();
}

void CpuFreqUtils::Processor::refresh() {
  for (auto* thread : threads_) thread->refresh();
}

bool CpuFreqUtils::Processor::read(std::ostringstream& err) {
  if (threads_.size() - 1 != cpuInfo().getLogicalCpu().size()) {
    DBGMSG("CpuFreqUtils::Processor::read(): Detected a different number of cpus!")
    removeWidgets();
    addWidgets();
    load();
  }
  bool retv = true;
  for (auto* thread : threads_) {
    if (thread->read(err) != true) retv = false;
  }
  return retv;
}

bool CpuFreqUtils::Processor::compare() {
  bool retv = true;
  for (auto* thread : threads_) {
    if (thread->compare() != true) retv = false;
  }
  return retv;
}

/*
 * class CpuFreqUtils::Thread
 */

CpuFreqUtils::Processor::Thread::Thread(
    CpuFreqUtils& _cpuFreqUtils,
    CpuFreqUtils::Processor& _processor,
    const LogicalCpuNr& cpu,
    const SingleCpuInfo& _cpuInfo,
    TabMemberValues& _tabValues,
    TabMemberSettings& _tabSettings,
    QWidget* parent)
    : QWidget(parent),
      cpu_(cpu),
      cpuFreqUtils_(_cpuFreqUtils),
      processor_(_processor),
      cpuInfo_(_cpuInfo),
      tabValues_(_tabValues),
      tabSettings_(_tabSettings) {

  /*
   * Create the inner widgets.
   */

  QRect charBounds_ = fontMetrics().boundingRect("Max. Frequency");

  cb_battery_ = new QCheckBox("Seperate settings while on battery power.");
  cb_adjust_ = new QCheckBox("Adjust frequency-scaling settings.");

  cb_all_ = nullptr;
  if (cpu_.value == ULONG_MAX) {
    cb_all_ = new QCheckBox("Adjust each cpu seperately.");
  }

  auto layout_cb = new QHBoxLayout();
  layout_cb->addWidget(cb_adjust_, 0);
  if (cb_all_) layout_cb->addWidget(cb_all_, 0);

  /* If cb_all_ != nullptr || cpu_ == -1 then we are the per-processor tab! */

  /* Governor select AC */

  governors_ac_ = new SaferCombo();
  auto* governors_layout_ac = new QVBoxLayout();
  governors_layout_ac->addWidget(governors_ac_);

  /* Min/max frequency AC: */

  minLabel_ac_ = new QLabel("Min. Frequency");
  minLabel_ac_->setMinimumWidth(charBounds_.width());
  minFrequency_ac_ = new SaferSlider(Qt::Horizontal);
  minValue_ac_ = new QLabel();
  minValue_ac_->setMinimumWidth(80);
  auto* min_ac = new QHBoxLayout();
  min_ac->addWidget(minLabel_ac_, 0);
  min_ac->addWidget(minFrequency_ac_, 1);
  min_ac->addWidget(minValue_ac_, 0);

  maxLabel_ac_ = new QLabel("Max. Frequency");
  maxLabel_ac_->setMinimumWidth(charBounds_.width());
  maxFrequency_ac_ = new SaferSlider(Qt::Horizontal);
  maxValue_ac_ = new QLabel();
  maxValue_ac_->setMinimumWidth(80);
  auto* max_ac = new QHBoxLayout();
  max_ac->addWidget(maxLabel_ac_, 0);
  max_ac->addWidget(maxFrequency_ac_, 1);
  max_ac->addWidget(maxValue_ac_, 0);

  auto* minmax_layout_ac = new QVBoxLayout();
  minmax_layout_ac->addLayout(min_ac);
  minmax_layout_ac->addLayout(max_ac);

  /* Current userspace frequency AC: */

  frequencyLabel_ac_ = new QLabel("Frequency");
  frequencyLabel_ac_->setMinimumWidth(charBounds_.width());
  frequency_ac_ = new SaferSlider(Qt::Horizontal);
  frequencyValue_ac_ = new QLabel();
  frequencyValue_ac_->setMinimumWidth(80);
  auto* frequency_layout_ac = new QHBoxLayout();
  frequency_layout_ac->addWidget(frequencyLabel_ac_, 0);
  frequency_layout_ac->addWidget(frequency_ac_, 1);
  frequency_layout_ac->addWidget(frequencyValue_ac_, 0);

  auto* ac_box_layout = new QVBoxLayout();
  ac_box_layout->addSpacing(10);
  ac_box_layout->addLayout(governors_layout_ac);
  ac_box_layout->addSpacing(20);
  ac_box_layout->addLayout(minmax_layout_ac);
  ac_box_layout->addSpacing(20);
  ac_box_layout->addLayout(frequency_layout_ac);
  ac_box_layout->addSpacing(10);

  ac_box_ = new QGroupBox("AC Powered");
  ac_box_->setFlat(true);
  ac_box_->setLayout(ac_box_layout);

  /* Governor select DC */

  governors_dc_ = new SaferCombo();
  auto* governors_layout_dc = new QVBoxLayout();
  governors_layout_dc->addWidget(governors_dc_);

  /* Min/max frequency DC: */

  minLabel_dc_ = new QLabel("Min. Frequency");
  minLabel_dc_->setMinimumWidth(charBounds_.width());
  minFrequency_dc_ = new SaferSlider(Qt::Horizontal);
  minValue_dc_ = new QLabel();
  minValue_dc_->setMinimumWidth(80);
  auto* min_dc = new QHBoxLayout();
  min_dc->addWidget(minLabel_dc_, 0);
  min_dc->addWidget(minFrequency_dc_, 1);
  min_dc->addWidget(minValue_dc_, 0);

  maxLabel_dc_ = new QLabel("Max. Frequency");
  maxLabel_dc_->setMinimumWidth(charBounds_.width());
  maxFrequency_dc_ = new SaferSlider(Qt::Horizontal);
  maxValue_dc_ = new QLabel();
  maxValue_dc_->setMinimumWidth(80);
  auto* max_dc = new QHBoxLayout();
  max_dc->addWidget(maxLabel_dc_, 0);
  max_dc->addWidget(maxFrequency_dc_, 1);
  max_dc->addWidget(maxValue_dc_, 0);

  auto* minmax_layout_dc = new QVBoxLayout();
  minmax_layout_dc->addLayout(min_dc);
  minmax_layout_dc->addLayout(max_dc);

  /* Current userspace frequency DC: */

  frequencyLabel_dc_ = new QLabel("Frequency");
  frequencyLabel_dc_->setMinimumWidth(charBounds_.width());
  frequency_dc_ = new SaferSlider(Qt::Horizontal);
  frequencyValue_dc_ = new QLabel();
  frequencyValue_dc_->setMinimumWidth(80);
  auto* frequency_layout_dc = new QHBoxLayout();
  frequency_layout_dc->addWidget(frequencyLabel_dc_, 0);
  frequency_layout_dc->addWidget(frequency_dc_, 1);
  frequency_layout_dc->addWidget(frequencyValue_dc_, 0);

  auto* dc_box_layout = new QVBoxLayout();
  dc_box_layout->addSpacing(10);
  dc_box_layout->addLayout(governors_layout_dc);
  dc_box_layout->addSpacing(20);
  dc_box_layout->addLayout(minmax_layout_dc);
  dc_box_layout->addSpacing(20);
  dc_box_layout->addLayout(frequency_layout_dc);
  dc_box_layout->addSpacing(10);

  dc_box_ = new QGroupBox("DC Powered");
  dc_box_->setFlat(true);
  dc_box_->setLayout(dc_box_layout);

  /*
   * Get the list of available cpu governors and the hw frequency limits.
   */

  enabled_ = true;

  if (getGovernors(governors_list_)) {
    DBGMSG("Failed to get cpufreq governors.")
    enabled_ = false;
  }
  else {
    governors_ac_->addItems(governors_list_);
    governors_dc_->addItems(governors_list_);
  }
  if (getHwFreqLimits(hw_min_, hw_max_)) {
    DBGMSG("Failed to get cpufreq hwlimits")
    enabled_ = false;
  }
  hw_min_ /= 1000;
  hw_max_ /= 1000;

  /* Disable widgets on error */
  if (enabled_ == false) {
    minFrequency_ac_->setEnabled(false);
    maxFrequency_ac_->setEnabled(false);
    governors_ac_->setEnabled(false);
    frequency_ac_->setEnabled(false);
  }

  /* Get the index of the userspace governor */
  userspace_idx_ = 0;
  for (auto& g : governors_list_) {
    if (g.compare("userspace") == 0) break;
    ++userspace_idx_;
  }

  /*
   * Set UI item ranges and bounds.
   */

  /* Calculate the widest slider label width. */
  charBounds_ = fontMetrics().boundingRect(QString("%1 MHz").arg(hw_max_));

  /* Set the sliders label width to the width of the widest. */
  minValue_ac_->setMinimumWidth(charBounds_.width());
  maxValue_ac_->setMinimumWidth(charBounds_.width());
  frequencyValue_ac_->setMinimumWidth(charBounds_.width());
  minValue_dc_->setMinimumWidth(charBounds_.width());
  maxValue_dc_->setMinimumWidth(charBounds_.width());
  frequencyValue_dc_->setMinimumWidth(charBounds_.width());

  /* Set sliders range */
  minFrequency_ac_->setRange(
      hw_min_ / SliderStepping, (hw_max_ / SliderStepping) + 1);
  maxFrequency_ac_->setRange(
      hw_min_ / SliderStepping, (hw_max_ / SliderStepping) + 1);
  frequency_ac_->setRange(
      hw_min_ / SliderStepping, (hw_max_ / SliderStepping) + 1);
  minFrequency_dc_->setRange(
      hw_min_ / SliderStepping, (hw_max_ / SliderStepping) + 1);
  maxFrequency_dc_->setRange(
      hw_min_ / SliderStepping, (hw_max_ / SliderStepping) + 1);
  frequency_dc_->setRange(
      hw_min_ / SliderStepping, (hw_max_ / SliderStepping) + 1);

  /* Set sliders page-up/down step-size (to 10 * 10 MHz) */
  minFrequency_ac_->setPageStep(10);
  maxFrequency_ac_->setPageStep(10);
  frequency_ac_->setPageStep(10);
  minFrequency_dc_->setPageStep(10);
  maxFrequency_dc_->setPageStep(10);
  frequency_dc_->setPageStep(10);

  /*
   * Assemble the layout of this widget
   */

  scroll_layout_ = new QVBoxLayout();
  scroll_layout_->addStretch();
  scroll_layout_->addWidget(ac_box_);
  scroll_layout_->addStretch();
  scroll_layout_->addWidget(dc_box_);
  scroll_layout_->addStretch();

  scroll_widget_ = new QWidget();
  scroll_widget_->setLayout(scroll_layout_);
  scroll_area_ = new QScrollArea();
  scroll_area_->setWidget(scroll_widget_);
  scroll_area_->setWidgetResizable(true);

  auto* layout = new QVBoxLayout(this);
  layout->addWidget(scroll_area_);
  layout->addWidget(cb_battery_);
  layout->addLayout(layout_cb);

  /*
   * Connect Signals to Slots
   */

  if (enabled_) {
    connect(governors_ac_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(governorAcIndexChanged(int)));

    connect(minFrequency_ac_, SIGNAL(valueChanged(int)),
        this, SLOT(minFreqAcValueChanged(int)));

    connect(maxFrequency_ac_, SIGNAL(valueChanged(int)),
        this, SLOT(maxFreqAcValueChanged(int)));

    connect(frequency_ac_, SIGNAL(valueChanged(int)),
        this, SLOT(usrFreqAcValueChanged(int)));

    connect(governors_dc_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(governorDcIndexChanged(int)));

    connect(minFrequency_dc_, SIGNAL(valueChanged(int)),
        this, SLOT(minFreqDcValueChanged(int)));

    connect(maxFrequency_dc_, SIGNAL(valueChanged(int)),
        this, SLOT(maxFreqDcValueChanged(int)));

    connect(frequency_dc_, SIGNAL(valueChanged(int)),
        this, SLOT(usrFreqDcValueChanged(int)));

    connect(cb_adjust_, SIGNAL(stateChanged(int)),
        this, SLOT(adjustStateChanged(int)));

    connect(cb_battery_, SIGNAL(stateChanged(int)),
        this, SLOT(batteryStateChanged(int)));

    if (cb_all_) {
      connect(cb_all_, SIGNAL(stateChanged(int)),
          this, SLOT(allStateChanged(int)));
    }
  }
}

QSize	CpuFreqUtils::Processor::Thread::sizeHint() const {
  auto size = scroll_widget_->sizeHint();
  return QSize(size.width() + 50, size.height() + 50);
}

unsigned int CpuFreqUtils::Processor::Thread::sliderValueFromFreq(unsigned int freq) {
  /* clamp to a valid frequency */
  unsigned int f = freq;
  if (f < hw_min_) f = hw_min_;
  else if (f > hw_max_) f = hw_max_;
  /* true if the clamped value matches the input value */
  bool b = (f == freq);
  /* divide the known good frequency by the step size */
  f /= SliderStepping;
  /* This enables turbo boost if it was allready at the max non-turbo frequency
   * (ie. this adds the extra +1 MHz) */
  if (b && (f * SliderStepping) != freq) ++f;
  return f;
}

unsigned int CpuFreqUtils::Processor::Thread::freqFromSliderValue(unsigned int value) {
  value *= SliderStepping;
  if (value > hw_max_) value = hw_max_;
  return value;
}

void CpuFreqUtils::Processor::Thread::load() {
  /*
   * Load the data from the Settings instance to the UI items.
   */
  governors_ac_->blockSignals(true);
  minFrequency_ac_->blockSignals(true);
  maxFrequency_ac_->blockSignals(true);
  frequency_ac_->blockSignals(true);
  governors_dc_->blockSignals(true);
  minFrequency_dc_->blockSignals(true);
  maxFrequency_dc_->blockSignals(true);
  frequency_dc_->blockSignals(true);
  cb_adjust_->blockSignals(true);
  cb_battery_->blockSignals(true);
  if (cb_all_) cb_all_->blockSignals(true);

  LogicalCpuNr cpu(
      cpu_.value == ULONG_MAX ? cpuInfo().firstLogicalCpu() : cpu_);

  /* Governor select AC */

  int current_governor = 0;
  bool found = false;
  auto& c_ac = tabSettings().frequencyScalingGovernorAc(cpu);
  for (auto& g : governors_list_) {
    if (g.compare(c_ac.c_str()) == 0) {
      found = true;
      break;
    }
    ++current_governor;
  }
  if (found == false) current_governor = 0;
  governors_ac_->setCurrentIndex(current_governor);

  /* Min/max frequency AC: */

  unsigned int min = sliderValueFromFreq(tabSettings().frequencyScalingMinFreqAc(cpu));
  minFrequency_ac_->setValue(static_cast<int>(min));
  minValue_ac_->setText(
      std::move(QString("%1 MHz").arg(freqFromSliderValue(min))));

  unsigned int max = sliderValueFromFreq(tabSettings().frequencyScalingMaxFreqAc(cpu));
  maxFrequency_ac_->setValue(static_cast<int>(max));
  maxValue_ac_->setText(
      std::move(QString("%1 MHz").arg(freqFromSliderValue(max))));

  /* Current userspace frequency AC: */

  unsigned int freq = sliderValueFromFreq(tabSettings().frequencyScalingFreqAc(cpu));
  frequency_ac_->setValue(static_cast<int>(freq));
  frequencyValue_ac_->setText(
      std::move(QString("%1 MHz").arg(freqFromSliderValue(freq))));

  /* Governor select DC */

  current_governor = 0;
  found = false;
  auto& c_dc = tabSettings().frequencyScalingGovernorDc(cpu);
  for (auto& g : governors_list_) {
    if (g.compare(c_dc.c_str()) == 0) {
      found = true;
      break;
    }
    ++current_governor;
  }
  if (found == false) current_governor = 0;
  governors_dc_->setCurrentIndex(current_governor);

  /* Min/max frequency DC: */

  min = sliderValueFromFreq(tabSettings().frequencyScalingMinFreqDc(cpu));
  minFrequency_dc_->setValue(static_cast<int>(min));
  minValue_dc_->setText(
      std::move(QString("%1 MHz").arg(freqFromSliderValue(min))));

  max = sliderValueFromFreq(tabSettings().frequencyScalingMaxFreqDc(cpu));
  maxFrequency_dc_->setValue(static_cast<int>(max));
  maxValue_dc_->setText(
      std::move(QString("%1 MHz").arg(freqFromSliderValue(max))));

  /* Current userspace frequency DC: */

  freq = sliderValueFromFreq(tabSettings().frequencyScalingFreqDc(cpu));
  frequency_dc_->setValue(static_cast<int>(freq));
  frequencyValue_dc_->setText(
      std::move(QString("%1 MHz").arg(freqFromSliderValue(freq))));

  /* checkboxes */

  if (cpu_.value == ULONG_MAX) {
    /* this is the per-processor tab */
    cb_all_->setChecked(tabSettings().gFrequencyScalingPerCpuEnabled());
    cb_battery_->setChecked(tabSettings().gFrequencyScalingBatteryEnabled());
    cb_adjust_->setChecked(tabSettings().gFrequencyScalingEnabled());
  }
  else {
    /* this is a per-cpu tab */
    cb_battery_->setChecked(tabSettings().frequencyScalingBatteryEnabled(cpu));
    cb_adjust_->setChecked(tabSettings().frequencyScalingEnabled(cpu));
  }

  if (cb_all_) cb_all_->blockSignals(false);
  cb_battery_->blockSignals(false);
  cb_adjust_->blockSignals(false);
  frequency_dc_->blockSignals(false);
  maxFrequency_dc_->blockSignals(false);
  minFrequency_dc_->blockSignals(false);
  governors_dc_->blockSignals(false);
  frequency_ac_->blockSignals(false);
  maxFrequency_ac_->blockSignals(false);
  minFrequency_ac_->blockSignals(false);
  governors_ac_->blockSignals(false);
}

void CpuFreqUtils::Processor::Thread::store() {
  /*
   * Store the current UI item values to Settings
   */
  store(tabSettings());
}

void CpuFreqUtils::Processor::Thread::store(CpuFreqUtils::Settings& settings) {
  LogicalCpuNr cpu(
      cpu_.value == ULONG_MAX ? cpuInfo().firstLogicalCpu() : cpu_);

  /*
   * Store the current UI item values to the specified Settings instance
   */

  if (cb_all_) {
    /* this is a per-processor tab */
    if (cb_adjust_->isChecked()) {
      /* AC */
      settings.gFrequencyScalingEnabled(true);
      settings.frequencyScalingGovernorAc(
          cpu, governors_list_[governors_ac_->currentIndex()].toStdString());
      if (governors_ac_->currentIndex() == userspace_idx_) {
        settings.frequencyScalingFreqAc(
            cpu, freqFromSliderValue(static_cast<unsigned>(frequency_ac_->value())));
      }
      else {
        settings.frequencyScalingMinFreqAc(
            cpu, freqFromSliderValue(static_cast<unsigned>(minFrequency_ac_->value())));
        settings.frequencyScalingMaxFreqAc(
            cpu, freqFromSliderValue(static_cast<unsigned>(maxFrequency_ac_->value())));
      }
      /* DC */
      settings.frequencyScalingGovernorDc(
          cpu, governors_list_[governors_dc_->currentIndex()].toStdString());
      if (cb_battery_->isChecked()) {
        settings.gFrequencyScalingBatteryEnabled(true);
        if (governors_dc_->currentIndex() == userspace_idx_) {
          settings.frequencyScalingFreqDc(
              cpu, freqFromSliderValue(static_cast<unsigned>(frequency_dc_->value())));
        }
        else {
          settings.frequencyScalingMinFreqDc(
              cpu, freqFromSliderValue(static_cast<unsigned>(minFrequency_dc_->value())));
          settings.frequencyScalingMaxFreqDc(
              cpu, freqFromSliderValue(static_cast<unsigned>(maxFrequency_dc_->value())));
        }
      }
      else {
        settings.gFrequencyScalingBatteryEnabled(false);
      }
      /* per thread? */
      settings.gFrequencyScalingPerCpuEnabled(cb_all_->isChecked());
    }
    else {
      settings.gFrequencyScalingEnabled(false);
    }
  }
  else {
    /* this is a per-thread tab */
    if (settings.gFrequencyScalingEnabled() &&
        settings.gFrequencyScalingPerCpuEnabled() &&
        cb_adjust_->isChecked()) {
      /* AC */
      settings.frequencyScalingEnabled(cpu, true);
      settings.frequencyScalingGovernorAc(
          cpu, governors_list_[governors_ac_->currentIndex()].toStdString());
      if (governors_ac_->currentIndex() == userspace_idx_) {
        settings.frequencyScalingFreqAc(
            cpu, freqFromSliderValue(static_cast<unsigned>(frequency_ac_->value())));
      }
      else {
        settings.frequencyScalingMinFreqAc(
            cpu, freqFromSliderValue(static_cast<unsigned>(minFrequency_ac_->value())));
        settings.frequencyScalingMaxFreqAc(
            cpu, freqFromSliderValue(static_cast<unsigned>(maxFrequency_ac_->value())));
      }
      /* DC */
      settings.frequencyScalingGovernorDc(
          cpu, governors_list_[governors_dc_->currentIndex()].toStdString());
      if (cb_battery_->isChecked()) {
        settings.frequencyScalingBatteryEnabled(cpu, true);
        if (governors_dc_->currentIndex() == userspace_idx_) {
          settings.frequencyScalingFreqDc(
              cpu, freqFromSliderValue(static_cast<unsigned>(frequency_dc_->value())));
        }
        else {
          settings.frequencyScalingMinFreqDc(
              cpu, freqFromSliderValue(static_cast<unsigned>(minFrequency_dc_->value())));
          settings.frequencyScalingMaxFreqDc(
              cpu, freqFromSliderValue(static_cast<unsigned>(maxFrequency_dc_->value())));
        }
      }
      else {
        settings.frequencyScalingBatteryEnabled(cpu, false);
      }
    }
    else {
      settings.frequencyScalingEnabled(cpu, false);
    }
  }
}

void CpuFreqUtils::Processor::Thread::refresh() {
  /*
   * Refresh the UI items using the current Values.
   */
  governors_ac_->blockSignals(true);
  minFrequency_ac_->blockSignals(true);
  maxFrequency_ac_->blockSignals(true);
  frequency_ac_->blockSignals(true);
  governors_dc_->blockSignals(true);
  minFrequency_dc_->blockSignals(true);
  maxFrequency_dc_->blockSignals(true);
  frequency_dc_->blockSignals(true);
  cb_adjust_->blockSignals(true);
  cb_battery_->blockSignals(true);
  if (cb_all_) cb_all_->blockSignals(true);

  LogicalCpuNr cpu(
      cpu_.value == ULONG_MAX ? cpuInfo().firstLogicalCpu() : cpu_);

  if (cpuFreqUtils().acdc() == true) {
    /* AC powered */

    int idx = 0;
    auto& g_ac = tabValues().frequencyScalingGovernorAc(cpu);
    for (auto& g : governors_list_) {
      if (g.compare(g_ac.c_str()) == 0) break;
      ++idx;
    }
    governors_ac_->setCurrentIndex(idx);

    unsigned int value = sliderValueFromFreq(tabValues().frequencyScalingMinFreqAc(cpu));
    minFrequency_ac_->setValue(static_cast<int>(value));
    QString qs(QString::number(freqFromSliderValue(value)));
    qs += " MHz";
    minValue_ac_->setText(qs);

    value = sliderValueFromFreq(tabValues().frequencyScalingMaxFreqAc(cpu));
    maxFrequency_ac_->setValue(static_cast<int>(value));
    qs = QString::number(freqFromSliderValue(value));
    qs += " MHz";
    maxValue_ac_->setText(qs);

    value = sliderValueFromFreq(tabValues().frequencyScalingFreqAc(cpu));
    frequency_ac_->setValue(static_cast<int>(value));
    qs = QString::number(freqFromSliderValue(value));
    qs += " MHz";
    frequencyValue_ac_->setText(qs);
  }
  else {
    /* DC powered */

    int idx = 0;
    auto& g_dc = tabValues().frequencyScalingGovernorDc(cpu);
    for (auto& g : governors_list_) {
      if (g.compare(g_dc.c_str()) == 0) break;
      ++idx;
    }
    governors_dc_->setCurrentIndex(idx);

    unsigned int value = sliderValueFromFreq(tabValues().frequencyScalingMinFreqDc(cpu));
    minFrequency_dc_->setValue(static_cast<int>(value));
    minValue_dc_->setText(
        std::move(QString("%1 MHz").arg(freqFromSliderValue(value))));

    value = sliderValueFromFreq(tabValues().frequencyScalingMaxFreqDc(cpu));
    maxFrequency_dc_->setValue(static_cast<int>(value));
    maxValue_dc_->setText(
        std::move(QString("%1 MHz").arg(freqFromSliderValue(value))));

    value = sliderValueFromFreq(tabValues().frequencyScalingFreqDc(cpu));
    frequency_dc_->setValue(static_cast<int>(value));
    frequencyValue_dc_->setText(
        std::move(QString("%1 MHz").arg(freqFromSliderValue(value))));
  }

  /* Checkboxes */
  if (cb_all_) {
    /* this is the per-processor tab */
    cb_adjust_->setChecked(tabSettings().gFrequencyScalingEnabled());
    cb_battery_->setChecked(tabSettings().gFrequencyScalingBatteryEnabled());
    cb_all_->setChecked(tabSettings().gFrequencyScalingPerCpuEnabled());
  }
  else {
    /* this is a per-cpu tab */
    cb_adjust_->setChecked(tabSettings().frequencyScalingEnabled(cpu));
    cb_battery_->setChecked(tabSettings().frequencyScalingBatteryEnabled(cpu));
  }

  /* Enable/disable widgets */

  if (cb_all_) {
    /* This is the per-processor tab */
    if (cb_adjust_->isChecked()) {
      cb_all_->setEnabled(true);
    }
    else {
      cb_all_->setEnabled(false);
      for (size_t i = 1; i <= processor().threads().size(); ++i) {
        processor().tabs()->setTabEnabled(static_cast<int>(i), false);
      }
    }
    if (cb_all_->isChecked()) {
      /* per thread-tabs enabled: disable this tab and enable all other tabs */
      ac_box_->setEnabled(false);
      cb_battery_->setEnabled(false);
      dc_box_->setEnabled(false);
      if (cb_adjust_->isChecked()) {
        for (size_t i = 1; i <= processor().threads().size(); ++i) {
          processor().tabs()->setTabEnabled(static_cast<int>(i), true);
        }
      }
    }
    else {
      /* per thread-tabs disabled: enable this tab and disable all other tabs */
      if (cb_adjust_->isChecked()) {
        ac_box_->setEnabled(true);
        cb_battery_->setEnabled(true);
        if (cb_battery_->isChecked()) {
          dc_box_->setEnabled(true);
        }
        else {
          dc_box_->setEnabled(false);
        }
      }
      else {
        ac_box_->setEnabled(false);
        cb_battery_->setEnabled(false);
        dc_box_->setEnabled(false);
      }
      for (size_t i = 1; i <= processor().threads().size(); ++i) {
        processor().tabs()->setTabEnabled(static_cast<int>(i), false);
      }
    }
  }
  else {
    /* This is a per-thread tab: */
    if (cb_adjust_->isChecked()) {
      ac_box_->setEnabled(true);
      cb_battery_->setEnabled(true);
      if (cb_battery_->isChecked()) dc_box_->setEnabled(true);
      else dc_box_->setEnabled(false);
    }
    else {
      ac_box_->setEnabled(false);
      cb_battery_->setEnabled(false);
      dc_box_->setEnabled(false);
    }
  }

  /* userspace selected? */
  if (governors_ac_->currentIndex() == userspace_idx_) {
    minFrequency_ac_->setEnabled(false);
    minLabel_ac_->setEnabled(false);
    minValue_ac_->setEnabled(false);
    maxFrequency_ac_->setEnabled(false);
    maxLabel_ac_->setEnabled(false);
    maxValue_ac_->setEnabled(false);
    frequency_ac_->setEnabled(true);
    frequencyLabel_ac_->setEnabled(true);
    frequencyValue_ac_->setEnabled(true);
  }
  else {
    minFrequency_ac_->setEnabled(true);
    minLabel_ac_->setEnabled(true);
    minValue_ac_->setEnabled(true);
    maxFrequency_ac_->setEnabled(true);
    maxLabel_ac_->setEnabled(true);
    maxValue_ac_->setEnabled(true);
    frequency_ac_->setEnabled(false);
    frequencyLabel_ac_->setEnabled(false);
    frequencyValue_ac_->setEnabled(false);
  }
  if (governors_dc_->currentIndex() == userspace_idx_) {
    minFrequency_dc_->setEnabled(false);
    minLabel_dc_->setEnabled(false);
    minValue_dc_->setEnabled(false);
    maxFrequency_dc_->setEnabled(false);
    maxLabel_dc_->setEnabled(false);
    maxValue_dc_->setEnabled(false);
    frequency_dc_->setEnabled(true);
    frequencyLabel_dc_->setEnabled(true);
    frequencyValue_dc_->setEnabled(true);
  }
  else {
    minFrequency_dc_->setEnabled(true);
    minLabel_dc_->setEnabled(true);
    minValue_dc_->setEnabled(true);
    maxFrequency_dc_->setEnabled(true);
    maxLabel_dc_->setEnabled(true);
    maxValue_dc_->setEnabled(true);
    frequency_dc_->setEnabled(false);
    frequencyLabel_dc_->setEnabled(false);
    frequencyValue_dc_->setEnabled(false);
  }

  if (cb_all_) cb_all_->blockSignals(false);
  cb_battery_->blockSignals(false);
  cb_adjust_->blockSignals(false);
  frequency_dc_->blockSignals(false);
  maxFrequency_dc_->blockSignals(false);
  minFrequency_dc_->blockSignals(false);
  governors_dc_->blockSignals(false);
  frequency_ac_->blockSignals(false);
  maxFrequency_ac_->blockSignals(false);
  minFrequency_ac_->blockSignals(false);
  governors_ac_->blockSignals(false);
}

bool CpuFreqUtils::Processor::Thread::read(std::ostringstream& ss) {

  /* return true if the tab is disabled */
  if (!enabled_) return true;

  /*
   * Read the Values for this cpu from the processor
   */

  bool retv = true;

  LogicalCpuNr cpu(
      cpu_.value == ULONG_MAX ? cpuInfo().firstLogicalCpu() : cpu_);

  /* extract current values */
  std::string selected_governor;
  unsigned int current_min = 0;
  unsigned int current_max = 0;
  unsigned int current_freq = 0;
  if (getFreqLimitsAndPolicy(current_min, current_max, selected_governor)) {
    DBGMSG("Failed to get cpufreq policy!")
  }
  if (getHwFreq(current_freq)) {
    DBGMSG("Failed to get cpufreq frequency!")
    current_freq = 0;
  }
  current_governor_ = 0;
  for (auto& g : governors_list_) {
    if (g.compare(selected_governor.c_str()) == 0) {
      break;
    }
    ++current_governor_;
  }
  /* Convert to MHz */
  current_min /= 1000;
  current_max /= 1000;
  current_freq /= 1000;

  if (cpuFreqUtils().acdc() == true) {
    /* AC powered */
    tabValues().frequencyScalingGovernorAc(cpu, selected_governor);
    tabValues().frequencyScalingMinFreqAc(cpu, current_min);
    tabValues().frequencyScalingMaxFreqAc(cpu, current_max);
    tabValues().frequencyScalingFreqAc(cpu, current_freq);
  }
  else {
    /* DC powered */
    tabValues().frequencyScalingGovernorDc(cpu, selected_governor);
    tabValues().frequencyScalingMinFreqDc(cpu, current_min);
    tabValues().frequencyScalingMaxFreqDc(cpu, current_max);
    tabValues().frequencyScalingFreqDc(cpu, current_freq);
  }

  /*
   * Compare the newly read Values against the stored Settings
   */

  bool enabled = false;
  if (cb_all_) {
    /* This is the per-processor tab, is it enabled? */
    if (tabSettings().gFrequencyScalingEnabled()) {
      enabled = true;
    }
  }
  else {
    /* This is a per-thread tab, is it enabled? */
    if (tabSettings().gFrequencyScalingEnabled() &&
        tabSettings().gFrequencyScalingPerCpuEnabled() &&
        tabSettings().frequencyScalingEnabled(cpu)) {
      enabled = true;
    }
  }
  /* If this tab is enabled then test the Values we just read against the Settings. */
  if (enabled == true) {
    /* Compare against AD or DC settings? */
    if (cpuFreqUtils().acdc() == true) {
      /* AC powered. Compare the governor. */
      if (tabSettings().frequencyScalingGovernorAc(cpu).compare(selected_governor) != 0) {
        ss << "<li><nobr>The '<b>Frequency Scaling Governor (AC)</b>' for "
           << ((cpu_.value == ULONG_MAX) ? "Processor " : "cpu ")
           << ((cpu_.value == ULONG_MAX) ? cpuInfo().physicalId().value : cpu_.value)
           << " is <b>" << selected_governor << "</b> instead of <b>"
           << tabSettings().frequencyScalingGovernorAc(cpu)
           << "</b>.</nobr></li>";
        /* Governor does not match */
        retv = false;
      }
      /* Is the 'userspace' governor selected? */
      if (tabSettings().frequencyScalingGovernorAc(cpu).compare("userspace") != 0) {
        /* No, compare the min/max frequencies. */
        if (tabSettings().frequencyScalingMinFreqAc(cpu) != current_min) {
          ss << "<li><nobr>The '<b>Minimum Scaling Frequency (AC)</b>' for "
             << ((cpu_.value == ULONG_MAX) ? "Processor " : "cpu ")
             << ((cpu_.value == ULONG_MAX) ? cpuInfo().physicalId().value : cpu_.value)
             << " is <b>" << current_min << "</b> instead of <b>"
             << tabSettings().frequencyScalingMinFreqAc(cpu)
             << "</b>.</nobr></li>";
          /* Minimum frequency does not match. */
          retv = false;
        }
        if (tabSettings().frequencyScalingMaxFreqAc(cpu) != current_max) {
          ss << "<li><nobr>The '<b>Maximum Scaling Frequency (AC)</b>' for "
             << ((cpu_.value == ULONG_MAX) ? "Processor " : "cpu ")
             << ((cpu_.value == ULONG_MAX) ? cpuInfo().physicalId().value : cpu_.value)
             << " is <b>" << current_max << "</b> instead of <b>"
             << tabSettings().frequencyScalingMaxFreqAc(cpu)
             << "</b>.</nobr></li>";
          /* Maximum frequency does not match. */
          retv = false;
        }
      }
      else {
        /* Yes, compare the frequency. */
        if (tabSettings().frequencyScalingFreqAc(cpu) != current_freq) {
          ss << "<li><nobr>The '<b>Scaling Frequency (AC)</b>' for "
             << ((cpu_.value == ULONG_MAX) ? "Processor " : "cpu ")
             << ((cpu_.value == ULONG_MAX) ? cpuInfo().physicalId().value : cpu_.value)
             << " is <b>" << current_freq << "</b> instead of <b>"
             << tabSettings().frequencyScalingFreqAc(cpu)
             << "</b>.</nobr></li>";
          /* Frequency does not match. */
          retv = false;
        }
      }
    }
    else {
      /* DC powered. Compare the governor.  */
      if (tabSettings().frequencyScalingGovernorDc(cpu).compare(selected_governor) != 0) {
        ss << "<li><nobr>The '<b>Frequency Scaling Governor (DC)</b>' for "
           << ((cpu_.value == ULONG_MAX) ? "Processor " : "cpu ")
           << ((cpu_.value == ULONG_MAX) ? cpuInfo().physicalId().value : cpu_.value)
           << " is <b>" << selected_governor << "</b> instead of <b>"
           << tabSettings().frequencyScalingGovernorDc(cpu)
           << "</b>.</nobr></li>";
        /* Governor does not match */
        retv = false;
      }
      /* Is the 'userspace' governor selected? */
      if (tabSettings().frequencyScalingGovernorDc(cpu).compare("userspace") != 0) {
        /* No, compare the min/max frequencies. */
        if (tabSettings().frequencyScalingMinFreqDc(cpu) != current_min) {
          ss << "<li><nobr>The '<b>Minimum Scaling Frequency (DC)</b>' for "
             << ((cpu_.value == ULONG_MAX) ? "Processor " : "cpu ")
             << ((cpu_.value == ULONG_MAX) ? cpuInfo().physicalId().value : cpu_.value)
             << " is <b>" << current_min << "</b> instead of <b>"
             << tabSettings().frequencyScalingMinFreqDc(cpu)
             << "</b>.</nobr></li>";
          /* Minimum frequency does not match. */
          retv = false;
        }
        if (tabSettings().frequencyScalingMaxFreqDc(cpu) != current_max) {
          ss << "<li><nobr>The '<b>Maximum Scaling Frequency (DC)</b>' for "
             << ((cpu_.value == ULONG_MAX) ? "Processor " : "cpu ")
             << ((cpu_.value == ULONG_MAX) ? cpuInfo().physicalId().value : cpu_.value)
             << " is <b>" << current_max << "</b> instead of <b>"
             << tabSettings().frequencyScalingMaxFreqDc(cpu) << "</b>.</nobr></li>";
          /* Maximum frequency does not match. */
          retv = false;
        }
      }
      else {
        /* Yes, compare the frequency. */
        if (tabSettings().frequencyScalingFreqDc(cpu) != current_freq) {
          ss << "<li><nobr>The '<b>Scaling Frequency (DC)</b>' for "
             << ((cpu_.value == ULONG_MAX) ? "Processor " : "cpu ")
             << ((cpu_.value == ULONG_MAX) ? cpuInfo().physicalId().value : cpu_.value)
             << " is <b>" << current_freq << "</b> instead of <b>"
             << tabSettings().frequencyScalingFreqDc(cpu)
             << "</b>.</nobr></li>";
          /* Frequency does not match. */
          retv = false;
        }
      }
    }      
  }

  /* Return false if the newly read Values differ from the stored Settings. */
  return retv;
}

bool CpuFreqUtils::Processor::Thread::compare() {
  /* Store the current UI values to a temporary object */
  CpuFreqUtils::Settings data(tabSettings());
  store(data);

  /* Compare the relevant values in the temporary object against the
   * global TabMemberSettings instance and return false if they differ. */

  /* compare per-processor settings */
  if (data.gFrequencyScalingEnabled() !=
      tabSettings().gFrequencyScalingEnabled()) {
    DBGMSG("CpuFreqUtils::Settings::gFrequencyScalingEnabled() does not match (processor" << cpuInfo().physicalId().value << ")")
    return false;
  }
  if (data.gFrequencyScalingPerCpuEnabled() !=
      tabSettings().gFrequencyScalingPerCpuEnabled()) {
    DBGMSG("CpuFreqUtils::Settings::gFrequencyScalingPerCpuEnabled() does not match (processor" << cpuInfo().physicalId().value << ")")
    return false;
  }
  if (data.gFrequencyScalingBatteryEnabled() !=
      tabSettings().gFrequencyScalingBatteryEnabled()) {
    DBGMSG("CpuFreqUtils::Settings::gFrequencyScalingBatteryEnabled() does not match (processor" << cpuInfo().physicalId().value << ")")
    return false;
  }

  /* compare per-cpu settings */
  for (auto& cpu : cpuInfo().getLogicalCpu()) {
    if (data.frequencyScalingGovernorAc(cpu).compare(
        tabSettings().frequencyScalingGovernorAc(cpu)) != 0) {
      DBGMSG("CpuFreqUtils::Settings::frequencyScalingGovernorAc() does not match (cpu" << cpu.value << ")")
      return false;
    }

    if (data.frequencyScalingMinFreqAc(cpu) !=
        tabSettings().frequencyScalingMinFreqAc(cpu)) {
      DBGMSG("CpuFreqUtils::Settings::frequencyScalingMinFreqAc() does not match (cpu" << cpu.value << ")")
      return false;
    }

    if (data.frequencyScalingMaxFreqAc(cpu) !=
        tabSettings().frequencyScalingMaxFreqAc(cpu)) {
      DBGMSG("CpuFreqUtils::Settings::frequencyScalingMaxFreqAc() does not match (cpu" << cpu.value << ")")
      return false;
    }

    if (data.frequencyScalingFreqAc(cpu) !=
        tabSettings().frequencyScalingFreqAc(cpu)) {
      DBGMSG("CpuFreqUtils::Settings::frequencyScalingFreqAc() does not match (cpu" << cpu.value << ")")
      return false;
    }

    if (data.frequencyScalingGovernorDc(cpu).compare(
        tabSettings().frequencyScalingGovernorDc(cpu)) != 0) {
      DBGMSG("CpuFreqUtils::Settings::frequencyScalingGovernorDc() does not match (cpu" << cpu.value << ")")
      return false;
    }

    if (data.frequencyScalingMinFreqDc(cpu) !=
        tabSettings().frequencyScalingMinFreqDc(cpu)) {
      DBGMSG("CpuFreqUtils::Settings::frequencyScalingMinFreqDc() does not match (cpu" << cpu.value << ")")
      return false;
    }

    if (data.frequencyScalingMaxFreqDc(cpu) !=
        tabSettings().frequencyScalingMaxFreqDc(cpu)) {
      DBGMSG("CpuFreqUtils::Settings::frequencyScalingMaxFreqDc() does not match (cpu" << cpu.value << ")")
      return false;
    }

    if (data.frequencyScalingFreqDc(cpu) !=
        tabSettings().frequencyScalingFreqDc(cpu)) {
      DBGMSG("CpuFreqUtils::Settings::frequencyScalingFreqDc() does not match (cpu" << cpu.value << ")")
      return false;
    }

    if (data.frequencyScalingEnabled(cpu) !=
        tabSettings().frequencyScalingEnabled(cpu)) {
      DBGMSG("CpuFreqUtils::Settings::frequencyScalingEnabled() does not match (cpu" << cpu.value << ")")
      return false;
    }

    if (data.frequencyScalingBatteryEnabled(cpu) !=
        tabSettings().frequencyScalingBatteryEnabled(cpu)) {
      DBGMSG("CpuFreqUtils::Settings::frequencyScalingBatteryEnabled() does not match (cpu" << cpu.value << ")")
      return false;
    }
  }

  DBGMSG("CpuFreqUtils::compare(): Settings match")
  return true;
}
 
int CpuFreqUtils::Processor::Thread::getGovernors(QList<QString>& governors_list) {
  LogicalCpuNr cpu(
      cpu_.value == ULONG_MAX ? cpuInfo().firstLogicalCpu() : cpu_);
  return xxx::shell_command(
    {
      Settings::CPUFREQ_INFO,
      "--cpu", std::to_string(cpu.value),
      "--governors"
    },
    [&governors_list](auto, auto str) {
      str.pop_back();
      std::istringstream ss(std::move(str));
      while (ss >> str) governors_list.push_back(
          std::move(QString::fromStdString(std::move(str))));
      return 0;
    }
  );
}

int CpuFreqUtils::Processor::Thread::getHwFreqLimits(
    unsigned int& min, unsigned int& max) {
  LogicalCpuNr cpu(
      cpu_.value == ULONG_MAX ? cpuInfo().firstLogicalCpu() : cpu_);
  return xxx::shell_command(
    { Settings::CPUFREQ_INFO, "--cpu", std::to_string(cpu.value), "--hwlimits" },
    [&min, &max](auto, auto str) {
      str.pop_back();
      std::istringstream ss(std::move(str));
      ss >> str;
      min = static_cast<unsigned>(std::stoi(str));
      ss >> str;
      max = static_cast<unsigned>(std::stoi(str));
      return 0;
    }
  );
}

int CpuFreqUtils::Processor::Thread::getHwFreq(unsigned int& freq) {
  LogicalCpuNr cpu(
      cpu_.value == ULONG_MAX ? cpuInfo().firstLogicalCpu() : cpu_);
  return xxx::shell_command(
    { Settings::CPUFREQ_INFO, "--cpu", std::to_string(cpu.value), "--hwfreq" },
    [&freq](auto, auto str) {
      str.pop_back();
      freq = static_cast<unsigned>(std::stoi(str));
      return 0;
    }
  );
}

int CpuFreqUtils::Processor::Thread::getFreqLimitsAndPolicy(
    unsigned int& min, unsigned int& max, std::string& policy) {
  LogicalCpuNr cpu(
      cpu_.value == ULONG_MAX ? cpuInfo().firstLogicalCpu() : cpu_);
  return xxx::shell_command(
    { Settings::CPUFREQ_INFO, "--cpu", std::to_string(cpu.value), "--policy" },
    [&min, &max, &policy](auto, auto str) {
      str.pop_back();
      std::istringstream ss(std::move(str));
      ss >> str;
      min = static_cast<unsigned>(std::stoi(str));
      ss >> str;
      max = static_cast<unsigned>(std::stoi(str));
      ss >> policy;
      return 0;
    }
  );
}

/*
 * Slots
 */

void CpuFreqUtils::Processor::Thread::governorAcIndexChanged(int idx) {
  if (idx == userspace_idx_) {
    minFrequency_ac_->setEnabled(false);
    minLabel_ac_->setEnabled(false);
    minValue_ac_->setEnabled(false);
    maxFrequency_ac_->setEnabled(false);
    maxLabel_ac_->setEnabled(false);
    maxValue_ac_->setEnabled(false);
    frequency_ac_->setEnabled(true);
    frequencyLabel_ac_->setEnabled(true);
    frequencyValue_ac_->setEnabled(true);
  }
  else {
    minFrequency_ac_->setEnabled(true);
    minLabel_ac_->setEnabled(true);
    minValue_ac_->setEnabled(true);
    maxFrequency_ac_->setEnabled(true);
    maxLabel_ac_->setEnabled(true);
    maxValue_ac_->setEnabled(true);
    frequency_ac_->setEnabled(false);
    frequencyLabel_ac_->setEnabled(false);
    frequencyValue_ac_->setEnabled(false);
  }
  cpuFreqUtils().valueChangedSlot();
}

void CpuFreqUtils::Processor::Thread::minFreqAcValueChanged(int value) {
  minValue_ac_->setText(
      std::move(QString("%1 MHz").arg(freqFromSliderValue(static_cast<unsigned>(value)))));
  cpuFreqUtils().valueChangedSlot();
}

void CpuFreqUtils::Processor::Thread::maxFreqAcValueChanged(int value) {
  maxValue_ac_->setText(
      std::move(QString("%1 MHz").arg(freqFromSliderValue(static_cast<unsigned>(value)))));
  cpuFreqUtils().valueChangedSlot();
}

void CpuFreqUtils::Processor::Thread::usrFreqAcValueChanged(int value) {
  frequencyValue_ac_->setText(
      std::move(QString("%1 MHz").arg(freqFromSliderValue(static_cast<unsigned>(value)))));
  cpuFreqUtils().valueChangedSlot();
}

void CpuFreqUtils::Processor::Thread::governorDcIndexChanged(int idx) {
  if (idx == userspace_idx_) {
    minFrequency_dc_->setEnabled(false);
    minLabel_dc_->setEnabled(false);
    minValue_dc_->setEnabled(false);
    maxFrequency_dc_->setEnabled(false);
    maxLabel_dc_->setEnabled(false);
    maxValue_dc_->setEnabled(false);
    frequency_dc_->setEnabled(true);
    frequencyLabel_dc_->setEnabled(true);
    frequencyValue_dc_->setEnabled(true);
  }
  else {
    minFrequency_dc_->setEnabled(true);
    minLabel_dc_->setEnabled(true);
    minValue_dc_->setEnabled(true);
    maxFrequency_dc_->setEnabled(true);
    maxLabel_dc_->setEnabled(true);
    maxValue_dc_->setEnabled(true);
    frequency_dc_->setEnabled(false);
    frequencyLabel_dc_->setEnabled(false);
    frequencyValue_dc_->setEnabled(false);
  }
  cpuFreqUtils().valueChangedSlot();
}

void CpuFreqUtils::Processor::Thread::minFreqDcValueChanged(int value) {
  minValue_dc_->setText(
      std::move(QString("%1 MHz").arg(freqFromSliderValue(static_cast<unsigned>(value)))));
  cpuFreqUtils().valueChangedSlot();
}

void CpuFreqUtils::Processor::Thread::maxFreqDcValueChanged(int value) {
  maxValue_dc_->setText(
      std::move(QString("%1 MHz").arg(freqFromSliderValue(static_cast<unsigned>(value)))));
  cpuFreqUtils().valueChangedSlot();
}

void CpuFreqUtils::Processor::Thread::usrFreqDcValueChanged(int value) {
  frequencyValue_dc_->setText(
      std::move(QString("%1 MHz").arg(freqFromSliderValue(static_cast<unsigned>(value)))));
  cpuFreqUtils().valueChangedSlot();
}

void CpuFreqUtils::Processor::Thread::adjustStateChanged(int state) {
  if (state) {
    if (cb_all_) {
      cb_all_->setEnabled(true);
      if (cb_all_->isChecked()) {
        ac_box_->setEnabled(false);
        cb_battery_->setEnabled(false);
        dc_box_->setEnabled(false);
        for (size_t i = 1; i <= processor().threads().size(); ++i) {
          processor().tabs()->setTabEnabled(static_cast<int>(i), true);
        }
      }
      else {
        ac_box_->setEnabled(true);
        cb_battery_->setEnabled(true);
        if (cb_battery_->isChecked()) dc_box_->setEnabled(true);
        else dc_box_->setEnabled(false);
        for (size_t i = 1; i <= processor().threads().size(); ++i) {
          processor().tabs()->setTabEnabled(static_cast<int>(i), false);
        }
      }
    }
    else {
      ac_box_->setEnabled(true);
      cb_battery_->setEnabled(true);
      if (cb_battery_->isChecked()) dc_box_->setEnabled(true);
      else dc_box_->setEnabled(false);
    }
  }
  else {
    if (cb_all_) {
      cb_all_->setEnabled(false);
      ac_box_->setEnabled(false);
      cb_battery_->setEnabled(false);
      dc_box_->setEnabled(false);
      for (size_t i = 1; i <= processor().threads().size(); ++i) {
        processor().tabs()->setTabEnabled(static_cast<int>(i), false);
      }
    }
    else {
      ac_box_->setEnabled(false);
      cb_battery_->setEnabled(false);
      dc_box_->setEnabled(false);
    }
  }
  cpuFreqUtils().valueChangedSlot();
}

void CpuFreqUtils::Processor::Thread::batteryStateChanged(int state) {
  if (state) dc_box_->setEnabled(true);
  else dc_box_->setEnabled(false);
  cpuFreqUtils().valueChangedSlot();
}

void CpuFreqUtils::Processor::Thread::allStateChanged(int state) {
  if (state) {
    ac_box_->setEnabled(false);
    dc_box_->setEnabled(false);
    cb_battery_->setEnabled(false);
    for (size_t i = 1; i <= processor().threads().size(); ++i) {
      processor().tabs()->setTabEnabled(static_cast<int>(i), true);
    }
  }
  else {
    ac_box_->setEnabled(true);
    cb_battery_->setEnabled(true);
    if (cb_battery_->isChecked()) dc_box_->setEnabled(true);
    else dc_box_->setEnabled(false);
    for (size_t i = 1; i <= processor().threads().size(); ++i) {
      processor().tabs()->setTabEnabled(static_cast<int>(i), false);
    }
  }
  cpuFreqUtils().valueChangedSlot();
}

/*
 * class CpuFreqUtils::Values
 */

CpuFreqUtils::Values::Values(const SingleCpuInfo& ci) {

  size_t hw_threads = 0;

  /* Get the number of hardware threads for this processor */
  std::vector<std::string> output;
  auto rv = xxx::shell_command(
      {
        TabSettings::ScriptPath, "--htt-detect",
        "--processor", std::to_string(ci.physicalId().value)
      },
      [&output](auto reason, auto str){
        /* str is from stdout? */
        if (reason == 0) {
          /* We are only interested in the last word of each line of
           * output from the command.
           * So remove the trailing newline and then trim everything
           * but the last word from every line. */
          str.pop_back();
          str.erase(
            str.begin(),
            /* Look for the first space from the end,
             * then return an iterator in the opposite direction. */
            std::find_if(
              str.rbegin(), str.rend(), [](int ch){ return std::isspace(ch); }
            ).base()
          );
          output.emplace_back(std::move(str));
        }
        return 0;
      }
  );

  if (!rv && output.size() == 6) {
    /* command output line 5 == nr of hw threads */
    hw_threads = std::stoul(output[5]);
    DBGMSG("CpuFreqUtils::Values(): Number of hardware threads:" << hw_threads)
  }
  else {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, unexpected output from command (CpuFreqUtils::Values):</b></p>"
      "<nobr>core-adjust core-adjust --htt-detect</nobr>");
    exit(EXIT_FAILURE);
  }

  /* Fill a vector with logical cpu numbers for this processor */
  size_t base = ci.firstLogicalCpu().value;
  for (size_t i = 0; i < hw_threads; ++i) logical_.emplace_back(base + i);

  /* Create storage space */
  frequency_scaling_governor_ac_.resize(hw_threads);
  frequency_scaling_min_freq_ac_.resize(hw_threads);
  frequency_scaling_max_freq_ac_.resize(hw_threads);
  frequency_scaling_usr_freq_ac_.resize(hw_threads);
  frequency_scaling_governor_dc_.resize(hw_threads);
  frequency_scaling_min_freq_dc_.resize(hw_threads);
  frequency_scaling_max_freq_dc_.resize(hw_threads);
  frequency_scaling_usr_freq_dc_.resize(hw_threads);
}

const std::string&
CpuFreqUtils::Values::frequencyScalingGovernorAc(const LogicalCpuNr& l) const {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  return frequency_scaling_governor_ac_[n];
}

void CpuFreqUtils::Values::frequencyScalingGovernorAc(
    const LogicalCpuNr& l, std::string s) {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  frequency_scaling_governor_ac_[n] = std::move(s);
}

unsigned int CpuFreqUtils::Values::frequencyScalingMinFreqAc(
    const LogicalCpuNr& l) const {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  return frequency_scaling_min_freq_ac_[n];
}

void CpuFreqUtils::Values::frequencyScalingMinFreqAc(
    const LogicalCpuNr& l, unsigned int freq) {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  frequency_scaling_min_freq_ac_[n] = freq;
}

unsigned int CpuFreqUtils::Values::frequencyScalingMaxFreqAc(
    const LogicalCpuNr& l) const {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  return frequency_scaling_max_freq_ac_[n];
}

void CpuFreqUtils::Values::frequencyScalingMaxFreqAc(
    const LogicalCpuNr& l, unsigned int freq) {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  frequency_scaling_max_freq_ac_[n] = freq;
}

unsigned int CpuFreqUtils::Values::frequencyScalingFreqAc(
    const LogicalCpuNr& l) const {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  return frequency_scaling_usr_freq_ac_[n];
}

void CpuFreqUtils::Values::frequencyScalingFreqAc(
    const LogicalCpuNr& l, unsigned int freq) {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  frequency_scaling_usr_freq_ac_[n] = freq;
}

const std::string& CpuFreqUtils::Values::frequencyScalingGovernorDc(
    const LogicalCpuNr& l) const {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  return frequency_scaling_governor_dc_[n];
}

void CpuFreqUtils::Values::frequencyScalingGovernorDc(
    const LogicalCpuNr& l, std::string s) {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  frequency_scaling_governor_dc_[n] = std::move(s);
}

unsigned int CpuFreqUtils::Values::frequencyScalingMinFreqDc(
    const LogicalCpuNr& l) const {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  return frequency_scaling_min_freq_dc_[n];
}

void CpuFreqUtils::Values::frequencyScalingMinFreqDc(
    const LogicalCpuNr& l, unsigned int freq) {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  frequency_scaling_min_freq_dc_[n] = freq;
}

unsigned int CpuFreqUtils::Values::frequencyScalingMaxFreqDc(
    const LogicalCpuNr& l) const {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  return frequency_scaling_max_freq_dc_[n];
}

void CpuFreqUtils::Values::frequencyScalingMaxFreqDc(
    const LogicalCpuNr& l, unsigned int freq) {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  frequency_scaling_max_freq_dc_[n] = freq;
}

unsigned int CpuFreqUtils::Values::frequencyScalingFreqDc(
    const LogicalCpuNr& l) const {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  return frequency_scaling_usr_freq_dc_[n];
}

void CpuFreqUtils::Values::frequencyScalingFreqDc(
    const LogicalCpuNr& l, unsigned int freq) {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  frequency_scaling_usr_freq_dc_[n] = freq;
}

/*
 * class CpuFreqUtils::Settings
 */

CpuFreqUtils::Settings::Settings(const SingleCpuInfo& ci)
  : Values(ci), cpuInfo_(ci) {

  size_t hw_threads = 0;

  /* Get the number of hardware threads for this processor */
  std::vector<std::string> output;
  auto rv = xxx::shell_command(
      {
        TabSettings::ScriptPath,
        "--htt-detect", "--processor",
        std::to_string(cpuInfo().physicalId().value)
      },
      [&output](auto reason, auto str){
        /* str is from stdout? */
        if (reason == 0) {
          /* We are only interested in the last word of each line of
           * output from the command.
           * So remove the trailing newline and then trim everything
           * but the last word from every line. */
          str.pop_back();
          str.erase(
            str.begin(),
            /* Look for the first space from the end,
             * then return an iterator in the opposite direction. */
            std::find_if(
              str.rbegin(), str.rend(), [](int ch){ return std::isspace(ch); }
            ).base()
          );
          output.emplace_back(std::move(str));
        }
        return 0;
      }
  );

  if (!rv && output.size() == 6) {
    /* command output line 5 == nr of hw threads */
    hw_threads = std::stoul(output[5]);
    DBGMSG("CpuFreqUtils::Settings(): Number of hardware threads:" << hw_threads)
  }
  else {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, unexpected output from command (CpuFreqUtils::Settings):</b></p>"
      "<nobr>core-adjust core-adjust --htt-detect</nobr>");
    exit(EXIT_FAILURE);
  }

  /* Fill a vector with logical cpu numbers for this processor */
  size_t base = cpuInfo().firstLogicalCpu().value;
  for (size_t i = 0; i < hw_threads; ++i) logical_.emplace_back(base + i);

  /* Create storage space */
  enable_frequency_scaling_.resize(hw_threads);
  enable_battery_frequency_scaling_.resize(hw_threads);
}

bool
CpuFreqUtils::Settings::frequencyScalingEnabled(const LogicalCpuNr& l) const {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  return enable_frequency_scaling_[n];
}

void CpuFreqUtils::Settings::frequencyScalingEnabled(
    const LogicalCpuNr& l, bool state) {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  enable_frequency_scaling_[n] = state;
}

bool CpuFreqUtils::Settings::frequencyScalingBatteryEnabled(
    const LogicalCpuNr& l) const {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  return enable_battery_frequency_scaling_[n];
}

void CpuFreqUtils::Settings::frequencyScalingBatteryEnabled(
    const LogicalCpuNr& l, bool state) {
  size_t n = 0;
  for (; n < logical_.size(); ++n) {
    if (logical_[n] == l) break;
  }
  if (n == logical_.size()) throw std::range_error(RANGE_MSG);
  enable_battery_frequency_scaling_[n] = state;
}

bool CpuFreqUtils::Settings::gFrequencyScalingEnabled() const {
  return g_enable_frequency_scaling_;
}

void CpuFreqUtils::Settings::gFrequencyScalingEnabled(bool state) {
  g_enable_frequency_scaling_ = state;
}

bool CpuFreqUtils::Settings::gFrequencyScalingPerCpuEnabled() const {
  return g_enable_per_cpu_frequency_scaling_;
}

void CpuFreqUtils::Settings::gFrequencyScalingPerCpuEnabled(bool state) {
  g_enable_per_cpu_frequency_scaling_ = state;
}

bool CpuFreqUtils::Settings::gFrequencyScalingBatteryEnabled() const {
  return g_enable_battery_frequency_scaling_;
}

void CpuFreqUtils::Settings::gFrequencyScalingBatteryEnabled(bool state) {
  g_enable_battery_frequency_scaling_ = state;
}

void CpuFreqUtils::Settings::load(QSettings& qs,
    const TabMemberValues& tabValues) {

  /*
   * Load the Settings for this tab from the INI file
   * (or set defaults using the provided TabMemberValues).
   */

  /* Load the CpuFreqUtils per-processor section */
  std::stringstream ss;
  ss << INI_CPUFREQ_GRP << cpuInfo().physicalId().value;
  qs.beginGroup(ss.str().c_str());
  gFrequencyScalingEnabled(
      qs.value(INI_CPUFREQ_ENABLE, false).toBool());
  gFrequencyScalingPerCpuEnabled(
      qs.value(INI_CPUFREQ_PER_CPU_ENABLE, false).toBool());
  gFrequencyScalingBatteryEnabled(
      qs.value(INI_CPUFREQ_BATTERY_ENABLE, false).toBool());
  qs.endGroup();

#if 0
  DBGMSG("CpuFreqUtils::Settings::gFrequencyScalingEnabled()          <--" << gFrequencyScalingEnabled())
  DBGMSG("CpuFreqUtils::Settings::gFrequencyScalingPerCpuEnabled()    <--" << gFrequencyScalingPerCpuEnabled())
  DBGMSG("CpuFreqUtils::Settings::gFrequencyScalingBatteryEnabled()   <--" << gFrequencyScalingBatteryEnabled())
#endif

  /* Load the per logical cpu CpuFreqUtils sections for this processor. */
  for (auto& cpu : logical_) {
    ss.str(std::string());
    ss << INI_CPUFREQ_GRP_CPU << cpu.value;
    qs.beginGroup(ss.str().c_str());
    frequencyScalingGovernorAc(cpu, qs.value(
        INI_CPUFREQ_GOVERNOR_AC,
        QString::fromStdString(tabValues.frequencyScalingGovernorAc(cpu)))
            .toString().toStdString());
    frequencyScalingMinFreqAc(cpu, qs.value(
        INI_CPUFREQ_MIN_FREQ_AC, tabValues.frequencyScalingMinFreqAc(cpu))
            .toUInt());
    frequencyScalingMaxFreqAc(cpu, qs.value(
        INI_CPUFREQ_MAX_FREQ_AC, tabValues.frequencyScalingMaxFreqAc(cpu))
            .toUInt());
    frequencyScalingFreqAc(cpu, qs.value(
        INI_CPUFREQ_USR_FREQ_AC, tabValues.frequencyScalingFreqAc(cpu))
            .toUInt());
    frequencyScalingGovernorDc(cpu, qs.value(
        INI_CPUFREQ_GOVERNOR_DC,
        QString::fromStdString(tabValues.frequencyScalingGovernorDc(cpu)))
            .toString().toStdString());
    frequencyScalingMinFreqDc(cpu, qs.value(
        INI_CPUFREQ_MIN_FREQ_DC, tabValues.frequencyScalingMinFreqDc(cpu))
            .toUInt());
    frequencyScalingMaxFreqDc(cpu, qs.value(
        INI_CPUFREQ_MAX_FREQ_DC, tabValues.frequencyScalingMaxFreqDc(cpu))
            .toUInt());
    frequencyScalingFreqDc(cpu, qs.value(
        INI_CPUFREQ_USR_FREQ_DC, tabValues.frequencyScalingFreqDc(cpu))
            .toUInt());
    frequencyScalingEnabled(cpu, qs.value(
        INI_CPUFREQ_ENABLE, false).toBool());
    frequencyScalingBatteryEnabled(cpu, qs.value(
        INI_CPUFREQ_BATTERY_ENABLE, false).toBool());
    qs.endGroup();
#if 0
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingGovernorAc(" << cpu.value << ")     <--" << frequencyScalingGovernorAc(cpu).c_str())
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingMinFreqAc(" << cpu.value << ")      <--" << frequencyScalingMinFreqAc(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingMaxFreqAc(" << cpu.value << ")      <--" << frequencyScalingMaxFreqAc(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingFreqAc(" << cpu.value << ")         <--" << frequencyScalingFreqAc(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingGovernorDc(" << cpu.value << ")     <--" << frequencyScalingGovernorDc(cpu).c_str())
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingMinFreqDc(" << cpu.value << ")      <--" << frequencyScalingMinFreqDc(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingMaxFreqDc(" << cpu.value << ")      <--" << frequencyScalingMaxFreqDc(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingFreqDc(" << cpu.value << ")         <--" << frequencyScalingFreqDc(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingEnabled(" << cpu.value << ")        <--" << frequencyScalingEnabled(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingBatteryEnabled(" << cpu.value << ") <--" << frequencyScalingBatteryEnabled(cpu))
#endif
  }
}

void CpuFreqUtils::Settings::save(QSettings& qs) {
  /*
   * Save the Settings for this tab to the INI file.
   */

  /* Save the CpuFreqUtils common section */
  std::stringstream ss;
  ss << INI_CPUFREQ_GRP << cpuInfo().physicalId().value;
  qs.beginGroup(ss.str().c_str());
  qs.setValue(INI_CPUFREQ_ENABLE,
      QVariant::fromValue<bool>(gFrequencyScalingEnabled()));
  qs.setValue(INI_CPUFREQ_PER_CPU_ENABLE,
      QVariant::fromValue<bool>(gFrequencyScalingPerCpuEnabled()));
  qs.setValue(INI_CPUFREQ_BATTERY_ENABLE,
      QVariant::fromValue<bool>(gFrequencyScalingBatteryEnabled()));
  qs.endGroup();

#if 0
  DBGMSG("CpuFreqUtils::Settings::gFrequencyScalingEnabled()          -->" << gFrequencyScalingEnabled())
  DBGMSG("CpuFreqUtils::Settings::gFrequencyScalingPerCpuEnabled()    -->" << gFrequencyScalingPerCpuEnabled())
  DBGMSG("CpuFreqUtils::Settings::gFrequencyScalingBatteryEnabled()   -->" << gFrequencyScalingBatteryEnabled())
#endif

  /* Save the per logical cpu CpuFreqUtils sections for this processor. */
  for (auto& cpu : logical_) {
    std::stringstream ss;
    ss << INI_CPUFREQ_GRP_CPU << cpu.value;
    qs.beginGroup(ss.str().c_str());
    qs.setValue(INI_CPUFREQ_GOVERNOR_AC,
        QVariant::fromValue<QString>(QString::fromStdString(
            frequencyScalingGovernorAc(cpu))));
    qs.setValue(INI_CPUFREQ_MIN_FREQ_AC,
        QVariant::fromValue<unsigned int>(frequencyScalingMinFreqAc(cpu)));
    qs.setValue(INI_CPUFREQ_MAX_FREQ_AC,
        QVariant::fromValue<unsigned int>(frequencyScalingMaxFreqAc(cpu)));
    qs.setValue(INI_CPUFREQ_USR_FREQ_AC,
        QVariant::fromValue<unsigned int>(frequencyScalingFreqAc(cpu)));
    qs.setValue(INI_CPUFREQ_GOVERNOR_DC,
        QVariant::fromValue<QString>(QString::fromStdString(
            frequencyScalingGovernorDc(cpu))));
    qs.setValue(INI_CPUFREQ_MIN_FREQ_DC,
        QVariant::fromValue<unsigned int>(frequencyScalingMinFreqDc(cpu)));
    qs.setValue(INI_CPUFREQ_MAX_FREQ_DC,
        QVariant::fromValue<unsigned int>(frequencyScalingMaxFreqDc(cpu)));
    qs.setValue(INI_CPUFREQ_USR_FREQ_DC,
        QVariant::fromValue<unsigned int>(frequencyScalingFreqDc(cpu)));
    qs.setValue(INI_CPUFREQ_ENABLE,
        QVariant::fromValue<bool>(frequencyScalingEnabled(cpu)));
    qs.setValue(INI_CPUFREQ_BATTERY_ENABLE,
        QVariant::fromValue<bool>(frequencyScalingBatteryEnabled(cpu)));
    qs.endGroup();

#if 0
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingGovernorAc(" << cpu.value << ")     -->" << frequencyScalingGovernorAc(cpu).c_str())
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingMinFreqAc(" << cpu.value << ")      -->" << frequencyScalingMinFreqAc(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingMaxFreqAc(" << cpu.value << ")      -->" << frequencyScalingMaxFreqAc(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingFreqAc(" << cpu.value << ")         -->" << frequencyScalingFreqAc(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingGovernorDc(" << cpu.value << ")     -->" << frequencyScalingGovernorDc(cpu).c_str())
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingMinFreqDc(" << cpu.value << ")      -->" << frequencyScalingMinFreqDc(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingMaxFreqDc(" << cpu.value << ")      -->" << frequencyScalingMaxFreqDc(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingFreqDc(" << cpu.value << ")         -->" << frequencyScalingFreqDc(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingEnabled(" << cpu.value << ")        -->" << frequencyScalingEnabled(cpu))
    DBGMSG("CpuFreqUtils::Settings::frequencyScalingBatteryEnabled(" << cpu.value << ") -->" << frequencyScalingBatteryEnabled(cpu))
#endif
  }
}

