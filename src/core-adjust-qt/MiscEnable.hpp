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

#ifndef CoreAdjust_MiscEnable
#define CoreAdjust_MiscEnable

#include <QButtonGroup>
#include <QLabel>
#include <QRadioButton>

#include "ShellCommand.hpp"
#include "TabMemberBase.hpp"

class MiscEnable final : public TabMemberTemplate {
  Q_OBJECT
  public:
    class Values;
    class Settings;

    explicit MiscEnable(
        const SingleCpuInfo&, const SingleCpuId&, TabMemberValues&,
        TabMemberSettings&, QWidget* parent = nullptr);

    ~MiscEnable() override = default;

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

    QLabel* fs_enable_name_;
    QRadioButton* fs_enable_on_;
    QRadioButton* fs_enable_off_;
    QRadioButton* fs_enable_default_;
    QButtonGroup* fs_enable_grp_;
    QLabel* fs_enable_current_;

    QLabel* hwp_disable_name_;
    QRadioButton* hwp_disable_on_;
    QRadioButton* hwp_disable_off_;
    QRadioButton* hwp_disable_default_;
    QButtonGroup* hwp_disable_grp_;
    QLabel* hwp_disable_current_;

    QLabel* ferr_enable_name_;
    QRadioButton* ferr_enable_on_;
    QRadioButton* ferr_enable_off_;
    QRadioButton* ferr_enable_default_;
    QButtonGroup* ferr_enable_grp_;
    QLabel* ferr_enable_current_;

    QLabel* fsm_enable_name_;
    QRadioButton* fsm_enable_on_;
    QRadioButton* fsm_enable_off_;
    QRadioButton* fsm_enable_default_;
    QButtonGroup* fsm_enable_grp_;
    QLabel* fsm_enable_current_;

    QLabel* adj_disable_name_;
    QRadioButton* adj_disable_on_;
    QRadioButton* adj_disable_off_;
    QRadioButton* adj_disable_default_;
    QButtonGroup* adj_disable_grp_;
    QLabel* adj_disable_current_;

    QLabel* cpuid_enable_name_;
    QRadioButton* cpuid_enable_on_;
    QRadioButton* cpuid_enable_off_;
    QRadioButton* cpuid_enable_default_;
    QButtonGroup* cpuid_enable_grp_;
    QLabel* cpuid_enable_current_;

    QLabel* xtpr_disable_name_;
    QRadioButton* xtpr_disable_on_;
    QRadioButton* xtpr_disable_off_;
    QRadioButton* xtpr_disable_default_;
    QButtonGroup* xtpr_disable_grp_;
    QLabel* xtpr_disable_current_;

    QLabel* xd_disable_name_;
    QRadioButton* xd_disable_on_;
    QRadioButton* xd_disable_off_;
    QRadioButton* xd_disable_default_;
    QButtonGroup* xd_disable_grp_;
    QLabel* xd_disable_current_;

    QLabel* dcu_disable_name_;
    QRadioButton* dcu_disable_on_;
    QRadioButton* dcu_disable_off_;
    QRadioButton* dcu_disable_default_;
    QButtonGroup* dcu_disable_grp_;
    QLabel* dcu_disable_current_;

    QLabel* ipp_disable_name_;
    QRadioButton* ipp_disable_on_;
    QRadioButton* ipp_disable_off_;
    QRadioButton* ipp_disable_default_;
    QButtonGroup* ipp_disable_grp_;
    QLabel* ipp_disable_current_;

  private slots:
    void toggledSlot(int id, bool checked);
};

class MiscEnable::Values {
  public:
    bool fastStringsEnable() const;
    void fastStringsEnable(bool state);
    bool hwPrefetcherDisable() const;
    void hwPrefetcherDisable(bool state);
    bool ferrMultiplexingEnable() const;
    void ferrMultiplexingEnable(bool state);
    bool fsmMonitorEnable() const;
    void fsmMonitorEnable(bool state);
    bool adjCacheLinePrefetchDisable() const;
    void adjCacheLinePrefetchDisable(bool state);
    bool cpuidMaxval() const;
    void cpuidMaxval(bool state);
    bool xtprMessageDisable() const;
    void xtprMessageDisable(bool state);
    bool xdBitDisable() const;
    void xdBitDisable(bool state);
    bool dcuPrefetcherDisable() const;
    void dcuPrefetcherDisable(bool state);
    bool ipPrefetcherDisable() const;
    void ipPrefetcherDisable(bool state);
  private:
    bool fs_enable_ { true };       /* Fast_Strings_Enable */
    bool hwp_disable_ { false };    /* Hardware_Prefetcher_Disable */
    bool fmpx_enable_ { false };    /* FERR_Multiplexing_Enable */
    bool fsm_enable_ { true };      /* ENABLE_MONITOR_FSM */
    bool aclp_disable_ { false };   /* Adjacent_Cache_Line_Prefetch_Disable */
    bool cpuid_max_ { false };      /* Limit_CPUID_Maxval */
    bool xtprm_disable_ { true };   /* xTPR_Message_Disable */
    bool xdbit_disable_ { false };  /* XD_Bit_Disable */
    bool dcup_disable_ { false };   /* DCU_Prefetcher_Disable */
    bool ipp_disable_ { false };    /* IP_Prefetcher_Disable */
};

