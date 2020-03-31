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
  * @file src/core-adjust-qt/SmtControl.hpp
  * @brief A TabMemberWidget for adjusting SMT/HTT settings
  *
  * @file src/core-adjust-qt/SmtControl.cpp
  * @brief A TabMemberWidget for adjusting SMT/HTT settings (implementation)
  *
  * @class SmtControl
  * @brief A TabMemberWidget for adjusting SMT/HTT settings
  *
  * @class HttControl
  * @brief Helper class for class SmtControl
  */
// Qt
#include <QApplication>
#include <QVBoxLayout>
#include <QMessageBox>
// App
#include "CpuFreqUtils.hpp"
#include "Dbg.hpp"
#include "Shell.hpp"
#include "SmtControl.hpp"
#include "Strings.hpp"
#include "TabMember.hpp"

/*
 * HttControl
 */

HttControl::HttControl(QWidget *parent) : QWidget(parent)
{
  dont_disable_smt_ = new QRadioButton(tr("Enable HyperThreading."));
  disable_smt_processor_ = new QRadioButton(tr("Disable HyperThreading."));
  buttons_ = new QButtonGroup(this);
  buttons_->addButton(dont_disable_smt_, 1);
  buttons_->addButton(disable_smt_processor_, 2);

  dont_disable_smt_->setToolTip("Enable Symmetric Multi Threading (SMT) for this processor.");
  disable_smt_processor_->setToolTip("Disable Symmetric Multi Threading (SMT) for this processor.");

  auto* grid = new QGridLayout(this);
  grid->setRowMinimumHeight(0, 10);
  grid->addWidget(dont_disable_smt_, 1, 1);
  grid->addWidget(disable_smt_processor_, 2, 1);
  grid->setRowStretch(3, 1);

  connect(buttons_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(button_toggled(int,bool)));
}

void HttControl::button_toggled(int, bool checked) {
  if (checked) {
    emit valueChanged();
  }
}

/*
 * SmtControl
 */

SmtControl::SmtControl(
  const CpuInfo& info,
  const CpuId& id,
  TabValues& values,
  TabSettings& data,
  QWidget *parent)
  : TabMemberTemplateAllCpus(info, id, values, data, parent),
    shell_(this, "Adjust HyperThreading settings",
        ShellCommand::EnableStd | ShellCommand::DisableAbortButton) {

  /* Create widgets */

  dont_adjust_smt_ = new QRadioButton(tr("Do not adjust HyperThreading settings."));
  enable_smt_global_ = new QRadioButton(tr("Enable HyperThreading."));
  disable_smt_global_ = new QRadioButton(tr("Disable HyperThreading."));
  per_processor_ = new QCheckBox(tr("Adjust HyperThreading for individual processors."));
  buttons_ = new QButtonGroup(this);
  buttons_->addButton(dont_adjust_smt_, 1);
  buttons_->addButton(enable_smt_global_, 2);
  buttons_->addButton(disable_smt_global_, 3);

  dont_adjust_smt_->setToolTip("Leaves the Symmetric Multi Threading (SMT) settings at their current state.");
  enable_smt_global_->setToolTip("Enable Symmetric Multi Threading (SMT) for all processors.");
  disable_smt_global_->setToolTip("Disable Symmetric Multi Threading (SMT) for all processors.");

  auto* radio_grid = new QGridLayout();
  radio_grid->setRowMinimumHeight(0, 10);
  radio_grid->addWidget(dont_adjust_smt_, 1, 1);
  radio_grid->addWidget(enable_smt_global_, 2, 1);
  radio_grid->addWidget(disable_smt_global_, 3, 1);
  radio_grid->setRowMinimumHeight(4, 10);
  radio_grid->addWidget(per_processor_, 5, 1);
  radio_grid->setRowMinimumHeight(6, 10);

  tabs_ = new QTabWidget();
  for (size_t n = 0; n < cpuInfo().size(); ++n) {
    auto* cpu = new HttControl(this);
    tabs_->addTab(cpu, std::move(QString("Processor %1").arg(n)));
    connect(cpu, SIGNAL(valueChanged()), this, SLOT(httValueChanged()));
  }

  auto* box1layout = new QVBoxLayout();
  box1layout->addLayout(radio_grid);
  box1layout->addWidget(tabs_, 1);

  auto* box1 = new QGroupBox(tr("Adjust HyperThreading settings."));
  box1->setLayout(box1layout);
  box1->setFlat(true);

  /* Add the inner widgets to the layout of the scroll widget */
  scroll_layout_->addWidget(box1);

  /* Signals */

  connect(buttons_, SIGNAL(buttonToggled(int,bool)),
    this, SLOT(button_toggled(int,bool)));

  connect(per_processor_, SIGNAL(stateChanged(int)),
      this, SLOT(perCpuChecked(int)));

  connect(&shell_, SIGNAL(finished()), &shell_, SLOT(accept()));
}

