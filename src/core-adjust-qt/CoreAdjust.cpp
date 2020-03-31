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

// STL
#include <sstream>
#include <vector>
// Qt
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
// App
#include "CoreAdjust.hpp"
#include "CpuId.hpp"
#include "CpuInfo.hpp"
#include "CpuFreqUtils.hpp"
#include "Dbg.hpp"
#include "Grub.hpp"
#include "TabMember.hpp"
#include "ThermalStatus.hpp"
#include "MsrReadout.hpp"

/* All TabMemberWidget that are added to each processor tab. */
const std::vector<CoreAdjust::TabMemberDescription>
CoreAdjust::TabMemberFactory {
  { "Speed Control",     &TabMemberDescription::Construct<SpeedControl> },
  { "Voltage Regulator", &TabMemberDescription::Construct<VoltageOffsets> },
  { "Thermal Control",   &TabMemberDescription::Construct<ThermalControl> },
  { "Misc Control",      &TabMemberDescription::Construct<MiscEnable> },
  { "Thermal Status",    &TabMemberDescription::Construct<ThermalStatus> },
  { "MSR readout",       &TabMemberDescription::Construct<MsrReadout> },
};

/* All TabMemberWidget that do not depend on a single processor. */
const std::vector<CoreAdjust::TabMemberAllCpusDescription>
CoreAdjust::TabMemberAllCpusFactory {
  { "Frequency Scaling", &TabMemberAllCpusDescription::Construct<CpuFreqUtils> },
  { "HyperThreading",    &TabMemberAllCpusDescription::Construct<SmtControl> },
  { "Bootloader",        &TabMemberAllCpusDescription::Construct<Grub> },
};

/*
 * CoreAdjust ctor:
 *   - Create and add widgets.
 *
 *   - Call all TabMemberWidget::read() instances to update the current TabValues
 *     and compare them against the TabSettings (loaded from the INI file).
 *
 *   - Call all TabMemberWidget::load() instances to load the settings
 *     from TabSettings into the UI of each TabMemberWidget.
 *
 *   - Enable the 'apply' button if any TabMemberWidget::read() returned false.
 *     (This is tested by CoreAdjust::eventFilter() upon the first QEvent::Show
 *     event. If enabled an apply dialog is then shown by CoreAdjust::applyAfterShowEvent().)
 *
 *   - Connect all relevant signals (from ui-items and TabMemberWidget) to slots.
 *
 *   - Start the timer that calls each TabMemberWidget::timed() method every 500ms.
 */
