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

#ifndef CoreAdjust_Grub
#define CoreAdjust_Grub

// STL
#include <climits>
#include <string>
#include <vector>
// Qt
#include <QCheckBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QLabel>
#include <QVBoxLayout>
// App
#include "SaferCombo.hpp"
#include "ShellCommand.hpp"
#include "TabMemberBase.hpp"

/*
 * See kernel sources: Documentation/admin-guide/kernel-parameters.txt
 */

class Grub final : public TabMemberTemplateAllCpus {
  Q_OBJECT
  public:

    explicit Grub(
      const CpuInfo& cpuInfo, const CpuId& cpuId,
      TabValues& tabValues, TabSettings& tabSettings,
      QWidget* parent = nullptr);

    ~Grub() override = default;

    void load() override;
    void store() override;
    void refresh() override;
    bool read(std::ostringstream& err) override;
    bool apply() override;
    bool compare() override;
    void timed(bool) override {}

  private:

    /* Each kernel-parameter has its own enum class describing the options
     * for that parameter.
     *
     * WARNING: The enum values must have the same index ordering
     *          as the combo boxes, the enum values are used as
     *          the (selected) index in the combo boxes... */

    enum class intel_pstate_t : int {
      Null = 0,                    /**< Empty, no flags set (default). */
      Disable = 1 << 0,            /**< Disable intel p-state driver  */
      Passive = 1 << 1,            /**< Disable intel p-state internal CPU governor  */
      Force = 1 << 2,              /**< Enable intel p-state on systems that prohibit it by default. */
      No_hwp = 1 << 3,             /**< Do not enable hardware P state control (HWP) if available. */
      Hwp_only = 1 << 4,           /**< Only load on systems which support HWP. */
      Support_acpi_ppc = 1 << 5,   /**< Enforce ACPI _PPC performance limits. */
      Per_cpu_perf_limits = 1 << 6 /**< Allow per-logical-CPU P-State performance control limits. */
    };

    friend bool operator&& (const intel_pstate_t lhs, const intel_pstate_t rhs);
    friend intel_pstate_t operator& (const intel_pstate_t& lhs, const intel_pstate_t& rhs);
    friend intel_pstate_t operator| (const intel_pstate_t& lhs, const intel_pstate_t& rhs);
    friend intel_pstate_t operator~ (const intel_pstate_t& rhs);

    enum class mitigations_t : int {
      Auto,      /**< Automaticly detect vulnerability mitigations. */
      Off,       /**< Do not enable any vulnerability mitigations. */
      AutoNosmt, /**< Automaticly detect vulnerability mitigations and disable SMT if needed. */
      Manual     /**< Manual selection. */
    };

    enum class spectre_v1_t : int {
      Auto, /**< Automaticly detect this vulnerability mitigation. */
      Off   /**< Do not enable this vulnerability mitigation. */
    };

    enum class spectre_v2_t : int {
      Auto,             /**< Automaticly detect this vulnerability mitigation. */
      Off,              /**< Do not enable this vulnerability mitigation. */
      On,               /**< Enable this vulnerability mitigation. */
      Retpoline,        /**< Use Googles retpoline mitigation. */
      RetpolineGeneric, /**< Use the generic retpoline mitigation. */
      RetpolineAmd      /**< Use AMDs retpoline mitigation. */
    };

    enum class spectre_v2_user_t : int {
      Auto,        /**< Automaticly detect this vulnerability mitigation. */
      Off,         /**< Do not enable this vulnerability mitigation. */
      On,          /**< Enable this vulnerability mitigation. */
      Prctl,
      PrctlIbpb,
      Seccomp,
      SeccompIbpb
    };

    enum class spec_store_bypass_disable_t : int {
      Auto,        /**< Automaticly detect this vulnerability mitigation. */
      Off,         /**< Do not enable this vulnerability mitigation. */
      On,          /**< Enable this vulnerability mitigation. */
      Prctl,
      Seccomp
    };

