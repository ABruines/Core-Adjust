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
  * @file src/core-adjust-qt/TabMember.hpp
  * @brief Classes for members of a QTabWidget in the Core Adjust application.
  *
  * @file src/core-adjust-qt/TabMember.cpp
  * @brief Classes for members of a QTabWidget in the Core Adjust application (implementation).
  *
  * @class CommonSettings
  * @brief Data for in the Common section in the INI file.
  *
  * This class stores all TabMemberWidget::Common settings
  * (ie. has all TabMemberWidget classes that implement a class 'Common' as a base class).
  * These settings are 'global' in the sense that they apply to all processors.
  * The settings defined in this class are accessible to anyone,
  * but the settings in the inherited classes should only be accessible to that
  * particular (TabMemberWidget) class.
  *
  * @fn bool CommonSettings::saveOnExit() const
  * @brief Save setting upon exit?
  *
  *'true' when the INI file must be re-written with the currently
  * applied settings upon exit.
  *
  * @fn void CommonSettings::saveOnExit(bool state)
  * @brief Save setting upon exit?
  *
  *'true' when the INI file must be re-written with the currently
  * applied settings upon exit.
  *
  * @fn bool applyOnBootAndResume() const
  * @brief Apply settings on boot and resume?
  *
  * 'true' to instruct the core-adjust service to apply the settings
  * in the INI file on (re-)boot and resume events.
  *
  * @fn void applyOnBootAndResume(bool state)
  * @brief Apply settings on boot and resume?
  *
  * 'true' to instruct the core-adjust service to apply the settings
  * in the INI file on (re-)boot and resume events.
  *
  * @fn bool applyOnAcpiPowerEvent() const
  * @brief Apply settings on power events?
  *
  * 'true' to instruct the core-adjust service to apply the settings
  * in the INI file on ACPI power events (ie. when switching between
  * AC and batteru power).
  *
  * @fn void applyOnAcpiPowerEvent(bool state)
  * @brief Apply settings on power events?
  *
  * 'true' to instruct the core-adjust service to apply the settings
  * in the INI file on ACPI power events (ie. when switching between
  * AC and battery power).
  *
  *
  *
  * @class TabMemberValues
  * @brief Class that inherits all TabMemberWidget::Values classes meant for a given processor.
  *
  * Class TabMemberValues has a dual purpose. The first is as internal storage
  * for the current values of the UI items inside of a TabMemberWidget.
  * These values are also required to be a part of class TabMemberSettings
  * so it has this class as its base.
  *
  *
  *
  * @class TabValues
  * @brief Vector of TabMemberValues with one entry for each processor.
  *
  * Class TabValues has a dual purpose. The first is as internal storage
  * for the current values of the UI items inside of a TabMemberWidget.
  * These values are also required to be a part of class TabSettings
  * so it has this class as its base.
  *
  * @fn TabMemberValues& TabValues::operator[](const PhysCpuNr& idx);
  * @brief Returns the TabMemberValues for the given processor number.
  * @param idx The processor number.
  * @return A reference to that TabMemberValues.
  *
  * @fn const TabMemberValues& TabValues::operator[](const PhysCpuNr& idx) const;
  * @brief Returns the TabMemberValues for the given processor number.
  * @param idx The processor number.
  * @return A const reference to that TabMemberValues.
  *
  * @fn TabMemberValues& TabValues::at(const PhysCpuNr& idx);
  * @brief Returns the TabMemberValues for the given processor number.
  * @param idx The processor number.
  * @return A reference to that TabMemberValues.
  *
  * @fn const TabMemberValues& TabValues::at[](const PhysCpuNr& idx) const;
  * @brief Returns the TabMemberValues for the given processor number.
  * @param idx The processor number.
  * @return A const reference to that TabMemberValues.
  *
  * @fn std::vector<TabMemberValues>::iterator TabValues::begin() noexcept
  * @brief Return iterator to beginning of the vector
  *
  * @fn std::vector<TabMemberValues>::const_iterator TabValues::cbegin() const noexcept
  * @brief Return const_iterator to beginning of the vector
  *
  * @fn std::vector<TabMemberValues>::iterator TabValues::end() noexcept
  * @brief Return iterator to end of the vector
  *
  * @fn std::vector<TabMemberValues>::const_iterator TabValues::cend() const noexcept
  * @brief Return const_iterator to end of the vector
  *
  * @fn std::vector<TabMemberValues>::size_type TabValues::size() const noexcept
  * @brief Return size of the vector
  *
  * @fn void TabValues::rescan()
  * @brief Rescan the required number of TabMemberValues instances and create/delete them if needed.
  *
  *
  * @class TabMemberSettings
  * @brief Settings container for all TabMemberWidget meant for a given processor.
  *
  * This class inherits all classes that implement TabMemberWidget::Settings
  * and implements the final override for the load() and save() methods.
  * It also overrides the two methods common() and data(), so that they give access to the
  * CommonSettings instance stored in class TabSettings and this TabMemberSettings instance
  * when called from within the TabMemberWidget::Settings implementations.
  *
  * @fn explicit TabMemberSettings::TabMemberSettings(const PhysCpuNr& p, CommonSettings& c)
  * @param p The processor this instance is meant for.
  * @param c Reference to the TabSettings instance.
  *
  * @fn const PhysCpuNr& TabMemberSettings::processorNumber()
  * @brief Returns the PhysCpuNr this instance is meant for.
  *
  * @fn void TabMemberSettings::load(QSettings& qs, const SingleCpuInfo& ci, const TabMemberValues& cv) override
  * @brief Load the settings for this TabMemberWidget
  * @param qs The QSettings instance to use.
  * @param ci Information about this processor.
  * @param cv The currently applied configuration values.
  * @note This method is called once for every processor.
  *
  * @fn void TabMemberSettings::save(QSettings& qs) override
  * @brief Save the settings for this TabMemberWidget
  * @param qs The QSettings instance to use.
  * @note This method is called once for every processor.
  *
  * @fn TabMemberSettings& TabMemberSettings::data() override
  * @brief Get this TabMemberSettings instance.
  *
  * @fn CommonSettings& TabMemberSettings::common() override
  * @brief Get the global CommonSettings instance.
  *
  *
  *
  * @class TabSettings
  * @brief Container for the data stored in the configuartion file.
  *
  * This class inherits and provides acces to an instance of class
  * CommonSettings and a vector of class TabMemberSettings to the application.
  * Most methods of the inherited classes are only accessible from their
  * respective TabMemberWidget implementations.
  * The load() and save() methods they provide are used by this class to
  * manage the configuartion file.
  *
  * The settings are read from an INI file when the application starts,
  * modified by the application and then optionaly written back to the
  * INI file when the application exits depending on the state of
  * CommonSettings::saveOnExit().
  *
  * @fn explicit TabSettings::TabSettings(const CpuInfo& ci, TabValues& cv)
  * @param ci The CpuInfo instance to use.
  * @param cv The TabValues instance to use.
  *
  * @fn TabMemberSettings& TabSettings::operator[](const PhysCpuNr& p)
  * @brief Retrieve the TabMemberSettings for a given processor.
  * @param p The ordinal number of the processor.
  * @returns The TabMemberSettings instance.
  *
  * @fn TabMemberSettings& TabSettings::at(const PhysCpuNr& p)
  * @brief Retrieve the TabMemberSettings for a given processor.
  * @param p The ordinal number of the processor.
  * @returns The TabMemberSettings instance.
  *
  * @fn const TabMemberSettings& TabSettings::operator[](const PhysCpuNr& p) const
  * @brief Retrieve the TabMemberSettings for a given processor.
  * @param p The ordinal number of the processor.
  * @returns The TabMemberSettings instance.
  *
  * @fn const TabMemberSettings& TabSettings::at(const PhysCpuNr& p) const
  * @brief Retrieve the TabMemberSettings for a given processor.
  * @param p The ordinal number of the processor.
  * @returns The TabMemberSettings instance.
  *
  * @fn std::vector<TabMemberSettings>::iterator TabSettings::begin() noexcept
  * @brief Return iterator to beginning of the vector
  *
  * @fn std::vector<TabMemberSettings>::const_iterator TabSettings::cbegin() const noexcept
  * @brief Return const_iterator to beginning of the vector
  *
  * @fn std::vector<TabMemberSettings>::iterator TabSettings::end() noexcept
  * @brief Return iterator to end of the vector
  *
  * @fn std::vector<TabMemberSettings>::const_iterator TabSettings::cend() const noexcept
  * @brief Return const_iterator to end of the vector
  *
  * @fn std::vector<TabMemberSettings>::size_type TabSettings::size() const noexcept
  * @brief Return size of the vector
  *
  * @fn void TabSettings::backup()
  * @brief Create an in-memory backup of all settings.
  *
  * @fn void TabSettings::restore()
  * @brief Restore the in-memory backup of all settings.
  */