CoreAdjust::CoreAdjust(
  CpuId& id,
  CpuInfo& info,
  TabValues& values,
  TabSettings& settings,
  QWidget* parent)
  : QWidget(parent),
    cpuId_(id),
    cpuInfo_(info),
    tabValues_(values),
    tabSettings_(settings),
    shell_(this, "",
        ShellCommand::EnableStd | ShellCommand::DisableAbortButton) {

  auto* buttons_hbox = new QHBoxLayout();
  btnApply_ = new QPushButton(tr("&Apply"));
  btnExit_ = new QPushButton(tr("E&xit"));
  buttons_hbox->addStretch(1);
  buttons_hbox->addWidget(btnApply_);
  buttons_hbox->addWidget(btnExit_);
  buttons_hbox->addStretch(1);

  auto* left_vbox = new QVBoxLayout();
  tabwidget_ = new QTabWidget();
  chkboxApplyOnEvent_ = new QCheckBox(tr(
      "Apply settings on &boot, resume and ACPI power events."));
  chkboxSaveOnExit_ = new QCheckBox(tr(
      "&Save settings on exit."));
  left_vbox->addWidget(tabwidget_);
  left_vbox->addWidget(chkboxApplyOnEvent_);
  left_vbox->addWidget(chkboxSaveOnExit_);
  left_vbox->addLayout(buttons_hbox);

  monitorLayout_ = new QVBoxLayout();
  monitor_ = new Monitor();
  monitorLayout_->addWidget(monitor_);

  auto* layout = new QHBoxLayout(this);
  layout->addLayout(left_vbox, 1);       /* Stretch the left_vbox */
  layout->addLayout(monitorLayout_, 0);  /* Do not strech the monitorLayout_ */

  /* Loop over all detected processors adding a tab for each one. */
  DBGMSG("CoreAdjust(): Adding TabMemberWidget tabs for"
      << cpuInfo().size() << "processor(s)")
  for (PhysCpuNr p(0); p.value < cpuInfo().size(); ++p) addProcessorTab(p);

  /* Add the tabs that are not for a single processor */
  for (auto& product : TabMemberAllCpusFactory) {
    auto* widget = product.construct(
        cpuInfo(), cpuId(), tabValues(), tabSettings(), nullptr);
    tabwidget_->addTab(widget, product.name);
    DBGMSG("CoreAdjust(): Adding global TabMemberWidget instance of class:"
        << widget->metaObject()->className())
    vTabMemberWidget_.emplace_back(nullptr,
        std::vector<TabMemberWidget*>(1, widget), 0);
  }

  /* Add the MonitorTab */
  monitor_tab_ = new MonitorTab();
  tabwidget_->addTab(monitor_tab_, "Monitor");
  vTabMemberWidget_.emplace_back(nullptr,
      std::vector<TabMemberWidget*>(1, monitor_tab_), 0);
  monitor_tab_->setMonitor(monitor_);

  /* Enable the apply button if the settings differ from the applied values. */
  ctor_apply_ = false;
  std::ostringstream unused;
  unused << std::boolalpha << "<ul>";
  for (auto& tpl : vTabMemberWidget_) {
    for (auto* widget : std::get<1>(tpl)) {
      DBGMSG("CoreAdjust(): Calling"
          << widget->metaObject()->className() << "\b::read()")
      if (widget->read(unused) == false) ctor_apply_ = true;
    }
  }
  unused << "</ul>";

  /* Apply values from the configuration file to the widgets */
  load();

  /* The apply button is disabled until changes to the settings are made */
  btnApply_->setEnabled(ctor_apply_);

  /* Stop the exit button from stealing focus after clicking apply. */
  btnExit_->setFocusPolicy(Qt::NoFocus);

  /* Connect relevant signals to slots: */

  connect(&shell_, SIGNAL(finished()), &shell_, SLOT(accept()));

  connect(chkboxApplyOnEvent_, SIGNAL(stateChanged(int)),
      this, SLOT(cbApplyOnSysEvent(int)));

  connect(btnApply_, SIGNAL(released(void)),
      this, SLOT(apply(void)));

  connect(btnExit_, SIGNAL(released(void)),
      this, SLOT(btnExit(void)));

  connect(tabwidget_, SIGNAL(currentChanged(int)),
      this, SLOT(tabSwitch(int)));

  for (auto& tpl : vTabMemberWidget_) {
    for (auto* widget : std::get<1>(tpl)) {
      connect(widget, SIGNAL(valueChanged(TabMemberWidget*)),
          this, SLOT(valueChanged(TabMemberWidget*)));
      DBGMSG("CoreAdjust(): Connecting to signal"
          << widget->metaObject()->className() << "\b::valueChanged()")
    }
  }

  /* setup a timer slot that is called every 500ms */
  timer_ = new QTimer(this);
  connect(timer_, SIGNAL(timeout()), this, SLOT(timerCallback()));
  timer_->start(500);

  DBGMSG("CoreAdjust(): Ready, waiting for events...")
}

/*
 * CoreAdjust dtor:
 *   - Prevent the MonitorTab from accessing the Monitor instance.
 *   - Call all TabMemberWidget::store() instances to save the current
 *     values into TabSettings.
 */
CoreAdjust::~CoreAdjust() {
  DBGMSG("~CoreAdjust(): Exit application...")
  monitor_tab_->setMonitor(nullptr);
  store();
  DBGMSG("~CoreAdjust(): Exit application!")
}


/*
 * CoreAdjust::addProcessorTab:
 *  - Create a new tab (widget) and add all TabMemberWidget(s) for that processor.
 *  - Add the new tab to tabwidget_.
 *  - Add all new TabMemberWidget instances to vTabMemberWidget_.
 */