bool SmtControl::read(std::ostringstream& ss) {

  bool retv = true;

  /* Set defaults */
  tabValues().smtGlobalDisable(false);

  /*
   * Read the values for this tab from the processor/system
   */

  /* Callback function for xxx::shell_command */
  std::vector<std::string> output;
  auto callback = [&output](auto reason, auto str){
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
  };

  /* Detect if SMT is supported */
  auto rv = xxx::shell_command(
      { TabSettings::ScriptPath, "--htt-detect" }, callback);

  bool enabled = false;
  if (!rv && output.size() == 6) {
    /* command output line 1 == SMT state enabled/disabled */
    if (output[1].compare("enabled") == 0) enabled = true;
    tabValues().smtGlobalDisable(/*supported &&*/ !enabled);
  }
  else {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, unexpected output from command (SmtControl):</b></p>"
      "<nobr>core-adjust core-adjust --htt-detect</nobr>");
    setEnabled(false);
    return true;
  }

  /* Compare the newly read value against the desired value */
  if (tabSettings().smtGlobalDisableEnabled()) {
    if (tabValues().smtGlobalDisable() != tabSettings().smtGlobalDisable()) {
      ss << "<li><nobr>'<b>SMT_Disable</b>' is <b>" << tabValues().smtGlobalDisable()
         << "</b> instead of <b>" << tabSettings().smtGlobalDisable() << "</b>.</nobr></li>";
      retv = false;
    }

    /* Detect HyperThreading for every processor */
    for (auto& ci : cpuInfo()) {
      auto& tv = tabValues()[ci.physicalId()];
      auto& ts = tabSettings()[ci.physicalId()];

      tv.httDisable(false);

      /* run the core-adjust script */
      output.clear();
      auto rv = xxx::shell_command({
          TabSettings::ScriptPath,
          "--processor", std::to_string(ci.physicalId().value),
          "--htt-detect" }, callback);

      if (!rv && output.size() == 6) {
        /* command output line 2 == HyperThreading supported by this cpu yes/no */
        /* command output line 3 == HyperThreading state of this cpu enabled/disabled */
        bool supported = false;
        bool enabled = false;
        if (output[2].compare("yes") == 0) supported = true;
        if (output[3].compare("enabled") == 0) enabled = true;
        tv.httDisable(supported && !enabled);
      }
      else {
        QMessageBox::critical(nullptr, "Core Adjust",
          "<p><b>Error, unexpected output from command (SmtControl):</b></p>"
          "<nobr>core-adjust core-adjust --htt-detect</nobr>");
        setEnabled(false);
        return true;
      }

      /* Compare the newly read value against the desired value */
      if (ts.httDisableEnabled()) {
        if (ts.httDisable() != tv.httDisable()) {
          ss << "<li><nobr>'<b>HTT_Disable</b>' for CPU#"
             << ci.physicalId().value <<" is <b>" << tv.httDisable()
             << "</b> instead of <b>" << ts.httDisable() << "</b>.</nobr></li>";
          retv = false;
        }
      }
    }
  }

  /*
   * Return false if the newly read values differ from the desired values.
   */
  return retv;
}