class MiscEnable::Settings
  : private MiscEnable::Values,
    virtual protected TabMemberWidget::Settings {

  friend class MiscEnable;

  public:
    Settings() = default;
    ~Settings() override = default;

  protected:
    void load(QSettings&, const TabMemberValues&) override;
    void save(QSettings&) override;

  private:
    bool fastStringsEnableEnabled() const;
    void fastStringsEnableEnabled(bool state);
    bool hwPrefetcherDisableEnabled() const;
    void hwPrefetcherDisableEnabled(bool state);
    bool ferrMultiplexingEnableEnabled() const;
    void ferrMultiplexingEnableEnabled(bool state);
    bool fsmMonitorEnableEnabled() const;
    void fsmMonitorEnableEnabled(bool state);
    bool adjCacheLinePrefetchDisableEnabled() const;
    void adjCacheLinePrefetchDisableEnabled(bool state);
    bool cpuidMaxvalEnabled() const;
    void cpuidMaxvalEnabled(bool state);
    bool xtprMessageDisableEnabled() const;
    void xtprMessageDisableEnabled(bool state);
    bool xdBitDisableEnabled() const;
    void xdBitDisableEnabled(bool state);
    bool dcuPrefetcherDisableEnabled() const;
    void dcuPrefetcherDisableEnabled(bool state);
    bool ipPrefetcherDisableEnabled() const;
    void ipPrefetcherDisableEnabled(bool state);

    static constexpr const char* INI_FS_ENABLE { "Fast_Strings_Enable" };
    static constexpr const char* INI_FS_ENABLE_ENABLE { "Fast_Strings_Enable_Enabled" };
    static constexpr const char* INI_HWP_DISABLE { "Hardware_Prefetcher_Disable" };
    static constexpr const char* INI_HWP_DISABLE_ENABLE { "Hardware_Prefetcher_Disable_Enabled" };
    static constexpr const char* INI_FMPX_ENABLE { "FERR_Multiplexing_Enable" };
    static constexpr const char* INI_FMPX_ENABLE_ENABLE { "FERR_Multiplexing_Enable_Enabled" };
    static constexpr const char* INI_FSM_ENABLE { "ENABLE_MONITOR_FSM" };
    static constexpr const char* INI_FSM_ENABLE_ENABLE { "ENABLE_MONITOR_FSM_Enabled" };
    static constexpr const char* INI_ACLP_DISABLE { "Adjacent_Cache_Line_Prefetch_Disable" };
    static constexpr const char* INI_ACLP_DISABLE_ENABLE { "Adjacent_Cache_Line_Prefetch_Disable_Enabled" };
    static constexpr const char* INI_CPUID_MAXVAL { "Limit_CPUID_Maxval" };
    static constexpr const char* INI_CPUID_MAXVAL_ENABLE { "Limit_CPUID_Maxval_Enabled" };
    static constexpr const char* INI_XTPRM_DISABLE { "xTPR_Message_Disable" };
    static constexpr const char* INI_XTPRM_DISABLE_ENABLE { "xTPR_Message_Disable_Enabled" };
    static constexpr const char* INI_XDBIT_DISABLE { "XD_Bit_Disable" };
    static constexpr const char* INI_XDBIT_DISABLE_ENABLE { "XD_Bit_Disable_Enabled" };
    static constexpr const char* INI_DCUP_DISABLE { "DCU_Prefetcher_Disable" };
    static constexpr const char* INI_DCUP_DISABLE_ENABLE { "DCU_Prefetcher_Disable_Enabled" };
    static constexpr const char* INI_IPP_DISABLE { "IP_Prefetcher_Disable" };
    static constexpr const char* INI_IPP_DISABLE_ENABLE { "IP_Prefetcher_Disable_Enabled" };

    bool enable_fs_enable_ { false };
    bool enable_hwp_disable_ { false };
    bool enable_fmpx_enable_ { false };
    bool enable_fsm_enable_ { false };
    bool enable_aclp_disable_ { false };
    bool enable_cpuid_max_ { false };
    bool enable_xtprm_disable_ { false };
    bool enable_xdbit_disable_ { false };
    bool enable_dcup_disable_ { false };
    bool enable_ipp_disable_ { false };
};