#include <sstream>
#include <QDebug>
#include <QMessageBox>
#include "TabMember.hpp"
#include "Shell.hpp"
#include "Dbg.hpp"

/*
 * Class CommonSettings impl
 */

bool CommonSettings::saveOnExit() const {
  return save_on_exit_;
}

void CommonSettings::saveOnExit(bool v) {
  save_on_exit_ = v;
}

bool CommonSettings::applyOnBootAndResume() const {
  return apply_on_boot_and_resume_;
}

void CommonSettings::applyOnBootAndResume(bool v) {
  apply_on_boot_and_resume_ = v;
}

bool CommonSettings::applyOnAcpiPowerEvent() const {
  return apply_on_acpi_power_event_;
}

void CommonSettings::applyOnAcpiPowerEvent(bool v) {
  apply_on_acpi_power_event_ = v;
}

/*
 * Class TabMemberSettings impl
 */

const PhysCpuNr& TabMemberSettings::processorNumber() {
  return processor_number_;
}

TabMemberSettings& TabMemberSettings::tabMemberSettings() {
  return *this;
}

CommonSettings& TabMemberSettings::commonSettings() {
  return common_settings_;
}

const SingleCpuInfo& TabMemberSettings::cpuInfo() const {
  return cpuInfo_;
}

/*
 * Class TabValues impl
 */