void CoreAdjust::addProcessorTab(const PhysCpuNr& processor_number)
{
  auto* tab = new QWidget();
  auto* tab_layout = new QVBoxLayout();
  auto* tab_widget = new QTabWidget();

  QLabel *model_name = new QLabel(QString::fromStdString(
      cpuInfo().at(processor_number).modelName()));
  auto *b1 = new QVBoxLayout();
  auto *b2 = new QHBoxLayout();
  b1->addSpacing(10);
  b1->addLayout(b2, 0);
  b1->addSpacing(10);
  b1->setMargin(0);
  b2->addStretch(1);
  b2->addWidget(model_name, 0);
  b2->addStretch(1);
  b2->setMargin(0);

  std::vector<TabMemberWidget*> vWidget;
  for (auto& product : TabMemberFactory) {
    vWidget.push_back(
        product.construct(
            cpuInfo().at(processor_number),
            cpuId().at(processor_number),
            tabValues().at(processor_number),
            tabSettings().at(processor_number),
            nullptr));
    DBGMSG("CoreAdjust::addProcessorTab(): Adding TabMemberWidget instance of class:"
        << vWidget.back()->metaObject()->className())
    tab_widget->addTab(vWidget.back(), product.name);
  }

  tab_layout->addLayout(b1);
  tab_layout->addWidget(tab_widget);
  tab->setLayout(tab_layout);

  vTabMemberWidget_.emplace_back(tab_widget, std::move(vWidget), 0);

  std::stringstream ss;
  ss << "Processor " << processor_number.value;
  tabwidget_->addTab(tab, QString::fromStdString(ss.str()));

  connect(tab_widget, SIGNAL(currentChanged(int)),
      this, SLOT(processorTabSwitch(int)));
}

/*
 * Load the current TabSettings into this and all TabMemberWidget instances.
 */
void CoreAdjust::load() {
  for (auto& tpl : vTabMemberWidget_) {
    for (auto* widget : std::get<1>(tpl)) {
      DBGMSG("CoreAdjust::load(): Calling"
          << widget->metaObject()->className() << "\b::load()")
      widget->load();
      DBGMSG("CoreAdjust::load(): Calling"
          << widget->metaObject()->className() << "\b::refresh()")
      widget->refresh();
    }
  }
  chkboxSaveOnExit_->setChecked(tabSettings().saveOnExit());
  chkboxApplyOnEvent_->setChecked(tabSettings().applyOnBootAndResume());
}

/*
 * Store the current values of this and all TabMemberWidget instances
 * to the TabSettings.
 */
void CoreAdjust::store() {
  for (auto& tpl : vTabMemberWidget_) {
    for (auto* widget : std::get<1>(tpl)) {
      DBGMSG("CoreAdjust::store(): Calling"
          << widget->metaObject()->className() << "\b::store()")
      widget->store();
    }
  }
  tabSettings().applyOnBootAndResume(chkboxApplyOnEvent_->isChecked());
  tabSettings().applyOnAcpiPowerEvent(chkboxApplyOnEvent_->isChecked());
  if (chkboxSaveOnExit_->isChecked()) {
    tabSettings().saveOnExit(true);
  }
  else {
    tabSettings().saveOnExit(false);
  }
}

/*
 * This method is set as QWidget::eventFilter override by our parent.
 * Upon every QEvent::Close event CoreAdjust::handleCloseEvent() is called.
 * Upon the first QEvent::Show event CoreAdjust::applyAfterShowEvent() is called.
 */
bool CoreAdjust::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::Close) {
    /* Lets ignore the close event and hanle it ourselves */
    event->ignore();
    handleCloseEvent();
    return true;
  }
  else if (event->type() == QEvent::Show) {
    /* Call applyAfterShowEvent() after the first show event
     * if the ctor has enabled the 'apply' button */
    if (ctor_apply_) {
      ctor_apply_ = false;
      /* The timeout MUST be 0 to fire the single shot after all pending
       * events have been handled, including this show event. */
      QTimer::singleShot(0, this, SLOT(applyAfterShowEvent()));
    }
  }
  return QWidget::eventFilter(obj, event);
}

/*
 * This method is called because the settings read from the INI file do not match
 * the current values. It opens a dialog to ask to apply the settings from the
 * INI file or to ignore them.
 */
void CoreAdjust::applyAfterShowEvent() {
  int rv = 1;
  while (rv) {
    rv = 0;
    std::ostringstream ss;
    ss << "The current system settings do not match the settings in the configuration file:"
       << std::boolalpha << "<ul>";
    for (auto& tpl : vTabMemberWidget_) {
      for (auto* widget : std::get<1>(tpl)) {
        if (widget->read(ss) == false) rv = 1;
        else {
          widget->refresh();
        }
      }
    }
    ss << "</ul>Do you want to apply or ignore the settings from the configuration file?";
    if (rv) {
      rv = QMessageBox::question(this, "Adjust settings?", ss.str().c_str(),
          QMessageBox::Apply | QMessageBox::Discard, QMessageBox::Discard);
      if (rv == QMessageBox::Apply) {
        /* Apply */
        monitor_tab_->setMonitor(nullptr);
        shell_.run({ TabSettings::ScriptPath, "--verbose", "--force", "--boot" });
        cpuId().refresh();
        cpuInfo().refresh();
        tabValues().rescan(cpuInfo());
        monitorLayout_->removeWidget(monitor_);
        monitor_->setParent(nullptr);
        delete monitor_;
        monitor_ = new Monitor();
        monitorLayout_->addWidget(monitor_);
        monitor_tab_->setMonitor(monitor_);
        rv = 1;
      }
      else {
        /* Discard */
        store();
        rv = 0;
      }
    }
  }
  btnApply_->setEnabled(false);
}

