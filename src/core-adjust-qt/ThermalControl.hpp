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

#ifndef CoreAdjust_ThermalControl
#define CoreAdjust_ThermalControl

// STL
#include <sstream>
// Qt
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QScrollArea>
#include <QWidget>
// App
#include "SaferSlider.hpp"
#include "ShellCommand.hpp"
#include "TabMemberBase.hpp"

class ThermalControl final : public TabMemberTemplate {
  Q_OBJECT
  public:
    class Values;
    class Settings;

    explicit ThermalControl(
        const SingleCpuInfo&,
        const SingleCpuId&,
        TabMemberValues&,
        TabMemberSettings&,
        QWidget *parent = nullptr);

    ~ThermalControl() override = default;

    void load() override;
    void store() override;
    void refresh() override;
    bool read(std::ostringstream& err) override;
    bool apply() override;
    bool compare() override;
    void timed(bool) override;

  private:
    void store(Settings&);

    int timed_count_ { 0 };
    bool on_ac_power_;
    ShellCommand shell_;

    QLabel* tm1_supported_;
    QLabel* tm2_supported_;
    QLabel* tj_offset_;
    QLabel* target_temp_;

    QRadioButton* tm1_select_;
    QRadioButton* tm2_select_;
    QRadioButton* tm_default_;

    QGroupBox* box2_;
    QCheckBox* temp_enable_;
    QCheckBox* batt_enable_;
    QLabel* ac_label_;
    QLabel* batt_label_;
    SaferSlider* ac_slider_;
    SaferSlider* batt_slider_;
    QLabel* ac_value_;
    QLabel* batt_value_;

  private slots:
    void toggledSlot(bool);
    void enableCheckedSlot(int);
    void battEnableCheckedSlot(int);
    void targetTemperatureChangedSlot(int);
    void battTargetTemperatureChangedSlot(int);
};

class ThermalControl::Values {
  private:
    size_t target_temperature_ { 100 };
    size_t target_temperature_offset_ { 0 };
    bool tm_select_ { false }; 
    bool tm2_enable_ { false }; 

  public:
    size_t targetTemperature() const;
    size_t targetTemperatureOffset() const;
    void targetTemperature(size_t t);
    void targetTemperatureOffset(size_t t);
    bool tmSelect() const;
    void tmSelect(bool state);
    bool tm2Enable() const;
    void tm2Enable(bool state);
};

class ThermalControl::Settings
  : private ThermalControl::Values,
    virtual protected TabMemberWidget::Settings {

  friend class ThermalControl;

  public:
    Settings() = default;
    ~Settings() override = default;

  protected:
    void load(QSettings&, const TabMemberValues&) override;
    void save(QSettings&) override;

  private:
    size_t targetTemperatureOffsetBattery() const;
    void targetTemperatureOffsetBattery(size_t offset);
    bool targetTemperatureEnabled() const;
    bool targetTemperatureBatteryEnabled() const;
    void targetTemperatureEnabled(bool state);
    void targetTemperatureBatteryEnabled(bool state);
    bool tmSelectEnabled() const;
    void tmSelectEnabled(bool state);
    bool tm2EnableEnabled() const;
    void tm2EnableEnabled(bool state);

    static constexpr const char* INI_TARGET_TEMPERATURE_AC { "Target_Temperature" };
    static constexpr const char* INI_TARGET_TEMPERATURE_BATT { "Target_Temperature_Battery" };
    static constexpr const char* INI_TARGET_TEMPERATURE_ENABLE { "Target_Temperature_Enabled" };
    static constexpr const char* INI_TARGET_TEMPERATURE_ENABLE_BATT { "Target_Temperature_Battery_Enabled" };
    static constexpr const char* INI_TM_SELECT { "TM_Select" };
    static constexpr const char* INI_TM_SELECT_ENABLE { "TM_Select_Enabled" };
    static constexpr const char* INI_TM2_ENABLE { "TM2_Enable" };
    static constexpr const char* INI_TM2_ENABLE_ENABLE { "TM2_Enable_Enabled" };

    bool enable_temperature_target_ { false };
    bool enable_battery_target_ { false };
    bool enable_tm_select_ { false };
    bool enable_tm2_enable_ { false };
    size_t target_temperature_offset_battery_ { 0 };
};


inline size_t ThermalControl::Values::targetTemperature() const {
  return target_temperature_;
}

inline void ThermalControl::Values::targetTemperature(size_t v) {
  target_temperature_ = v;
}

inline size_t ThermalControl::Values::targetTemperatureOffset() const {
  return target_temperature_offset_;
}

inline void ThermalControl::Values::targetTemperatureOffset(size_t v) {
  target_temperature_offset_ = v;
}

inline bool ThermalControl::Values::tmSelect() const {
  return tm_select_;
}

inline void ThermalControl::Values::tmSelect(bool v) {
  tm_select_ = v;
}

inline bool ThermalControl::Values::tm2Enable() const {
  return tm2_enable_;
}

inline void ThermalControl::Values::tm2Enable(bool v) {
  tm2_enable_ = v;
}


inline size_t ThermalControl::Settings::targetTemperatureOffsetBattery() const {
  return target_temperature_offset_battery_;
}

inline void ThermalControl::Settings::targetTemperatureOffsetBattery(size_t v) {
  target_temperature_offset_battery_ = v;
}

inline bool ThermalControl::Settings::targetTemperatureBatteryEnabled() const {
  return enable_battery_target_;
}

inline void ThermalControl::Settings::targetTemperatureBatteryEnabled(bool v) {
  enable_battery_target_ = v;
}

inline bool ThermalControl::Settings::targetTemperatureEnabled() const {
  return enable_temperature_target_;
}

inline void ThermalControl::Settings::targetTemperatureEnabled(bool v) {
  enable_temperature_target_ = v;
}

inline bool ThermalControl::Settings::tmSelectEnabled() const {
  return enable_tm_select_;
}

inline void ThermalControl::Settings::tmSelectEnabled(bool v) {
  enable_tm_select_ = v;
}

inline bool ThermalControl::Settings::tm2EnableEnabled() const {
  return enable_tm2_enable_;
}

inline void ThermalControl::Settings::tm2EnableEnabled(bool v) {
  enable_tm2_enable_ = v;
}

#endif