#ifdef DEBUG
TabValues* TabValues::singleton_ = nullptr;
#endif

TabValues::TabValues(const CpuInfo& cpuInfo) {
#ifdef DEBUG
  if (singleton_ != nullptr) {
    throw std::runtime_error("There can only be one TabValues instance!");
  }
  singleton_ = this;
#endif

  /* Count the number of processors */
  size_t num_phys_cpu = 0;
  std::vector<std::string> output;
  auto rv = xxx::shell_command(
    { TabSettings::ScriptPath, "--list" },
    [&output](auto, auto str){
      output.push_back(std::move(str));
      return 0;
    }
  );

  /* on success each line of the output represents a processor */
  if (!rv) {
    num_phys_cpu = output.size();
  }
  if (!num_phys_cpu) {
    QMessageBox::critical(nullptr, "Core Adjust",
        "<p><b>Error, unexpected output from command:</b></p>"
        "<nobr>core-adjust --list</nobr>");
    /* The Qt event loop is not running yet, so use exit() to quit */
    exit(EXIT_FAILURE);
  }

  /* loop over all detected processors */
  for (PhysCpuNr p(0); p.value < num_phys_cpu; ++p) {
    /* Create the new TabValues entry */
    emplace_back(cpuInfo[p]);
  }

  DBGMSG("TabValues(): Succesfully initialized TabMemberValues instance(s) for" << size() << "processor(s)")
}

void TabValues::rescan(const CpuInfo& cpuInfo) {
  /* Count the number of processors */
  size_t num_phys_cpu = 0;
  std::vector<std::string> output;
  auto rv = xxx::shell_command(
    { TabSettings::ScriptPath, "--list" },
    [&output](auto, auto str){
      output.push_back(std::move(str));
      return 0;
    }
  );

  /* on success each line of the output represents a processor */
  if (!rv) {
    num_phys_cpu = output.size();
  }
  if (!num_phys_cpu) {
    QMessageBox::critical(nullptr, "Core Adjust",
        "<p><b>Error, unexpected output from command:</b></p>"
        "<nobr>core-adjust --list</nobr>");
    // fixme? the event loop may be running.
    // 1st called from main() - before - the event loop is running,
    // called later by CoreAdjust when it is running...
    // lets assume any 2nd or later call always succeeds
    exit(EXIT_FAILURE);
  }

  /* loop over all detected processors */
  clear();
  for (PhysCpuNr p(0); p.value < num_phys_cpu; ++p) {
    /* Create the new TabValues entry */
    emplace_back(cpuInfo[p]);
  }

  DBGMSG("TabValues::rescan(): Succesfully re-initialized TabMemberValues instance(s) for" << size() << "processor(s)")
}

TabMemberValues& TabValues::operator[](const PhysCpuNr& idx) {
  return std::vector<TabMemberValues>::operator[](idx.value);
}

const TabMemberValues& TabValues::operator[](const PhysCpuNr& idx) const {
  return std::vector<TabMemberValues>::operator[](idx.value);
}

TabMemberValues& TabValues::at(const PhysCpuNr& idx) {
  return std::vector<TabMemberValues>::at(idx.value);
}

const TabMemberValues& TabValues::at(const PhysCpuNr& idx) const {
  return std::vector<TabMemberValues>::at(idx.value);
}