/*
 * This method is called upon every QEvent::Close event.
 * It shows a dialog asking to discard any modified settings or to cancel the
 * request to exit the application.
 */
void CoreAdjust::handleCloseEvent() {
  if (btnApply_->isEnabled()) {
    /* There are un-applied modifications, ask the user to cancel or discard and exit. */
    auto reply = QMessageBox::warning(
        this, tr("Discard changes?"), tr(
            "<nobr>There are changes to the settings that have not been applied.</nobr><br/>"
        ),
        QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Cancel
    );

    if (reply == QMessageBox::Cancel) {
      /* Cancel: ignore the close event */
      DBGMSG("CoreAdjust::handleCloseEvent(): Canceling QEvent::Close...")
      return;
    }

    /* Discard: reload settings from backup, then exit the application */
    DBGMSG("CoreAdjust::handleCloseEvent(): Discarding new TabSettings...")
    bool s = chkboxSaveOnExit_->isChecked();
    tabSettings().restore();
    for (auto& tpl : vTabMemberWidget_) {
      for (auto* widget : std::get<1>(tpl)) {
        DBGMSG("CoreAdjust::handleCloseEvent(): Calling"
            << widget->metaObject()->className() << "\b::load()")
        widget->load();
      }
    }
    chkboxSaveOnExit_->setChecked(s);
    chkboxApplyOnEvent_->setChecked(tabSettings().applyOnBootAndResume());
  }
  /* Exit the application */
  DBGMSG("CoreAdjust::handleCloseEvent(): Issueing quit signal...")
  qApp->quit();
}

/* SLOTS */

/*
 * Handle stateChanged() event for the 'apply on boot...' checkbox widget.
 * The new state is immediately saved to the configuration file so that the new
 * setting is immediately used by the systemd scripts.
 */
void CoreAdjust::cbApplyOnSysEvent(int) {
  tabSettings().applyOnBootAndResume(chkboxApplyOnEvent_->isChecked());
  tabSettings().applyOnAcpiPowerEvent(chkboxApplyOnEvent_->isChecked());
  QSettings qs(TabSettings::CfgPath, QSettings::IniFormat);
  qs.beginGroup(TabSettings::INI_GRP_COMMON);
  qs.setValue(TabSettings::INI_APPLY_ON_BOOT,
      QVariant::fromValue<bool>(tabSettings().applyOnBootAndResume()));
  qs.setValue(TabSettings::INI_APPLY_ON_ACPI,
      QVariant::fromValue<bool>(tabSettings().applyOnAcpiPowerEvent()));
  qs.endGroup();
  qs.sync();
  DBGMSG("CoreAdjust::cbApplyOnSysEvent(): CommonSettings::applyOnBootAndResume() -->"
      << tabSettings().applyOnBootAndResume())
  DBGMSG("CoreAdjust::cbApplyOnSysEvent(): CommonSettings::applyOnAcpiPowerEvent() -->"
      << tabSettings().applyOnAcpiPowerEvent())
  if (qs.status() != QSettings::NoError) {
    qDebug() << tr("Warning: Could not update the configuration file!");
  }
}

/*
 * This method is called when the 'apply' button is clicked:
 *
 *  - Call TabMemberWidget::store() of the active tab to store the new
 *    settings to TabSettings.
 *
 *  - Call TabMemberWidget::apply() of the active tab to apply the new TabSettings.
 *
 *  - The number of logical cpus may have changed:
 *    - Reinitialize CpuId, CpuInfo and TabValues to adjust them to the new settings.
 *    - Reinitialize the Monitor and MonitorTab instances.
 *
 *  - Call TabMemberWidget::read() of all tabs to refresh the TabValues and to
 *    verify that the settings have been applied.
 *
 *  - Display a dialog if something did not work as expected.
 *
 *  - Call TabMemberWidget::refresh() to load the new TabValues into the ui-items.
 *
 *  - Create a backup of the applied TabSettings.
 *
 *  - Disable the 'apply' button if the settings are successfully applied.
 */