void SmtControl::load() {
  /* Run 'core-adjust --htt-detect' for every processor to determine
   * what GUI options to enable/disable. */
  global_smt_support_ = false;
  cpu_htt_support_.clear();
  for (auto& ci : cpuInfo()) {
    std::vector<std::string> v;
    int rv = xxx::shell_command(
      { TabSettings::ScriptPath, "--htt-detect",
        "--processor", std::to_string(ci.physicalId().value) },
      [&v](int reason, std::string s){
        if (reason == 0) {
          /* We are only interested in the last word of each line of
           * output from the command.
           * So remove the trailing newline and then trim everything
           * but the last word from every line. */
          s.pop_back();
          s.erase(
            /* Start erasing characters from the beginning. */
            s.begin(),
            /* Look for the first space from the end,
             * then return an iterator in the oposite direction. */
            std::find_if(
              s.rbegin(), s.rend(), [](int c){ return std::isspace(c); }
            ).base()
          );
          v.push_back(std::move(s));
        }
        return 0;
      }
    );
    /* Does the output look valid? */
    if (rv != 0 || v.size() != 6) {
      std::stringstream ss;
      ss << "<p><b>Error, unexpected output from command:</b></p>"
         << "<nobr>core-adjust --htt-detect --processor "
         << ci.physicalId().value << "</nobr>";
      QMessageBox::critical(nullptr, "Core Adjust",
          QString::fromStdString(ss.str()));
      global_smt_support_ = false;
      break;
    }
    /* command output line 0 == global SMT support yes/no */
    if (v[0].compare("yes") == 0) global_smt_support_ = true;
    /* command output line 2 == HyperThreading supported by this cpu yes/no */
    if (v[2].compare("yes") == 0) cpu_htt_support_.push_back(true);
    else cpu_htt_support_.push_back(false);
  }

  /* Enable/disable widgets */

  if (global_smt_support_ == false) {
    setEnabled(false);
  }
  else {
    setEnabled(true);
    for (size_t n = 0; n < cpuInfo().size(); ++n) {
      tabs_->widget(static_cast<int>(n))->setEnabled(cpu_htt_support_[n]);
    }
  }
}

void SmtControl::store() {
  /* Store current UI values to the global Settings instance */
  store(tabSettings());
}

void SmtControl::store(TabSettings& ts) {
  /* Store current UI values to the specified Settings instance */
  if (global_smt_support_) {
    switch (buttons_->checkedId()) {
      case 2: /* Enable Global SMT */
        ts.smtGlobalDisableEnabled(true);
        ts.smtGlobalDisable(false);
        if (per_processor_->isChecked()) {
          ts.httPerCpuEnabled(true);
          for (PhysCpuNr p(0); p.value < cpuInfo().size(); ++p) {
            ts[p].httDisableEnabled(true);
            ts[p].httDisable(
                static_cast<HttControl*>(tabs_->widget(
                    static_cast<int>(p.value)))->disable_smt_processor_->
                        isChecked());
          }
        }
        else {
          ts.httPerCpuEnabled(false);
          for (PhysCpuNr p(0); p.value < cpuInfo().size(); ++p) {
            ts[p].httDisableEnabled(false);
            ts[p].httDisable(false);
          }
        }
        break;
      case 3: /* Disable Global SMT */
        ts.smtGlobalDisableEnabled(true);
        ts.smtGlobalDisable(true);
        if (per_processor_->isChecked()) {
          ts.httPerCpuEnabled(true);
          for (PhysCpuNr p(0); p.value < cpuInfo().size(); ++p) {
            ts[p].httDisableEnabled(true);
            ts[p].httDisable(static_cast<HttControl*>(
                tabs_->widget(static_cast<int>(p.value)))
                    ->disable_smt_processor_->isChecked());
          }
        }
        else {
          ts.httPerCpuEnabled(false);
          for (PhysCpuNr p(0); p.value < cpuInfo().size(); ++p) {
            ts[p].httDisableEnabled(false);
            ts[p].httDisable(true);
          }
        }
        break;
      default: /*do not adjust*/
        ts.smtGlobalDisableEnabled(false);
        ts.httPerCpuEnabled(false);
        for (PhysCpuNr p(0); p.value < cpuInfo().size(); ++p) {
          ts[p].httDisableEnabled(false);
          ts[p].httDisable(false);
        }
        break;
    }
  }
}