/*
 * Class TabSettings impl
 */

TabSettings::TabSettings(const CpuInfo& cpuInfo, TabValues& tabValues) {

  QSettings qs(CfgPath, QSettings::IniFormat);
  if (qs.allKeys().isEmpty()) {
    qDebug() << "Warning: Could not read configuration from file!";
  }

  qs.beginGroup(INI_GRP_COMMON);
  applyOnBootAndResume(qs.value(INI_APPLY_ON_BOOT, false).toBool());
  applyOnAcpiPowerEvent(qs.value(INI_APPLY_ON_ACPI, false).toBool());
  saveOnExit(qs.value(INI_SAVE_ON_EXIT, false).toBool());
  qs.endGroup();

  DBGMSG("TabSettings(): CommonSettings::applyOnBootAndResume() <--" << applyOnBootAndResume())
  DBGMSG("TabSettings(): CommonSettings::applyOnAcpiPowerEvent() <--" << applyOnAcpiPowerEvent())
  DBGMSG("TabSettings(): CommonSettings::saveOnExit() <--" << saveOnExit())

  auto ci_iter = cpuInfo.begin();
  auto tv_iter = tabValues.begin();
  for (; tv_iter != tabValues.end() && ci_iter != cpuInfo.end();
      ++tv_iter, ++ci_iter) {
    DBGMSG("TabSettings(): Loading TabMemberSettings for processor:" << ci_iter->physicalId().value + 1)
    emplace_back(cpuInfo[ci_iter->physicalId()], ci_iter->physicalId(), *this);
    back().load(qs, *tv_iter);
  }

  backup();
}

TabSettings::TabSettings(const TabSettings& t)
  : CommonSettings(std::forward<const CommonSettings>(t)),
    std::vector<TabMemberSettings>(std::forward<const std::vector<TabMemberSettings>>(t)) {
  backup_ = t.backup_;
  is_copy_ = true;
}

TabSettings::~TabSettings() {
  if (is_copy_ == false) {
    QSettings qs(CfgPath, QSettings::IniFormat);

    DBGMSG("~TabSettings(): CommonSettings::applyOnBootAndResume() -->" << applyOnBootAndResume())
    DBGMSG("~TabSettings(): CommonSettings::applyOnAcpiPowerEvent() -->" << applyOnAcpiPowerEvent())
    DBGMSG("~TabSettings(): CommonSettings::saveOnExit() -->" << saveOnExit())
    qs.beginGroup(INI_GRP_COMMON);
    qs.setValue(INI_APPLY_ON_BOOT, QVariant::fromValue<bool>(applyOnBootAndResume()));
    qs.setValue(INI_APPLY_ON_ACPI, QVariant::fromValue<bool>(applyOnAcpiPowerEvent()));
    qs.setValue(INI_SAVE_ON_EXIT, QVariant::fromValue<bool>(saveOnExit()));
    qs.endGroup();

    if (saveOnExit()) {
      DBGMSG("~TabSettings(): Saving TabMemberSettings...")
      for (auto& tabMemberSettings : *this) {
        tabMemberSettings.save(qs);
      }
    }
    else {
      DBGMSG("~TabSettings(): Do NOT save TabMemberSettings...")
    }

    qs.sync();
    if (qs.status() != QSettings::NoError) {
      qDebug() << "Warning: Could not update the configuration file!";
    }
  }
}

TabMemberSettings& TabSettings::operator[](const PhysCpuNr& p) {
  return std::vector<TabMemberSettings>::at(p.value);
}

TabMemberSettings& TabSettings::at(const PhysCpuNr& p) {
  return std::vector<TabMemberSettings>::at(p.value);
}

const TabMemberSettings& TabSettings::operator[](const PhysCpuNr& p) const {
  return std::vector<TabMemberSettings>::at(p.value);
}

const TabMemberSettings& TabSettings::at(const PhysCpuNr& p) const {
  return std::vector<TabMemberSettings>::at(p.value);
}

void TabSettings::backup() {
  DBGMSG("TabSettings::backup()")
  backup_.per_cpu_settings_ = *static_cast<std::vector<TabMemberSettings>*>(this);
  backup_.global_settings_ = *static_cast<CommonSettings*>(this);
}

void TabSettings::restore() {
  DBGMSG("TabSettings::restore()")
  *static_cast<std::vector<TabMemberSettings>*>(this) = backup_.per_cpu_settings_;
  *static_cast<CommonSettings*>(this) = backup_.global_settings_;
}

