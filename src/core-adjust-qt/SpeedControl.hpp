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

#ifndef CoreAdjust_SpeedControl
#define CoreAdjust_SpeedControl

// Qt
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
// App
#include "SaferSlider.hpp"
#include "ShellCommand.hpp"
#include "TabMemberBase.hpp"

class SpeedControl final : public TabMemberTemplate {
  Q_OBJECT
  public:
    class Values;
    class Settings;

    explicit SpeedControl(
        const SingleCpuInfo&,
        const SingleCpuId&,
        TabMemberValues&,
        TabMemberSettings&,
        QWidget *parent = nullptr);

    ~SpeedControl() override = default;

    void load() override;
    void store() override;
    void refresh() override;
    bool read(std::ostringstream& err) override;
    bool apply() override;
    bool compare() override;
    void timed(bool) override {}

  private:
    void store(Settings&);

    ShellCommand shell_;

    QLabel* EIST_enable_name_;
    QRadioButton* EIST_enable_on_;
    QRadioButton* EIST_enable_off_;
    QRadioButton* EIST_enable_default_;
    QButtonGroup* EIST_enable_grp_;
    QLabel* EIST_enable_current_;

    QLabel* EIST_lock_name_;
    QRadioButton* EIST_lock_on_;
    QRadioButton* EIST_lock_off_;
    QRadioButton* EIST_lock_default_;
    QButtonGroup* EIST_lock_grp_;
    QLabel* EIST_lock_current_;

    QLabel* IDA_disable_name_;
    QRadioButton* IDA_disable_on_;
    QRadioButton* IDA_disable_off_;
    QRadioButton* IDA_disable_default_;
    QButtonGroup* IDA_disable_grp_;
    QLabel* IDA_disable_current_;

    QLabel* Max_nonturbo_ratio_lock_name_;
    QRadioButton* Max_nonturbo_ratio_lock_on_;
    QRadioButton* Max_nonturbo_ratio_lock_off_;
    QRadioButton* Max_nonturbo_ratio_lock_default_;
    QButtonGroup* Max_nonturbo_ratio_lock_grp_;
    QCheckBox* Max_nonturbo_ratio_enable_;
    QSpinBox* Max_nonturbo_ratio_;
    QLabel* Max_nonturbo_ratio_current_;

    QTabWidget* tabs_ratio_limits_;
    QCheckBox* enable_ratio_limit_;
    QLabel* enable_ratio_limit_current_;

    QLabel* label_[18];
    QSpinBox* spinner_[18];
    QLabel* current_[18];
    QLabel* arrow_[18];
    QLabel* note_[18];

  private slots:
    void toggledSlot(int id, bool checked);
    void Max_nonturbo_ratio_enable_changed(int state);
    void Max_nonturbo_ratio_changed(int value);
    void enable_ratio_limit_changed(int state);
    void spinner_changed(int value);
};