    enum class pti_t : int {
      Auto,        /**< Automaticly detect this vulnerability mitigation. */
      Off,         /**< Do not enable this vulnerability mitigation. */
      On,          /**< Enable this vulnerability mitigation. */
    };

    enum class mds_t : int {
      Full,     /**< Enable this vulnerability mitigation. */
      Off,      /**< Do not enable this vulnerability mitigation. */
      FullNosmt /**< Enable this vulnerability mitigation and disable SMT if needed. */
    };

    enum class tsx_async_abort_t : int {
      Full,     /**< Enable this vulnerability mitigation. */
      Off,      /**< Do not enable this vulnerability mitigation. */
      FullNosmt /**< Enable this vulnerability mitigation and disable SMT if needed. */
    };

    enum class l1tf_t : int {
      Flush, Off, Full, FullForce, FlushNosmt, FlushNowarn
    };

    enum class nx_huge_pages_t : int {
      Auto, /**< Automaticly detect this vulnerability mitigation. */
      Off,  /**< Do not enable this vulnerability mitigation. */
      Force /**< Enable this vulnerability mitigation. */
    };

    struct GrubData {
      intel_pstate_t intel_pstate_ { intel_pstate_t::Null };
      mitigations_t mitigations_ { mitigations_t::Manual };
      spectre_v1_t spectre_v1_ { spectre_v1_t::Auto };
      spectre_v2_t spectre_v2_ { spectre_v2_t::Auto };
      spectre_v2_user_t spectre_v2_user_ { spectre_v2_user_t::Auto };
      spec_store_bypass_disable_t spec_store_bypass_disable_ { spec_store_bypass_disable_t::Auto };
      pti_t pti_ { pti_t::Auto };
      mds_t mds_ { mds_t::Full };
      tsx_async_abort_t tsx_async_abort_ { tsx_async_abort_t::Full };
      l1tf_t l1tf_ { l1tf_t::Flush };
      nx_huge_pages_t nx_huge_pages_ { nx_huge_pages_t::Auto };
    };

    int parseGrubConfig();
    void store(GrubData&);
    std::string generateGrubConfig(GrubData& data);

    /* This works because our parent calls the compare() method after this
     * class emits a valueChanged signal. */
    bool doApply { false };

    long lineno_ { -1 };
    bool dialog_closed_ { false };
    std::vector<std::string> vsCfg_;
    std::vector<std::string> vsOther_;
    GrubData data_;
    GrubData backup_;
    ShellCommand shell_;

    QCheckBox* pstateDisable_;
    QCheckBox* pstatePassive_;
    QCheckBox* pstateForce_;
    QCheckBox* pstateNoHwp_;
    QCheckBox* pstateHwpOnly_;
    QCheckBox* pstateAcpiPpc_;
    QCheckBox* pstatePerCpu_;
    QLabel* v1Label_;
    QLabel* v2Label_;
    QLabel* v2usrLabel_;
    QLabel* specLabel_;
    QLabel* ptiLabel_;
    QLabel* mdsLabel_;
    QLabel* taaLabel_;
    QLabel* l1tfLabel_;
    QLabel* nxLabel_;
    SaferCombo* mitCombo_;
    SaferCombo* v1Combo_;
    SaferCombo* v2Combo_;
    SaferCombo* v2usrCombo_;
    SaferCombo* specCombo_;
    SaferCombo* ptiCombo_;
    SaferCombo* mdsCombo_;
    SaferCombo* taaCombo_;
    SaferCombo* l1tfCombo_;
    SaferCombo* nxCombo_;

  private slots:
    void pstateDisableChanged(int);
    void pstatePassiveChanged(int);
    void pstateForceChanged(int);
    void pstateNoHwpChanged(int);
    void pstateHwpOnlyChanged(int);
    void pstateAcpiPpcChanged(int);
    void pstatePerCpuChanged(int);
    void mitIndexChanged(int);
    void indexChanged(int);
};

#endif

