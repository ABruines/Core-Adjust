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

#ifndef CoreAdjust_MsrReadout
#define CoreAdjust_MsrReadout

#include <sstream>
#include <vector>
#include <QGridLayout>
#include <QScrollArea>
#include <QTabWidget>
#include <QLabel>
#include "Msr.hpp"
#include "TabMemberBase.hpp"

/**
  * @file src/core-adjust-qt/MsrReadout.hpp
  * @brief A TabMemberWidget for displaying various MSR registers.
  *
  * @file src/core-adjust-qt/MsrReadout.cpp
  * @brief A TabMemberWidget for displaying various MSR registers (implementation).
  *
  * @class MsrReadout
  * @brief A TabMemberWidget for displaying various MSR registers.
  *
  * @struct MsrReadout::MsrWidget::Description
  * @brief Description of a single MsrWidget that shall be displayed.
  *
  * @var MsrReadout::MsrWidget::Description::name;
  * @brief Name of the widget.
  *
  * @var MsrReadout::MsrWidget::Description::construct;
  * @brief Pointer to function that creates the widget.
  *
  * @var static const std::vector<MsrReadout::MsrWidget::Description> MsrReadout::MsrWidget::Factory;
  * @brief List of all the MsrWidget added to the MsrReadout widget.
  */
class MsrReadout : public TabMemberTemplate {
  Q_OBJECT
  public:
    class MsrWidget;
    class MsrPlatformId;
    class MsrPlatformInfo;
    class MsrPkgCstConfigControl;
    class Ia32ClockModulation;
    class MsrTherm2Ctl;
    class Ia32MiscEnable;
    class MsrTemperatureTarget;
    class MsrTurboRatioLimit;
    class MsrEnergyPerfBias;
    class MsrTurboActivationRatio;

    explicit MsrReadout(
        const SingleCpuInfo&, const SingleCpuId&,
        TabMemberValues&, TabMemberSettings&,
        QWidget *parent = nullptr);

    ~MsrReadout() override = default;

    void load() override {}
    void store() override {}
    void refresh() override;
    bool read(std::ostringstream&) override { return true; }
    bool apply() override { return true; }
    bool compare() override { return false; }
    void timed(bool) override {}

  private:
    QTabWidget* tabs_;
};

/** @class MsrReadout::MsrWidget
  * @brief Base class for MSR widgets displayed on the MsrReadout TabMemberWidget.
  */
class MsrReadout::MsrWidget : public QWidget {
  Q_OBJECT

  protected:
    const SingleCpuInfo& cpuInfo_;
    const SingleCpuId& cpuId_;
    QGridLayout* layout_;
    QScrollArea* scroll_area_;
    QWidget* scroll_widget_;

    explicit MsrWidget(
      const SingleCpuInfo& cpuInfo,
      const SingleCpuId& cpuId,
      QWidget* parent = nullptr);

  public:
    MsrWidget() = delete;
    ~MsrWidget() override = default;

    struct Description {
      const char* name;
      MsrWidget* (*construct)(const SingleCpuInfo&, const SingleCpuId&, QWidget*);
    };

    template<typename T>
    static MsrWidget* Construct(
        const SingleCpuInfo& cpu_info,
        const SingleCpuId& cpu_id,
        QWidget* parent = nullptr) {
      return new T(cpu_info, cpu_id, parent);
    }

    static const std::vector<Description> Factory;

    QSize	sizeHint() const override;

    virtual void refresh() = 0;
};

/** @brief Widget that implements an MsrReadout::MsrWidget and displays an MSR_PLATFORM_ID instance. */
class MsrReadout::MsrPlatformId : public MsrReadout::MsrWidget {
  Q_OBJECT
  public:
    MsrPlatformId() = delete;
    explicit MsrPlatformId(
        const SingleCpuInfo&,
        const SingleCpuId&,
        QWidget *parent = nullptr);
    ~MsrPlatformId() override = default;
    void refresh() override;
  private:
    MSR_PLATFORM_ID msr_;
    QLabel* value_;
    QLabel* read_success_;
    QLabel* max_ratio_;
    QLabel* platform_id_;
};