void CoreAdjust::apply() {
  /* Prevent more clicks of the apply button */
  btnApply_->setEnabled(false);

  /* Get the active tab */
  auto& tpl = vTabMemberWidget_[tabMemberIdx_];
  auto* active = std::get<1>(tpl).at(std::get<2>(tpl));
  DBGMSG("CoreAdjust::apply(): Active TabMemberWidget:" << active)

  /* Let the active tab store its new settings */
  DBGMSG("CoreAdjust::apply(): Calling"
      << active->metaObject()->className() << "\b::store()")
  active->store();

  /* Let the active tab apply its new settings */
  DBGMSG("CoreAdjust::apply(): Calling"
      << active->metaObject()->className() << "\b::apply()")
  active->apply();

  /* Refresh the CPUID info, it may have changed. */
  DBGMSG("CoreAdjust::apply(): Calling CpuId::refresh()")
  cpuId().refresh();

  /* Refresh the CpuInfo, it may have changed. */
  DBGMSG("CoreAdjust::apply(): Calling CpuInfo::refresh()")
  cpuInfo().refresh();

  /* Determine/adjust the required size of TabValues */
  DBGMSG("CoreAdjust::apply(): Calling TabValues::rescan()")
  tabValues().rescan(cpuInfo());

  /* Re-read TabValues for all tabs and compare them against the
   * desired settings. Also refresh the UI (if the read was successfull). */
  bool rv = true;
  std::ostringstream ss;
  ss << std::boolalpha << "<ul>";
  for (auto& tpl : vTabMemberWidget_) {
    for (auto* widget : std::get<1>(tpl)) {
      DBGMSG("CoreAdjust::apply(): Calling"
          << widget->metaObject()->className() << "\b::read()")
      if (widget->read(ss) == false) rv = false;
      else {
        DBGMSG("CoreAdjust::apply(): Calling"
            << widget->metaObject()->className() << "\b::refresh()")
        widget->refresh();
      }
    }
  }
  ss << "</ul>";

  /* Recreate the monitor_(tab_) widgets, the number of cpus may have changed */
  monitor_tab_->setMonitor(nullptr);
  monitorLayout_->removeWidget(monitor_);
  monitor_->setParent(nullptr);
  delete monitor_;
  monitor_ = new Monitor();
  monitorLayout_->addWidget(monitor_);
  monitor_tab_->setMonitor(monitor_);

  /* If the newly read values differ from the desired settings
   * then notify the user and re-enable the apply button.
   * Otherwise create a new backup of the settings. */
  if (rv == false) {
    btnApply_->setEnabled(true);
    QMessageBox::warning(this, "Adjusting settings", std::move(QString(
        "<p>(Some of) the adjustments have not been applied:%1</p>").arg(
            ss.str().c_str())));
  }
  else {
    tabSettings().backup();
  }
}

/*
 * This method is called when the 'exit' button was clicked.
 */
void CoreAdjust::btnExit() {
  /* Request the application to close */
  QEvent ev(QEvent::Close);
  QApplication::sendEvent(qApp, &ev);
}

/*
 * This method is called when a TabMemberWidget emits a valueChanged signal
 * and enables the 'apply' button if that TabMemberWidget::compare() method
 * indicates that the setting have indeed been modified.
 */
void CoreAdjust::valueChanged(TabMemberWidget* widget) {
  DBGMSG("CoreAdjust::valueChanged():" << widget)
  btnApply_->setEnabled(!widget->compare());
}

/*
 * This method is called when switching to another tab of tabwidget_
 * It prevents switching to the new tab if there are unapplied settings
 * and displays a dialog asking to cancel the action or to discard the
 * new settings on the current tab and switch to the other tab.
 */
