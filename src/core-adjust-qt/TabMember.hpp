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

#ifndef CoreAdjust_TabMember
#define CoreAdjust_TabMember

#include "config.h"

#ifndef CONFIG_FILE
#error CONFIG_FILE is undefined!
//#define CONFIG_FILE "/etc/core-adjust/core-adjust.ini"
#endif

#ifndef SCRIPT_EXEC
#error SCRIPT_EXEC is undefined!
//#define SCRIPT_EXEC "core-adjust"
#endif

#ifndef GRUB_CFG_FILE
#error GRUB_CFG_FILE is undefined!
//#define GRUB_CFG_FILE "/etc/default/grub"
#endif

//#undef SCRIPT_EXEC
//#define SCRIPT_EXEC "src/core-adjust/core-adjust"

// STL
#include <vector>
// App
#include "CpuFreqUtils.hpp"
#include "CpuNumber.hpp"
#include "Dbg.hpp"
#include "MiscEnable.hpp"
#include "SpeedControl.hpp"
#include "SmtControl.hpp"
#include "TabMemberBase.hpp"
#include "ThermalControl.hpp"
#include "VoltageOffsets.hpp"

class CommonSettings
  : public SmtControl::CommonSettings {

  private:
    bool save_on_exit_ { true };
    bool apply_on_boot_and_resume_ { false };
    bool apply_on_acpi_power_event_ { false };

  public:
    bool saveOnExit() const;
    void saveOnExit(bool state);
    bool applyOnBootAndResume() const;
    void applyOnBootAndResume(bool state);
    bool applyOnAcpiPowerEvent() const;
    void applyOnAcpiPowerEvent(bool state);
};

class TabMemberValues
  : public ThermalControl::Values,
    public VoltageOffsets::Values,
    public SpeedControl::Values,
    public SmtControl::Values,
    public CpuFreqUtils::Values,
    public MiscEnable::Values {
  public:
    TabMemberValues(const SingleCpuInfo& ci)
      : ThermalControl::Values(),
        VoltageOffsets::Values(),
        SpeedControl::Values(),
        SmtControl::Values(),
        CpuFreqUtils::Values(ci),
        MiscEnable::Values()
    { }
    ~TabMemberValues() = default;
};

class CommonValues
  : public SmtControl::CommonValues {
};

class TabValues : public CommonValues, private std::vector<TabMemberValues> {
  public:
    using std::vector<TabMemberValues>::begin;
    using std::vector<TabMemberValues>::cbegin;
    using std::vector<TabMemberValues>::end;
    using std::vector<TabMemberValues>::cend;
    using std::vector<TabMemberValues>::size;

    TabValues(const CpuInfo& cpuInfo);
    ~TabValues() = default;

    TabMemberValues& operator[](const PhysCpuNr& idx);
    const TabMemberValues& operator[](const PhysCpuNr& idx) const;

    TabMemberValues& at(const PhysCpuNr& idx);
    const TabMemberValues& at(const PhysCpuNr& idx) const;

    void rescan(const CpuInfo& cpuInfo);

#ifdef DEBUG
  private:
    static TabValues* singleton_;
#endif
};

class TabMemberSettings
  : public ThermalControl::Settings,
    public VoltageOffsets::Settings,
    public SpeedControl::Settings,
    public SmtControl::Settings,
    public CpuFreqUtils::Settings,
    public MiscEnable::Settings {

  public:

    explicit TabMemberSettings(
      const SingleCpuInfo& ci, const PhysCpuNr& p, CommonSettings& c)
      : ThermalControl::Settings(),
        VoltageOffsets::Settings(),
        SpeedControl::Settings(),
        SmtControl::Settings(),
        CpuFreqUtils::Settings(ci),
        MiscEnable::Settings(),
        processor_number_(p),
        common_settings_(c),
        cpuInfo_(ci) { }

    ~TabMemberSettings() override = default;

    void load(QSettings& qs, const TabMemberValues& cv) override {
      ThermalControl::Settings::load(qs, cv);
      VoltageOffsets::Settings::load(qs, cv);
      SpeedControl::Settings::load(qs, cv);
      SmtControl::Settings::load(qs, cv);
      CpuFreqUtils::Settings::load(qs, cv);
      MiscEnable::Settings::load(qs, cv);
    }

    void save(QSettings& qs) override {
      ThermalControl::Settings::save(qs);
      VoltageOffsets::Settings::save(qs);
      SpeedControl::Settings::save(qs);
      SmtControl::Settings::save(qs);
      CpuFreqUtils::Settings::save(qs);
      MiscEnable::Settings::save(qs);
    }

    const PhysCpuNr& processorNumber();
    TabMemberSettings& tabMemberSettings() override;
    CommonSettings& commonSettings() override;
    const SingleCpuInfo& cpuInfo() const override;

  private:
    PhysCpuNr processor_number_;
    std::reference_wrapper<CommonSettings> common_settings_;
    std::reference_wrapper<const SingleCpuInfo> cpuInfo_;
};

class TabSettings
  : public CommonSettings,
    private std::vector<TabMemberSettings> {

  public:
    static constexpr const char* INI_GRP_COMMON { "Common" };
    static constexpr const char* INI_GRP_PROCESSOR { "Processor" };

    static constexpr const char* INI_SAVE_ON_EXIT { "GUI_Save_On_Exit" };
    static constexpr const char* INI_APPLY_ON_BOOT { "Apply_On_Boot_And_Resume" };
    static constexpr const char* INI_APPLY_ON_ACPI { "Apply_On_Acpi_Power_Event" };

    static constexpr const char* CfgPath { CONFIG_FILE };
    static constexpr const char* ScriptPath { SCRIPT_EXEC };
    static constexpr const char* GrubCfgPath { GRUB_CFG_FILE };

    static constexpr const char* Cmd_IsDaemonEnabled { "/bin/systemctl is-enabled core-ajust" };
    static constexpr const char* Cmd_OnAcPower { "/usr/bin/on_ac_power" };
    static constexpr const char* Cmd_UpdateGrub { "/usr/sbin/update-grub" };

    using std::vector<TabMemberSettings>::begin;
    using std::vector<TabMemberSettings>::cbegin;
    using std::vector<TabMemberSettings>::end;
    using std::vector<TabMemberSettings>::cend;
    using std::vector<TabMemberSettings>::size;

    explicit TabSettings(const CpuInfo&, TabValues&);
    ~TabSettings();

    /* Only the 'root' instance may save settings to the INI file.
     * We need a copy ctor that tells the new instance that its a copy
     * so that it never saves to the INI file when that copy is destroyed.
     * ( see SmtControl::compare() ) */
    TabSettings(const TabSettings&);

    TabMemberSettings& operator[](const PhysCpuNr& p);
    const TabMemberSettings& operator[](const PhysCpuNr& p) const;

    TabMemberSettings& at(const PhysCpuNr& p);
    const TabMemberSettings& at(const PhysCpuNr& p) const;

    void backup();
    void restore();

  private:
    bool is_copy_ { false };
    struct {
      std::vector<TabMemberSettings> per_cpu_settings_;
      CommonSettings global_settings_;
    } backup_;
};

#endif