class SpeedControl::Values {
  private:
    bool eist_enable_ { false };
    bool eist_lock_ { false };
    bool tbt_disable_ { false };
    uint8_t tbt_activation_ratio_ { 0 };
    bool tbt_activation_ratio_lock_ { false };
    uint8_t tbt_ratio_limit_1c_ { 0 };
    uint8_t tbt_ratio_limit_2c_ { 0 };
    uint8_t tbt_ratio_limit_3c_ { 0 };
    uint8_t tbt_ratio_limit_4c_ { 0 };
    uint8_t tbt_ratio_limit_5c_ { 0 };
    uint8_t tbt_ratio_limit_6c_ { 0 };
    uint8_t tbt_ratio_limit_7c_ { 0 };
    uint8_t tbt_ratio_limit_8c_ { 0 };
    uint8_t tbt_ratio_limit_9c_ { 0 };
    uint8_t tbt_ratio_limit_10c_ { 0 };
    uint8_t tbt_ratio_limit_11c_ { 0 };
    uint8_t tbt_ratio_limit_12c_ { 0 };
    uint8_t tbt_ratio_limit_13c_ { 0 };
    uint8_t tbt_ratio_limit_14c_ { 0 };
    uint8_t tbt_ratio_limit_15c_ { 0 };
    uint8_t tbt_ratio_limit_16c_ { 0 };
    uint8_t tbt_ratio_limit_17c_ { 0 };
    uint8_t tbt_ratio_limit_18c_ { 0 };
    /* not a value that is read from the cpu,
     * but it is required here for comparison
     * in CpuValuesBase::compare() */
    bool tbt_ratio_limit_enable_ { false };
  public:
    bool eistEnable() const;
    void eistEnable(bool state);
    bool eistLock() const;
    void eistLock(bool state);
    bool tbtDisable() const;
    void tbtDisable(bool state);
    uint8_t tbtActivationRatio() const;
    void tbtActivationRatio(uint8_t ratio);
    bool tbtActivationRatioLock() const;
    void tbtActivationRatioLock(bool state);
    uint8_t tbtRatioLimit1C() const;
    void tbtRatioLimit1C(uint8_t ratio);
    uint8_t tbtRatioLimit2C() const;
    void tbtRatioLimit2C(uint8_t ratio);
    uint8_t tbtRatioLimit3C() const;
    void tbtRatioLimit3C(uint8_t ratio);
    uint8_t tbtRatioLimit4C() const;
    void tbtRatioLimit4C(uint8_t ratio);
    uint8_t tbtRatioLimit5C() const;
    void tbtRatioLimit5C(uint8_t ratio);
    uint8_t tbtRatioLimit6C() const;
    void tbtRatioLimit6C(uint8_t ratio);
    uint8_t tbtRatioLimit7C() const;
    void tbtRatioLimit7C(uint8_t ratio);
    uint8_t tbtRatioLimit8C() const;
    void tbtRatioLimit8C(uint8_t ratio);
    uint8_t tbtRatioLimit9C() const;
    void tbtRatioLimit9C(uint8_t ratio);
    uint8_t tbtRatioLimit10C() const;
    void tbtRatioLimit10C(uint8_t ratio);
    uint8_t tbtRatioLimit11C() const;
    void tbtRatioLimit11C(uint8_t ratio);
    uint8_t tbtRatioLimit12C() const;
    void tbtRatioLimit12C(uint8_t ratio);
    uint8_t tbtRatioLimit13C() const;
    void tbtRatioLimit13C(uint8_t ratio);
    uint8_t tbtRatioLimit14C() const;
    void tbtRatioLimit14C(uint8_t ratio);
    uint8_t tbtRatioLimit15C() const;
    void tbtRatioLimit15C(uint8_t ratio);
    uint8_t tbtRatioLimit16C() const;
    void tbtRatioLimit16C(uint8_t ratio);
    uint8_t tbtRatioLimit17C() const;
    void tbtRatioLimit17C(uint8_t ratio);
    uint8_t tbtRatioLimit18C() const;
    void tbtRatioLimit18C(uint8_t ratio);
    bool tbtRatioLimitEnable() const;
    void tbtRatioLimitEnable(bool state);
};