void CoreAdjust::tabSwitch(int index) {
  if (btnApply_->isEnabled()) {
    /* prevent tab switch (by switching back to the previous tab) */
    tabwidget_->blockSignals(true);
    tabwidget_->setCurrentIndex(tabMemberIdx_);
    tabwidget_->blockSignals(false);

    auto reply = QMessageBox::warning(
      this, tr("Discard changes?"), tr(
          "<nobr>Modifications must be applied before switching tabs.</nobr><br/>"
      ),
      QMessageBox::Discard | QMessageBox::Cancel,
      QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
      DBGMSG("CoreAdjust::tabSwitch(): Prevented tab switch.")
      return;
    }
    else {
      /* Discard and reset to backup TabSettings. */
      DBGMSG("CoreAdjust::tabSwitch(): Discarding new TabSettings...")
      bool s = chkboxSaveOnExit_->isChecked();
      tabSettings().restore();
      for (auto& tpl : vTabMemberWidget_) {
        for (auto* widget : std::get<1>(tpl)) {
          DBGMSG("CoreAdjust::tabSwitch(): Calling"
              << widget->metaObject()->className() << "\b::load()")
          widget->load();
          DBGMSG("CoreAdjust::tabSwitch(): Calling"
              << widget->metaObject()->className() << "\b::refresh()")
          widget->refresh();
        }
      }
      chkboxSaveOnExit_->setChecked(s);
      chkboxApplyOnEvent_->setChecked(tabSettings().applyOnBootAndResume());
      btnApply_->setEnabled(false);
      /* Switch to the new tab */
      tabwidget_->blockSignals(true);
      tabwidget_->setCurrentIndex(index);
      tabwidget_->blockSignals(false);
    }
  }
  tabMemberIdx_ = static_cast<unsigned short>(index);
  DBGMSG("CoreAdjust::tabSwitch(): Switching to tab"
      << std::get<1>(vTabMemberWidget_[tabMemberIdx_]).at(
          std::get<2>(vTabMemberWidget_[tabMemberIdx_])))
}

/*
 * This method is called when switching to another tab in a 'processor' tab.
 * It prevents switching to the new tab if there are unapplied settings
 * and displays a dialog asking to cancel the action or to discard the
 * new settings on the current tab and switch to the other tab.
 */
void CoreAdjust::processorTabSwitch(int index) {
  auto& tpl = vTabMemberWidget_[tabMemberIdx_];
  if (btnApply_->isEnabled()) {
    /* prevent tab switch (by switching back to the previous sub-tab) */
    std::get<0>(tpl)->blockSignals(true);
    std::get<0>(tpl)->setCurrentIndex(std::get<2>(tpl));
    std::get<0>(tpl)->blockSignals(false);

    auto reply = QMessageBox::warning(
      this, tr("Discard changes?"), tr(
          "<nobr>Modifications must be applied before switching tabs.</nobr><br/>"
      ),
      QMessageBox::Discard | QMessageBox::Cancel,
      QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
      DBGMSG("CoreAdjust::processorTabSwitch(): Prevented tab switch.")
      return;
    }

    /* Discard and reset to backup TabSettings. */
    DBGMSG("CoreAdjust::processorTabSwitch(): Discarding new TabSettings...")
    bool s = chkboxSaveOnExit_->isChecked();
    tabSettings().restore();
    for (auto& tpl : vTabMemberWidget_) {
      for (auto* widget : std::get<1>(tpl)) {
        DBGMSG("CoreAdjust::processorTabSwitch(): Calling"
            << widget->metaObject()->className() << "\b::load()")
        widget->load();
        DBGMSG("CoreAdjust::processorTabSwitch(): Calling"
            << widget->metaObject()->className() << "\b::refresh()")
        widget->refresh();
      }
    }
    chkboxSaveOnExit_->setChecked(s);
    chkboxApplyOnEvent_->setChecked(tabSettings().applyOnBootAndResume());
    btnApply_->setEnabled(false);
    /* Switch to the new tab */
    std::get<0>(tpl)->blockSignals(true);
    std::get<0>(tpl)->setCurrentIndex(index);
    std::get<0>(tpl)->blockSignals(false);
  }
  std::get<2>(tpl) = static_cast<unsigned short>(index);
  DBGMSG("CoreAdjust::processorTabSwitch(): Switching to tab"
      << std::get<1>(tpl).at(std::get<2>(tpl)))
}

/*
 * This method is called every 500ms and calls TabMemberWidget::timed() of every tab.
 */
void CoreAdjust::timerCallback() {
  /* Call the timed() method of every TabMemberWidget */
  auto& tpl = vTabMemberWidget_[tabMemberIdx_];
  auto* current = std::get<1>(tpl).at(std::get<2>(tpl));
  for (auto& tpl : vTabMemberWidget_) {
    for (auto* widget : std::get<1>(tpl)) {
      widget->timed(widget == current);
    }
  }
}