void SmtControl::refresh() {
  /* Exit if we are disabled */
  if (!isEnabled()) return;

  buttons_->blockSignals(true);
  per_processor_->blockSignals(true);
  for (size_t n = 0; n < cpuInfo().size(); ++n) {
    static_cast<HttControl*>(
        tabs_->widget(static_cast<int>(n)))->buttons_->blockSignals(true);
  }

  /* Global SMP supported? */
  if (global_smt_support_ == false) {
    /* No, select 'dont adjust' for global SMP and
     * 'enable htt' for each cpu  */
    dont_adjust_smt_->setChecked(true);
    for (size_t n = 0; n < cpuInfo().size(); ++n) {
      static_cast<HttControl*>(tabs_->widget(static_cast<int>(n)))->
          dont_disable_smt_->setChecked(true);
    }
  }
  else {
    /* Global SMP is supported, normal selection */
    if (tabSettings().smtGlobalDisableEnabled() == false) {
      dont_adjust_smt_->setChecked(true);
      per_processor_->setEnabled(false);
    }
    else {
      if (tabValues().smtGlobalDisable() == false) {
        enable_smt_global_->setChecked(true);
        per_processor_->setEnabled(true);
      }
      else {
        disable_smt_global_->setChecked(true);
        per_processor_->setEnabled(false);
      }
    }

    if (tabSettings().httPerCpuEnabled() == true) {
      per_processor_->setChecked(true);
      for (size_t n = 0; n < cpuInfo().size(); ++n) {
        tabs_->widget(static_cast<int>(n))->setEnabled(true);
      }
    }
    else {
      per_processor_->setChecked(false);
      for (size_t n = 0; n < cpuInfo().size(); ++n) {
        tabs_->widget(static_cast<int>(n))->setEnabled(false);
      }
    }

    for (PhysCpuNr p(0); p.value < cpuInfo().size(); ++p) {
      auto* widget = static_cast<HttControl*>(
          tabs_->widget(static_cast<int>(p.value)));
      if (tabSettings()[p].httDisableEnabled() == true) {
        if (tabValues()[p].httDisable() == true) {
          widget->disable_smt_processor_->setChecked(true);
        }
        else {
          widget->dont_disable_smt_->setChecked(true);
        }
      }
      else {
        if (tabValues().smtGlobalDisable() == false) {
          widget->dont_disable_smt_->setChecked(true);
        }
        else {
          widget->disable_smt_processor_->setChecked(true);
        }
      }
    }
  }

  buttons_->blockSignals(false);
  per_processor_->blockSignals(false);
  for (size_t n = 0; n < cpuInfo().size(); ++n) {
    static_cast<HttControl*>(tabs_->widget(static_cast<int>(n)))->
        buttons_->blockSignals(false);
  }
}

bool SmtControl::apply() {
  /* Exit if we are disabled */
  if (!isEnabled()) return true;

  /* Apply the current tabSettings() */

  int rv = 0;
  if (doApply) {
    if (tabSettings().smtGlobalDisableEnabled()) {
      /* Clear the 'screen' of the shell dialog */
      shell_.cls();

      if (tabSettings().smtGlobalDisable()) {
        /* Disable SMT globally */
        DBGMSG("SmtControl::apply(): Disabling SMT")
        rv = applySMT(true);
      }
      else {
        /* Enable SMT globally. */
        DBGMSG("SmtControl::apply(): Enabling SMT")
        rv = applySMT(false);
        /* Adjust HTT for individual processors ? */
        if (rv == 0) {
          if (tabSettings().httPerCpuEnabled()) {
            for (PhysCpuNr p(0); p.value < cpuInfo().size(); ++p) {
              if (tabSettings()[p].httDisableEnabled()) {
                DBGMSG("SmtControl::apply(): Apply HTT settings for processor" << p.value << "(" << tabSettings()[p].httDisable() << ")")
                rv = applyHTT(p, tabSettings()[p].httDisable());
              }
            }
          }
        }
      }

      /* Apply frequency scaling settings if the number of logical cpus has changed */
      std::vector<size_t> vs;
      for (auto& ci : cpuInfo()) vs.push_back(ci.getLogicalCpu().size());
      const_cast<CpuInfo&>(cpuInfo()).refresh();
      auto it = cpuInfo().begin();
      auto os = vs.begin();
      bool fs = false;
      for (; it != cpuInfo().end() && os != vs.end(); ++it, ++os) {
        if (it->getLogicalCpu().size() != *os) fs = true;
      }
      if (fs) {
        auto&& cmd = CpuFreqUtils::GenerateShellCmd(cpuInfo(), tabSettings());
        if (cmd.size()) shell_.run(std::move(cmd), false, false);
      }
    }

    /* This enables the close button of the shell dialog */
    DBGMSG("SmtControl::apply(): Finished")
    shell_.allowClose();

    if (rv == 0) doApply = false;
  }

  return rv == 0;
}