inline bool MiscEnable::Values::fastStringsEnable() const {
  return fs_enable_;
}

inline void MiscEnable::Values::fastStringsEnable(bool state) {
  fs_enable_ = state;
}

inline bool MiscEnable::Values::hwPrefetcherDisable() const {
  return hwp_disable_;
}

inline void MiscEnable::Values::hwPrefetcherDisable(bool state) {
  hwp_disable_ = state;
}

inline bool MiscEnable::Values::ferrMultiplexingEnable() const {
  return fmpx_enable_;
}

inline void MiscEnable::Values::ferrMultiplexingEnable(bool state) {
  fmpx_enable_ = state;
}

inline bool MiscEnable::Values::fsmMonitorEnable() const {
  return fsm_enable_;
}

inline void MiscEnable::Values::fsmMonitorEnable(bool state) {
  fsm_enable_ = state;
}

inline bool MiscEnable::Values::adjCacheLinePrefetchDisable() const {
  return aclp_disable_;
}

inline void MiscEnable::Values::adjCacheLinePrefetchDisable(bool state) {
  aclp_disable_ = state;
}

inline bool MiscEnable::Values::cpuidMaxval() const {
  return cpuid_max_;
}

inline void MiscEnable::Values::cpuidMaxval(bool state) {
  cpuid_max_ = state;
}

inline bool MiscEnable::Values::xtprMessageDisable() const {
  return xtprm_disable_;
}

inline void MiscEnable::Values::xtprMessageDisable(bool state) {
  xtprm_disable_ = state;
}

inline bool MiscEnable::Values::xdBitDisable() const {
  return xdbit_disable_;
}

inline void MiscEnable::Values::xdBitDisable(bool state) {
  xdbit_disable_ = state;
}

inline bool MiscEnable::Values::dcuPrefetcherDisable() const {
  return dcup_disable_;
}

inline void MiscEnable::Values::dcuPrefetcherDisable(bool state) {
  dcup_disable_ = state;
}

inline bool MiscEnable::Values::ipPrefetcherDisable() const {
  return ipp_disable_;
}

inline void MiscEnable::Values::ipPrefetcherDisable(bool state) {
  ipp_disable_ = state;
}


inline bool MiscEnable::Settings::fastStringsEnableEnabled() const {
  return enable_fs_enable_;
}

inline void MiscEnable::Settings::fastStringsEnableEnabled(bool state) {
  enable_fs_enable_ = state;
}

inline bool MiscEnable::Settings::hwPrefetcherDisableEnabled() const {
  return enable_hwp_disable_;
}

inline void MiscEnable::Settings::hwPrefetcherDisableEnabled(bool state) {
  enable_hwp_disable_ = state;
}

inline bool MiscEnable::Settings::ferrMultiplexingEnableEnabled() const {
  return enable_fmpx_enable_;
}

inline void MiscEnable::Settings::ferrMultiplexingEnableEnabled(bool state) {
  enable_fmpx_enable_ = state;
}

inline bool MiscEnable::Settings::fsmMonitorEnableEnabled() const {
  return enable_fsm_enable_;
}

inline void MiscEnable::Settings::fsmMonitorEnableEnabled(bool state) {
  enable_fsm_enable_ = state;
}

inline bool MiscEnable::Settings::adjCacheLinePrefetchDisableEnabled() const {
  return enable_aclp_disable_;
}

inline void MiscEnable::Settings::adjCacheLinePrefetchDisableEnabled(bool state) {
  enable_aclp_disable_ = state;
}

inline bool MiscEnable::Settings::cpuidMaxvalEnabled() const {
  return enable_cpuid_max_;
}

inline void MiscEnable::Settings::cpuidMaxvalEnabled(bool state) {
  enable_cpuid_max_ = state;
}

inline bool MiscEnable::Settings::xtprMessageDisableEnabled() const {
  return enable_xtprm_disable_;
}

inline void MiscEnable::Settings::xtprMessageDisableEnabled(bool state) {
  enable_xtprm_disable_ = state;
}

inline bool MiscEnable::Settings::xdBitDisableEnabled() const {
  return enable_xdbit_disable_;
}

inline void MiscEnable::Settings::xdBitDisableEnabled(bool state) {
  enable_xdbit_disable_ = state;
}

inline bool MiscEnable::Settings::dcuPrefetcherDisableEnabled() const {
  return enable_dcup_disable_;
}

inline void MiscEnable::Settings::dcuPrefetcherDisableEnabled(bool state) {
  enable_dcup_disable_ = state;
}

inline bool MiscEnable::Settings::ipPrefetcherDisableEnabled() const {
  return enable_ipp_disable_;
}

inline void MiscEnable::Settings::ipPrefetcherDisableEnabled(bool state) {
  enable_ipp_disable_ = state;
}

#endif