/** @brief Widget that implements an MsrReadout::MsrWidget and displays an MSR_PLATFORM_INFO instance. */
class MsrReadout::MsrPlatformInfo : public MsrReadout::MsrWidget {
  Q_OBJECT
  public:
    MsrPlatformInfo() = delete;
    explicit MsrPlatformInfo(
        const SingleCpuInfo&,
        const SingleCpuId&,
        QWidget *parent = nullptr);
    ~MsrPlatformInfo() override = default;
    void refresh() override;
  private:
    MSR_PLATFORM_INFO msr_;
    QLabel* value_;
    QLabel* read_success_;
    QLabel* max_non_turbo_;
    QLabel* ppin_cap_;
    QLabel* has_ratio_limit_;
    QLabel* has_tdp_limit_;
    QLabel* has_tj_offset_;
    QLabel* has_lpm_support_;
    QLabel* nr_tdp_levels_;
    QLabel* max_eff_ratio_;
    QLabel* min_ratio_;
};

/** @brief Widget that implements an MsrReadout::MsrWidget and displays an MSR_PKG_CST_CONFIG_CONTROL instance. */
class MsrReadout::MsrPkgCstConfigControl : public MsrReadout::MsrWidget {
  Q_OBJECT
  public:
    MsrPkgCstConfigControl() = delete;
    explicit MsrPkgCstConfigControl(
        const SingleCpuInfo&,
        const SingleCpuId&,
        QWidget *parent = nullptr);
    ~MsrPkgCstConfigControl() override = default;
    void refresh() override;
  private:
    MSR_PKG_CST_CONFIG_CONTROL msr_;
    QLabel* value_;
    QLabel* read_success_;
    QLabel* c_state_limit_;
    QLabel* mwait_redirection_;
    QLabel* cfg_lock_;
    QLabel* c3_auto_demote_;
    QLabel* c1_auto_demote_;
    QLabel* enable_c3_undemote_;
    QLabel* enable_c1_undemote_;
};

/** @brief Widget that implements an MsrReadout::MsrWidget and displays an IA32_CLOCK_MODULATION instance. */
class MsrReadout::Ia32ClockModulation : public MsrReadout::MsrWidget {
  Q_OBJECT
  public:
    Ia32ClockModulation() = delete;
    explicit Ia32ClockModulation(
        const SingleCpuInfo&,
        const SingleCpuId&,
        QWidget *parent = nullptr);
    ~Ia32ClockModulation() override = default;
    void refresh() override;
  private:
    IA32_CLOCK_MODULATION msr_;
    QLabel* value_;
    QLabel* read_success_;
    QLabel* extended_;
    QLabel* dutycycle_;
    QLabel* enabled_;
};

/** @brief Widget that implements an MsrReadout::MsrWidget and displays an MSR_THERM2_CTL instance. */
class MsrReadout::MsrTherm2Ctl : public MsrReadout::MsrWidget {
  Q_OBJECT
  public:
    MsrTherm2Ctl() = delete;
    explicit MsrTherm2Ctl(
        const SingleCpuInfo&,
        const SingleCpuId&,
        QWidget *parent = nullptr);
    ~MsrTherm2Ctl() override = default;
    void refresh() override;
  private:
    MSR_THERM2_CTL msr_;
    QLabel* value_;
    QLabel* read_success_;
    QLabel* tm2_transition_target_;
    QLabel* tm_select_;
};

/** @brief Widget that implements an MsrReadout::MsrWidget and displays an IA32_MISC_ENABLE instance. */
class MsrReadout::Ia32MiscEnable : public MsrReadout::MsrWidget {
  Q_OBJECT
  public:
    Ia32MiscEnable() = delete;
    explicit Ia32MiscEnable(
        const SingleCpuInfo&,
        const SingleCpuId&,
        QWidget *parent = nullptr);
    ~Ia32MiscEnable() override = default;
    void refresh() override;
  private:
    IA32_MISC_ENABLE msr_;
    QLabel* value_;
    QLabel* read_success_;
    QLabel* fast_strings_enable_;
    QLabel* tcc_enable_;
    QLabel* perfmon_available_;
    QLabel* prefetch_disable_;
    QLabel* ferr_mux_enable_;
    QLabel* bts_unavailable_;
    QLabel* ev_based_sampling_;
    QLabel* tm2_enable_;
    QLabel* eist_enable_;
    QLabel* fsm_enable_;
    QLabel* cache_prefetch_disable_;
    QLabel* eist_select_lock_;
    QLabel* cpuid_limit_;
    QLabel* xptr_msg_disable_;
    QLabel* xd_bit_disable_;
    QLabel* dcu_prefetch_disable_;
    QLabel* ida_disable_;
    QLabel* ip_prefetch_disable_;
};