/* true=disable smt, false=enable smt */
int SmtControl::applySMT(bool state) {
  return shell_.run(
      { TabSettings::ScriptPath, "-v", "--smt", (state) ? "off" : "on" },
      false, false);
}

/* true=disable htt, false=enable htt */
int SmtControl::applyHTT(PhysCpuNr n, bool state) {
  return shell_.run(
      {
        TabSettings::ScriptPath, "-v", "-p", std::to_string(n.value),
        "--htt", (state) ? "off" : "on"
      },
      false, false);
}

bool SmtControl::compare() {
  /* Store the current UI values to a temporary object.
   * Create the temporary as a copy so that it has the correct vector size. */
  TabSettings tmp(tabSettings());
  store(tmp);

  /* Compare the relevant values in the temporary object against the
   * global TabSettings instance and return false if they differ. */
  if (tmp.smtGlobalDisableEnabled() != tabSettings().smtGlobalDisableEnabled()) {
    DBGMSG("SmtControl::Settings::smtGlobalDisableEnabled() does not match")
    doApply = true;
    return false;
  }

  if (tabSettings().smtGlobalDisableEnabled()) {
    if (tmp.smtGlobalDisable() != tabSettings().smtGlobalDisable()) {
      DBGMSG("SmtControl::Settings::smtGlobalDisable() does not match")
      doApply = true;
      return false;
    }

    if (tmp.httPerCpuEnabled() != tabSettings().httPerCpuEnabled()) {
      DBGMSG("SmtControl::Settings::httPerCpuEnabled() does not match")
      doApply = true;
      return false;
    }

    for (PhysCpuNr p(0); p.value < tmp.size(); ++p) {
      if (tmp[p].httDisableEnabled() != tabSettings()[p].httDisableEnabled()) {
        DBGMSG("SmtControl::Settings::httDisableEnabled() does not match (processor" << p.value << ")")
        doApply = true;
        return false;
      }
      if (tabSettings()[p].httDisableEnabled()) {
        if (tmp[p].httDisable() != tabSettings()[p].httDisable()) {
          DBGMSG("SmtControl::Settings::httDisable() does not match (processor" << p.value << ")")
          doApply = true;
          return false;
        }
      }
    }
  }
  doApply = false;
  DBGMSG("SmtControl::compare(): Settings match!")
  return true;
}

/* SLOTS */

void SmtControl::button_toggled(int id, bool checked) {
  if (checked) {
    per_processor_->blockSignals(true);
    switch (id) {
      case 2:
        /* Enable SMT */
        per_processor_->setEnabled(true);
        if (per_processor_->isChecked()) {
          for (size_t n = 0; n < cpuInfo().size(); ++n) {
            static_cast<HttControl*>(tabs_->widget(static_cast<int>(n)))->
                setEnabled(true);
          }
        }
        break;
      case 3:
        /* Disable SMT */
        per_processor_->setEnabled(false);
        for (size_t n = 0; n < cpuInfo().size(); ++n) {
          static_cast<HttControl*>(tabs_->widget(static_cast<int>(n)))->
              setEnabled(false);
        }
        break;
      default:
        /* Do not adjust SMP/HTT */
        per_processor_->setEnabled(false);
        for (size_t n = 0; n < cpuInfo().size(); ++n) {
          static_cast<HttControl*>(tabs_->widget(static_cast<int>(n)))->
              setEnabled(false);
        }
        break;
    }
    per_processor_->blockSignals(false);
    emit valueChanged(this);
  }
}