class SpeedControl::Settings
  : private SpeedControl::Values,
    virtual protected TabMemberWidget::Settings {

  friend class SpeedControl;

  public:
    Settings() = default;
    ~Settings() override = default;

  protected:
    void load(QSettings&, const TabMemberValues&) override;
    void save(QSettings&) override;

  private:
    bool eistEnableEnabled() const;
    void eistEnableEnabled(bool state);
    bool eistLockEnabled() const;
    void eistLockEnabled(bool state);
    bool tbtDisableEnabled() const;
    void tbtDisableEnabled(bool state);
    bool tbtActivationRatioEnabled() const;
    void tbtActivationRatioEnabled(bool state);
    bool tbtActivationRatioLockEnabled() const;
    void tbtActivationRatioLockEnabled(bool state);

    static constexpr const char* INI_EIST_ENABLE { "EIST_Enable" };
    static constexpr const char* INI_EIST_ENABLE_ENABLE { "EIST_Enable_Enabled" };
    static constexpr const char* INI_EIST_LOCK { "EIST_Lock" };
    static constexpr const char* INI_EIST_LOCK_ENABLE { "EIST_Lock_Enabled" };
    static constexpr const char* INI_TBT_DISABLE { "TBT_Disable" };
    static constexpr const char* INI_TBT_DISABLE_ENABLE { "TBT_Disable_Enabled" };
    static constexpr const char* INI_TURBO_ACTIVATION_RATIO { "TBT_Activation_Ratio" };
    static constexpr const char* INI_TURBO_ACTIVATION_RATIO_ENABLE { "TBT_Activation_Ratio_Enabled" };
    static constexpr const char* INI_TURBO_ACTIVATION_RATIO_LOCK { "TBT_Activation_Ratio_Lock" };
    static constexpr const char* INI_TURBO_ACTIVATION_RATIO_LOCK_ENABLE { "TBT_Activation_Ratio_Lock_Enabled" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_1C { "TBT_Ratio_Limit_1C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_2C { "TBT_Ratio_Limit_2C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_3C { "TBT_Ratio_Limit_3C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_4C { "TBT_Ratio_Limit_4C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_5C { "TBT_Ratio_Limit_5C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_6C { "TBT_Ratio_Limit_6C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_7C { "TBT_Ratio_Limit_7C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_8C { "TBT_Ratio_Limit_8C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_9C { "TBT_Ratio_Limit_9C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_10C { "TBT_Ratio_Limit_10C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_11C { "TBT_Ratio_Limit_11C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_12C { "TBT_Ratio_Limit_12C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_13C { "TBT_Ratio_Limit_13C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_14C { "TBT_Ratio_Limit_14C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_15C { "TBT_Ratio_Limit_15C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_16C { "TBT_Ratio_Limit_16C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_17C { "TBT_Ratio_Limit_17C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_18C { "TBT_Ratio_Limit_18C" };
    static constexpr const char* INI_TURBO_RATIO_LIMIT_ENABLE { "TBT_Ratio_Limit_Enabled" };

    bool enable_eist_enable_ { false };
    bool enable_eist_lock_ { false };
    bool enable_tbt_disable_ { false };
    bool enable_tbt_activation_ratio_ { false };
    bool enable_tbt_activation_ratio_lock_ { false };
};


inline bool SpeedControl::Values::eistEnable() const {
  return eist_enable_;
}

inline void SpeedControl::Values::eistEnable(bool v) {
  eist_enable_ = v;
}

inline bool SpeedControl::Values::eistLock() const {
  return eist_lock_;
}

inline void SpeedControl::Values::eistLock(bool v) {
  eist_lock_ = v;
}

inline bool SpeedControl::Values::tbtDisable() const {
  return tbt_disable_;
}

inline void SpeedControl::Values::tbtDisable(bool v) {
  tbt_disable_ = v;
}

inline uint8_t SpeedControl::Values::tbtActivationRatio() const {
  return tbt_activation_ratio_;
}

inline void SpeedControl::Values::tbtActivationRatio(uint8_t v) {
  tbt_activation_ratio_ = v;
}

inline bool SpeedControl::Values::tbtActivationRatioLock() const {
  return tbt_activation_ratio_lock_;
}

inline void SpeedControl::Values::tbtActivationRatioLock(bool v) {
  tbt_activation_ratio_lock_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit1C() const {
  return tbt_ratio_limit_1c_;
}

inline void SpeedControl::Values::tbtRatioLimit1C(uint8_t v) {
  tbt_ratio_limit_1c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit2C() const {
  return tbt_ratio_limit_2c_;
}

inline void SpeedControl::Values::tbtRatioLimit2C(uint8_t v) {
  tbt_ratio_limit_2c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit3C() const {
  return tbt_ratio_limit_3c_;
}

inline void SpeedControl::Values::tbtRatioLimit3C(uint8_t v) {
  tbt_ratio_limit_3c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit4C() const {
  return tbt_ratio_limit_4c_;
}

inline void SpeedControl::Values::tbtRatioLimit4C(uint8_t v) {
  tbt_ratio_limit_4c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit5C() const {
  return tbt_ratio_limit_5c_;
}

inline void SpeedControl::Values::tbtRatioLimit5C(uint8_t v) {
  tbt_ratio_limit_5c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit6C() const {
  return tbt_ratio_limit_6c_;
}

inline void SpeedControl::Values::tbtRatioLimit6C(uint8_t v) {
  tbt_ratio_limit_6c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit7C() const {
  return tbt_ratio_limit_7c_;
}

inline void SpeedControl::Values::tbtRatioLimit7C(uint8_t v) {
  tbt_ratio_limit_7c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit8C() const {
  return tbt_ratio_limit_8c_;
}

inline void SpeedControl::Values::tbtRatioLimit8C(uint8_t v) {
  tbt_ratio_limit_8c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit9C() const {
  return tbt_ratio_limit_9c_;
}

inline void SpeedControl::Values::tbtRatioLimit9C(uint8_t v) {
  tbt_ratio_limit_9c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit10C() const {
  return tbt_ratio_limit_10c_;
}

inline void SpeedControl::Values::tbtRatioLimit10C(uint8_t v) {
  tbt_ratio_limit_10c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit11C() const {
  return tbt_ratio_limit_11c_;
}

inline void SpeedControl::Values::tbtRatioLimit11C(uint8_t v) {
  tbt_ratio_limit_11c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit12C() const {
  return tbt_ratio_limit_12c_;
}

inline void SpeedControl::Values::tbtRatioLimit12C(uint8_t v) {
  tbt_ratio_limit_12c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit13C() const {
  return tbt_ratio_limit_13c_;
}

inline void SpeedControl::Values::tbtRatioLimit13C(uint8_t v) {
  tbt_ratio_limit_13c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit14C() const {
  return tbt_ratio_limit_14c_;
}

inline void SpeedControl::Values::tbtRatioLimit14C(uint8_t v) {
  tbt_ratio_limit_14c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit15C() const {
  return tbt_ratio_limit_15c_;
}

inline void SpeedControl::Values::tbtRatioLimit15C(uint8_t v) {
  tbt_ratio_limit_15c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit16C() const {
  return tbt_ratio_limit_16c_;
}

inline void SpeedControl::Values::tbtRatioLimit16C(uint8_t v) {
  tbt_ratio_limit_16c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit17C() const {
  return tbt_ratio_limit_17c_;
}

inline void SpeedControl::Values::tbtRatioLimit17C(uint8_t v) {
  tbt_ratio_limit_17c_ = v;
}

inline uint8_t SpeedControl::Values::tbtRatioLimit18C() const {
  return tbt_ratio_limit_18c_;
}

inline void SpeedControl::Values::tbtRatioLimit18C(uint8_t v) {
  tbt_ratio_limit_18c_ = v;
}

inline bool SpeedControl::Values::tbtRatioLimitEnable() const {
  return tbt_ratio_limit_enable_;
}

inline void SpeedControl::Values::tbtRatioLimitEnable(bool v) {
  tbt_ratio_limit_enable_ = v;
}


inline bool SpeedControl::Settings::eistEnableEnabled() const {
  return enable_eist_enable_;
}

inline void SpeedControl::Settings::eistEnableEnabled(bool v) {
  enable_eist_enable_ = v;
}

inline bool SpeedControl::Settings::eistLockEnabled() const {
  return enable_eist_lock_;
}

inline void SpeedControl::Settings::eistLockEnabled(bool v) {
  enable_eist_lock_ = v;
}

inline bool SpeedControl::Settings::tbtDisableEnabled() const {
  return enable_tbt_disable_;
}

inline void SpeedControl::Settings::tbtDisableEnabled(bool v) {
  enable_tbt_disable_ = v;
}

inline bool SpeedControl::Settings::tbtActivationRatioEnabled() const {
  return enable_tbt_activation_ratio_;
}

inline void SpeedControl::Settings::tbtActivationRatioEnabled(bool v) {
  enable_tbt_activation_ratio_ = v;
}

inline bool SpeedControl::Settings::tbtActivationRatioLockEnabled() const {
  return enable_tbt_activation_ratio_lock_;
}

inline void SpeedControl::Settings::tbtActivationRatioLockEnabled(bool v) {
  enable_tbt_activation_ratio_lock_ = v;
}

#endif