/** @brief Widget that implements an MsrReadout::MsrWidget and displays an MSR_TEMPERATURE_TARGET instance. */
class MsrReadout::MsrTemperatureTarget : public MsrReadout::MsrWidget {
  Q_OBJECT
  public:
    MsrTemperatureTarget() = delete;
    explicit MsrTemperatureTarget(
        const SingleCpuInfo&,
        const SingleCpuId&,
        QWidget *parent = nullptr);
    ~MsrTemperatureTarget() override = default;
    void refresh() override;
  private:
    MSR_TEMPERATURE_TARGET msr_;
    QLabel* value_;
    QLabel* read_success_;
    QLabel* temperature_target_;
    QLabel* target_offset_;
};

/** @brief Widget that implements an MsrReadout::MsrWidget and displays an MSR_TURBO_RATIO_LIMIT instance. */
class MsrReadout::MsrTurboRatioLimit : public MsrReadout::MsrWidget {
  Q_OBJECT
  public:
    MsrTurboRatioLimit() = delete;
    explicit MsrTurboRatioLimit(
        const SingleCpuInfo&,
        const SingleCpuId&,
        QWidget *parent = nullptr);
    ~MsrTurboRatioLimit() override = default;
    void refresh() override;
  private:
    MSR_TURBO_RATIO_LIMIT msr_;
    QLabel* value_;
    QLabel* read_success_;
    QLabel* ratio_limit_1c_;
    QLabel* ratio_limit_2c_;
    QLabel* ratio_limit_3c_;
    QLabel* ratio_limit_4c_;
    QLabel* ratio_limit_5c_;
    QLabel* ratio_limit_6c_;
    QLabel* ratio_limit_7c_;
    QLabel* ratio_limit_8c_;

    MSR_TURBO_RATIO_LIMIT1 msr1_;
    QLabel* value1_;
    QLabel* read_success1_;
    QLabel* ratio_limit_9c_;
    QLabel* ratio_limit_10c_;
    QLabel* ratio_limit_11c_;
    QLabel* ratio_limit_12c_;
    QLabel* ratio_limit_13c_;
    QLabel* ratio_limit_14c_;
    QLabel* ratio_limit_15c_;
    QLabel* ratio_limit_16c_;

    MSR_TURBO_RATIO_LIMIT2 msr2_;
    QLabel* value2_;
    QLabel* read_success2_;
    QLabel* ratio_limit_17c_;
    QLabel* ratio_limit_18c_;
    QLabel* ratio_sema2_;

    MSR_TURBO_RATIO_LIMIT3 msr3_;
    QLabel* value3_;
    QLabel* read_success3_;
    QLabel* ratio_sema3_;
};

/** @brief Widget that implements an MsrReadout::MsrWidget and displays an IA32_ENERGY_PERF_BIAS instance. */
class MsrReadout::MsrEnergyPerfBias : public MsrReadout::MsrWidget {
  Q_OBJECT
  public:
    MsrEnergyPerfBias() = delete;
    explicit MsrEnergyPerfBias(
        const SingleCpuInfo&,
        const SingleCpuId&,
        QWidget *parent = nullptr);
    ~MsrEnergyPerfBias() override = default;
    void refresh() override;
  private:
    IA32_ENERGY_PERF_BIAS msr_;
    QLabel* value_;
    QLabel* read_success_;
    QLabel* power_policy_preference_;
};

/** @brief Widget that implements an MsrReadout::MsrWidget and displays an MSR_TURBO_ACTIVATION_RATIO instance. */
class MsrReadout::MsrTurboActivationRatio : public MsrReadout::MsrWidget {
  Q_OBJECT
  public:
    MsrTurboActivationRatio() = delete;
    explicit MsrTurboActivationRatio(
        const SingleCpuInfo&,
        const SingleCpuId&,
        QWidget *parent = nullptr);
    ~MsrTurboActivationRatio() override = default;
    void refresh() override;
  private:
    MSR_TURBO_ACTIVATION_RATIO msr_;
    QLabel* value_;
    QLabel* read_success_;
    QLabel* max_non_turbo_ratio_;
    QLabel* max_non_turbo_lock_;
};

#endif