void SmtControl::perCpuChecked(int state) {
  buttons_->blockSignals(true);
  per_processor_->blockSignals(true);
  for (size_t n = 0; n < cpuInfo().size(); ++n) {
    auto* widget =
        static_cast<HttControl*>(tabs_->widget(static_cast<int>(n)));
    widget->buttons_->blockSignals(true);
    widget->setEnabled(state);
    widget->buttons_->blockSignals(false);
  }
  buttons_->blockSignals(false);
  per_processor_->blockSignals(false);
  emit valueChanged(this);
}

void SmtControl::httValueChanged() {
  emit valueChanged(this);
}

/*
 * SmtControl::Settings
 */

void SmtControl::Settings::load(
    QSettings& qs, const TabMemberValues&) {

  /* Load settings from the Common section of the INI file
   * when the settings for the first processor are loaded. */
  if (cpuInfo().physicalId().value == 0) {
    CommonSettings& c = commonSettings();
    qs.beginGroup(TabSettings::INI_GRP_COMMON);
    c.smtGlobalDisable(qs.value(INI_SMT_DISABLE, false).toBool());
    c.smtGlobalDisableEnabled(qs.value(INI_SMT_DISABLE_ENABLE, false).toBool());
    c.httPerCpuEnabled(qs.value(INI_HTT_PER_CPU_ENABLE, false).toBool());
    qs.endGroup();
//    DBGMSG("SmtControl::Common::smtGlobalDisable()                      <--" << c.smtGlobalDisable())
//    DBGMSG("SmtControl::Common::smtGlobalDisableEnabled()               <--" << c.smtGlobalDisableEnabled())
//    DBGMSG("SmtControl::Common::httPerCpuEnabled()                      <--" << c.httPerCpuEnabled())
  }

  /* select the INI section for this processor */
  std::stringstream ss;
  ss << TabSettings::INI_GRP_PROCESSOR << cpuInfo().physicalId().value;
  qs.beginGroup(ss.str().c_str());

  /* Load the settings or set a default value: */

  httDisable(qs.value(INI_HTT_DISABLE, false).toBool());
  httDisableEnabled(qs.value(INI_HTT_DISABLE_ENABLE, false).toBool());
//  DBGMSG("SmtControl::Settings::httDisable()                          <--" << httDisable())
//  DBGMSG("SmtControl::Settings::httDisableEnabled()                   <--" << httDisableEnabled())

  qs.endGroup();
}

void SmtControl::Settings::save(QSettings& qs) {

  /* Save settings from the Common section of the INI file
   * when the settings for the first processor are saved. */
  if (tabMemberSettings().processorNumber().value == 0) {
    CommonSettings& c = commonSettings();
    qs.beginGroup(TabSettings::INI_GRP_COMMON);
    qs.setValue(INI_SMT_DISABLE, QVariant::fromValue<bool>(c.smtGlobalDisable()));
    qs.setValue(INI_SMT_DISABLE_ENABLE, QVariant::fromValue<bool>(c.smtGlobalDisableEnabled()));
    qs.setValue(INI_HTT_PER_CPU_ENABLE, QVariant::fromValue<bool>(c.httPerCpuEnabled()));
    qs.endGroup();
//    DBGMSG("SmtControl::Common::smtGlobalDisable()                      -->" << c.smtGlobalDisable())
//    DBGMSG("SmtControl::Common::smtGlobalDisableEnabled()               -->" << c.smtGlobalDisableEnabled())
//    DBGMSG("SmtControl::Common::httPerCpuEnabled()                      -->" << c.httPerCpuEnabled())
  }

  /* select the INI section for this processor */
  std::stringstream ss;
  ss << TabSettings::INI_GRP_PROCESSOR
     << tabMemberSettings().processorNumber().value;
  qs.beginGroup(ss.str().c_str());

  /* Save the settings: */

  qs.setValue(INI_HTT_DISABLE, QVariant::fromValue<bool>(httDisable()));
  qs.setValue(INI_HTT_DISABLE_ENABLE, QVariant::fromValue<bool>(httDisableEnabled()));
//  DBGMSG("SmtControl::Settings::httDisable()                          -->" << httDisable())
//  DBGMSG("SmtControl::Settings::httDisableEnabled()                   -->" << httDisableEnabled())

  qs.endGroup();
}

