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
  * @file src/core-adjust-qt/Grub.hpp
  * @brief A TabMemberWidget implementation to adjust bootloader (GNU Grub) settings.
  *
  * @file src/core-adjust-qt/Grub.cpp
  * @brief A TabMemberWidget implementation to adjust bootloader (GNU Grub) settings (implementation).
  *
  * @class Grub
  * @brief A TabMemberWidget implementation to change bootloader (GNU Grub) settings.
  *
  * This class allows to change some processor related kernel-options to be
  * enabled or disabled when the system boots using the Grub bootloader.
  *
  * This class does not save its settings to the INI file but writes directly
  * to /etc/default/grub
  *
  * @note
  * Any 'leaked on vgs' messages while running update-grub come from the LVM2 library.
  * These messages are overly verbose and can be safely ignored.
  * They only occur when using root/boot on LVM.
  *
  * @see See kernel sources: Documentation/admin-guide/kernel-parameters.txt
  */
// STL
#include <fstream>
#include <type_traits>
// Qt
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
// App
#include "Dbg.hpp"
#include "Grub.hpp"
#include "Shell.hpp"
#include "Strings.hpp"
#include "TabMember.hpp"

namespace {
  template <typename E>
  inline constexpr typename std::underlying_type<E>::type
  enum_cast(E e) noexcept {
      return static_cast<typename std::underlying_type<E>::type>(e);
  }
}

/** @brief Logical AND operator for intel_pstate_t */
inline bool operator&& (
    const Grub::intel_pstate_t lhs, const Grub::intel_pstate_t rhs) {
  return enum_cast(lhs) & enum_cast(rhs);
}

/** @brief Bitwise AND operator for intel_pstate_t */
inline Grub::intel_pstate_t operator& (
    const Grub::intel_pstate_t& lhs, const Grub::intel_pstate_t& rhs) {
  return static_cast<Grub::intel_pstate_t>(enum_cast(lhs) & enum_cast(rhs));
}

/** @brief Bitwise OR operator for intel_pstate_t */
inline Grub::intel_pstate_t operator| (
    const Grub::intel_pstate_t& lhs, const Grub::intel_pstate_t& rhs) {
  return static_cast<Grub::intel_pstate_t>(enum_cast(lhs) | enum_cast(rhs));
}

/** @brief Returns the ones' complement of a intel_pstate_t */
inline Grub::intel_pstate_t operator~ (const Grub::intel_pstate_t& rhs) {
  return static_cast<Grub::intel_pstate_t>(~enum_cast(rhs));
}

/**
  * @param c
  * Reference to the global CpuInfo instance.
  * @param i
  * Reference to the global CpuId instance.
  * @param v
  * Reference to the TabMemberValues instance.
  * @param s
  * Reference to the TabMemberSettings instance.
  * @param p
  * Pointer to the parent widget.
  */
Grub::Grub(
    const CpuInfo& c, const CpuId& i,
    TabValues& v, TabSettings& s, QWidget* p)
    : TabMemberTemplateAllCpus(c, i, v, s, p),
      shell_(this, "Running update-grub.",
          ShellCommand::EnableStd | ShellCommand::DisableAbortButton |
          ShellCommand::DisableColor) {

  int status = parseGrubConfig();

  /* WARNING
   * The items in the combo boxes must have the same
   * index as the values in the enum class it belongs to. */

  pstateDisable_ = new QCheckBox("disable");
  pstatePassive_ = new QCheckBox("passive");
  pstateForce_ = new QCheckBox("force");
  pstateNoHwp_ = new QCheckBox("no_hwp");
  pstateHwpOnly_ = new QCheckBox("hwp_only");
  pstateAcpiPpc_ = new QCheckBox("support_acpi_ppc");
  pstatePerCpu_ = new QCheckBox("per_cpu_perf_limits");
  auto* pstateLayout = new QGridLayout();
  pstateLayout->addWidget(pstateDisable_, 0, 0);
  pstateLayout->addWidget(pstatePassive_, 0, 1);
  pstateLayout->addWidget(pstateForce_, 0, 2);
  pstateLayout->addWidget(pstateNoHwp_, 1, 0);
  pstateLayout->addWidget(pstateHwpOnly_, 1, 1);
  pstateLayout->addWidget(pstateAcpiPpc_, 1, 2);
  pstateLayout->addWidget(pstatePerCpu_, 2, 0);
  auto* pstateBox = new QGroupBox("Intel P-State Frequency Scaling");
  pstateBox->setFlat(true);
  pstateBox->setLayout(pstateLayout);
  pstateBox->setToolTip("Value for kernel parameter 'intel_pstate='");
  pstateDisable_->setToolTip(
    "<p>Do not enable intel_pstate as the default "
    "frequency scaling driver.</p>"
    "<p>This will enable the ACPI frequency scaling driver if available.</p>");
  pstatePassive_->setToolTip(
    "<p>Use intel_pstate as a scaling driver, but configure it "
    "to work with generic cpufreq governors (instead of "
    "enabling its internal governor).</p><p>This mode cannot be "
    "used along with the hardware-managed P-states (HWP) "
    "feature.</p>");
  pstateForce_->setToolTip(
    "<p>Enable intel_pstate on systems that prohibit it by default "
    "in favor of acpi-cpufreq.</p><p>Forcing the intel_pstate driver "
    "instead of acpi-cpufreq may disable platform features, such "
    "as thermal controls and power capping, that rely on ACPI "
    "P-States information being indicated to OSPM and therefore "
    "should be used with caution.</p><p>This option does not work with "
    "processors that aren't supported by the intel_pstate driver "
    "or on platforms that use pcc-cpufreq instead of acpi-cpufreq.</p>");
  pstateNoHwp_->setToolTip(
    "<p>Do not enable hardware P state control (HWP) if available.</p>");
  pstateHwpOnly_->setToolTip(
    "<p>Only load intel_pstate on systems which support "
    "hardware P state control (HWP) if available.</p>");
  pstateAcpiPpc_->setToolTip(
    "<p>Enforce ACPI _PPC performance limits.</p><p>If the Fixed ACPI "
    "Description Table, specifies preferred power management "
    "profile as \"Enterprise Server\" or \"Performance Server\", "
    "then this feature is turned on by default.</p>");
  pstatePerCpu_->setToolTip(
    "<p>Allow per-logical-CPU P-State performance control limits using "
    "cpufreq sysfs interface.</p>");

  v1Label_ = new QLabel("Spectre V1 mitigation");
  v1Combo_ = new SaferCombo();
  v1Combo_->addItems({ "auto", "off" });
  auto* v1Layout = new QVBoxLayout();
  v1Layout->setContentsMargins(0, 0, 0, 0);
  v1Layout->addWidget(v1Label_);
  v1Layout->addWidget(v1Combo_);
  v1Combo_->setToolTip("Value for kernel parameter 'nospectre_v1'");

  v2Label_ = new QLabel("Spectre V2 mitigation");
  v2Combo_ = new SaferCombo();
  v2Combo_->addItems({
      "auto", "off", "on",
      "retpoline", "retpoline_generic)", "retpoline_amd" });
  auto* v2Layout = new QVBoxLayout();
  v2Layout->setContentsMargins(0, 0, 0, 0);
  v2Layout->addWidget(v2Label_);
  v2Layout->addWidget(v2Combo_);
  v2Combo_->setToolTip("Value for kernel parameter 'spectre_v2='");

  v2usrLabel_ = new QLabel("Spectre V2 (User) mitigation");
  v2usrCombo_ = new SaferCombo();
  v2usrCombo_->addItems({
      "auto", "off", "on", "prctl", "prctl,ibpb",
      "seccomp", "seccomp,ibpb" });
  auto* v2usrLayout = new QVBoxLayout();
  v2usrLayout->setContentsMargins(0, 0, 0, 0);
  v2usrLayout->addWidget(v2usrLabel_);
  v2usrLayout->addWidget(v2usrCombo_);
  v2usrCombo_->setToolTip("Value for kernel parameter 'spectre_v2_user='");

  auto* v2Combined = new QHBoxLayout();
  v2Combined->setContentsMargins(0, 0, 0, 0);
  v2Combined->addLayout(v2Layout);
  v2Combined->addLayout(v2usrLayout);

  specLabel_ = new QLabel("Speculative Store Bypass mitigation");
  specCombo_ = new SaferCombo();
  specCombo_->addItems({ "auto", "off", "on", "prctl", "seccomp" });
  auto* specLayout = new QVBoxLayout();
  specLayout->setContentsMargins(0, 0, 0, 0);
  specLayout->addWidget(specLabel_);
  specLayout->addWidget(specCombo_);
  specCombo_->setToolTip("Value for kernel parameter 'spec_store_bypass_disable='");

  ptiLabel_ = new QLabel("Page Table Isolation");
  ptiCombo_ = new SaferCombo();
  ptiCombo_->addItems({ "auto", "off", "on" });
  auto* ptiLayout = new QVBoxLayout();
  ptiLayout->setContentsMargins(0, 0, 0, 0);
  ptiLayout->addWidget(ptiLabel_);
  ptiLayout->addWidget(ptiCombo_);
  ptiCombo_->setToolTip("Value for kernel parameter 'pti='");

  mdsLabel_ = new QLabel("Micro-architectural Data Sampling (MDS) vulnerability mitigation");
  mdsCombo_ = new SaferCombo();
  mdsCombo_->addItems({ "full", "off", "full,nosmt" });
  auto* mdsLayout = new QVBoxLayout();
  mdsLayout->setContentsMargins(0, 0, 0, 0);
  mdsLayout->addWidget(mdsLabel_);
  mdsLayout->addWidget(mdsCombo_);
  mdsCombo_->setToolTip("Value for kernel parameter 'mds='");

  taaLabel_ = new QLabel("TSX Async Abort (TAA) vulnerability mitigation");
  taaCombo_ = new SaferCombo();
  taaCombo_->addItems({ "full", "off", "full,nosmt" });
  auto* taaLayout = new QVBoxLayout();
  taaLayout->setContentsMargins(0, 0, 0, 0);
  taaLayout->addWidget(taaLabel_);
  taaLayout->addWidget(taaCombo_);
  taaCombo_->setToolTip("Value for kernel parameter 'tsx_async_abort='");

  l1tfLabel_ = new QLabel("L1TF vulnerability mitigation");
  l1tfCombo_ = new SaferCombo();
  l1tfCombo_->addItems({ "flush", "off", "full", "full,force",
      "flush,nosmt", "flush,nowarn" });
  auto* l1tfLayout = new QVBoxLayout();
  l1tfLayout->setContentsMargins(0, 0, 0, 0);
  l1tfLayout->addWidget(l1tfLabel_);
  l1tfLayout->addWidget(l1tfCombo_);
  l1tfCombo_->setToolTip("Value for kernel parameter 'l1tf='");

  nxLabel_ = new QLabel("Software workaround for the X86_BUG_ITLB_MULTIHIT bug");
  nxCombo_ = new SaferCombo();
  nxCombo_->addItems({ "auto", "off", "force" });
  auto* nxLayout = new QVBoxLayout();
  nxLayout->setContentsMargins(0, 0, 0, 0);
  nxLayout->addWidget(nxLabel_);
  nxLayout->addWidget(nxCombo_);
  nxCombo_->setToolTip("Value for kernel parameter 'kvm.nx_huge_pages='");

  mitCombo_ = new SaferCombo();
  mitCombo_->addItems({
      "Automaticly detect vulnerabilities",
      "Disable all mitigations",
      "Automaticly detect vulnerabilities and disable SMT (if needed)",
      "Manual selection" });
  mitCombo_->setToolTip("<p>Value for kernel parameter 'mitigations='</p><p>(combines all the vulnerability mitigation parameters below...)</p>");
  auto* mit_layout = new QVBoxLayout();
  mit_layout->addWidget(mitCombo_);
  mit_layout->addSpacing(12);
  mit_layout->addLayout(v1Layout);
  mit_layout->addLayout(v2Combined);
  mit_layout->addLayout(specLayout);
  mit_layout->addLayout(ptiLayout);
  mit_layout->addLayout(mdsLayout);
  mit_layout->addLayout(taaLayout);
  mit_layout->addLayout(l1tfLayout);
  mit_layout->addLayout(nxLayout);

  auto* mit_box = new QGroupBox("CPU vulnerability mitigations");
  mit_box->setFlat(true);
  mit_box->setLayout(mit_layout);

  auto* vm_layout = new QVBoxLayout();
  vm_layout->setContentsMargins(0, 0, 0, 0);
  vm_layout->addStretch(1);
  vm_layout->addWidget(pstateBox);
  vm_layout->addStretch(1);
  vm_layout->addWidget(mit_box);
  vm_layout->addStretch(1);

  auto* box_layout = new QVBoxLayout();
  box_layout->setContentsMargins(0, 0, 0, 0);
  box_layout->addLayout(vm_layout);

  auto* box = new QGroupBox("Adjust Kernel Boot Parameters");
  box->setFlat(true);
  box->setLayout(box_layout);
  box->setEnabled((status) ? false : true);

  /* Add the inner widgets to the layout of the scroll widget */

  scroll_layout_->setContentsMargins(0, 0, 0, 0);
  scroll_layout_->addWidget(box);

  /* signals */

  connect(mitCombo_, SIGNAL(currentIndexChanged(int)),
      this, SLOT(mitIndexChanged(int)));

  connect(pstateDisable_, SIGNAL(stateChanged(int)),
      this, SLOT(pstateDisableChanged(int)));

  connect(pstatePassive_, SIGNAL(stateChanged(int)),
      this, SLOT(pstatePassiveChanged(int)));

  connect(pstateForce_, SIGNAL(stateChanged(int)),
      this, SLOT(pstateForceChanged(int)));

  connect(pstateNoHwp_, SIGNAL(stateChanged(int)),
      this, SLOT(pstateNoHwpChanged(int)));

  connect(pstateHwpOnly_, SIGNAL(stateChanged(int)),
      this, SLOT(pstateHwpOnlyChanged(int)));

  connect(pstateAcpiPpc_, SIGNAL(stateChanged(int)),
      this, SLOT(pstateAcpiPpcChanged(int)));

  connect(pstatePerCpu_, SIGNAL(stateChanged(int)),
      this, SLOT(pstatePerCpuChanged(int)));

  connect(v1Combo_, SIGNAL(currentIndexChanged(int)),
      this, SLOT(indexChanged(int)));

  connect(v2Combo_, SIGNAL(currentIndexChanged(int)),
      this, SLOT(indexChanged(int)));

  connect(v2usrCombo_, SIGNAL(currentIndexChanged(int)),
      this, SLOT(indexChanged(int)));

  connect(specCombo_, SIGNAL(currentIndexChanged(int)),
      this, SLOT(indexChanged(int)));

  connect(ptiCombo_, SIGNAL(currentIndexChanged(int)),
      this, SLOT(indexChanged(int)));

  connect(mdsCombo_, SIGNAL(currentIndexChanged(int)),
      this, SLOT(indexChanged(int)));

  connect(taaCombo_, SIGNAL(currentIndexChanged(int)),
      this, SLOT(indexChanged(int)));

  connect(l1tfCombo_, SIGNAL(currentIndexChanged(int)),
      this, SLOT(indexChanged(int)));

  connect(nxCombo_, SIGNAL(currentIndexChanged(int)),
      this, SLOT(indexChanged(int)));

  connect(&shell_, SIGNAL(finished()), &shell_, SLOT(accept()));
}

bool Grub::read(std::ostringstream&) {
  return true;
}

void Grub::load() {
  data_ = backup_;
}

void Grub::store() {
  store(data_);
}

void Grub::refresh() {

  /* P state */

  pstateDisable_->blockSignals(true);
  pstatePassive_->blockSignals(true);
  pstateForce_->blockSignals(true);
  pstateNoHwp_->blockSignals(true);
  pstateHwpOnly_->blockSignals(true);
  pstateAcpiPpc_->blockSignals(true);
  pstatePerCpu_->blockSignals(true);

  pstateDisable_->setEnabled(true);
  pstatePassive_->setEnabled(true);
  pstateForce_->setEnabled(true);
  pstateNoHwp_->setEnabled(true);
  pstateHwpOnly_->setEnabled(true);
  pstateAcpiPpc_->setEnabled(true);
  pstatePerCpu_->setEnabled(true);

  pstateDisable_->setChecked(data_.intel_pstate_ && intel_pstate_t::Disable);
  pstatePassive_->setChecked(data_.intel_pstate_ && intel_pstate_t::Passive);
  pstateForce_->setChecked(data_.intel_pstate_ && intel_pstate_t::Force);
  pstateNoHwp_->setChecked(data_.intel_pstate_ && intel_pstate_t::No_hwp);
  pstateHwpOnly_->setChecked(data_.intel_pstate_ && intel_pstate_t::Hwp_only);
  pstateAcpiPpc_->setChecked(data_.intel_pstate_ && intel_pstate_t::Support_acpi_ppc);
  pstatePerCpu_->setChecked(data_.intel_pstate_ && intel_pstate_t::Per_cpu_perf_limits);

  if (data_.intel_pstate_ && intel_pstate_t::Passive) {
    pstateHwpOnly_->setEnabled(false);
  }

  if ((data_.intel_pstate_ && intel_pstate_t::Hwp_only) ||
      (data_.intel_pstate_ && intel_pstate_t::No_hwp)) {
    pstatePassive_->setEnabled(false);
  }

  if (data_.intel_pstate_ && intel_pstate_t::No_hwp) {
    pstateHwpOnly_->setEnabled(false);
  }

  if (data_.intel_pstate_ && intel_pstate_t::Hwp_only) {
    pstateNoHwp_->setEnabled(false);
  }

  if (data_.intel_pstate_ && intel_pstate_t::Disable) {
    pstatePassive_->setEnabled(false);
    pstateForce_->setEnabled(false);
    pstateNoHwp_->setEnabled(false);
    pstateHwpOnly_->setEnabled(false);
    pstateAcpiPpc_->setEnabled(false);
    pstatePerCpu_->setEnabled(false);
  }
  else {
    if (data_.intel_pstate_ != intel_pstate_t::Null) {
      pstateDisable_->setEnabled(false);
    }
  }

  pstateDisable_->blockSignals(false);
  pstatePassive_->blockSignals(false);
  pstateForce_->blockSignals(false);
  pstateNoHwp_->blockSignals(false);
  pstateHwpOnly_->blockSignals(false);
  pstateAcpiPpc_->blockSignals(false);
  pstatePerCpu_->blockSignals(false);

  /* Vulnerability Mitigations */

  blockSignals(true);
  mitCombo_->setCurrentIndex(enum_cast(data_.mitigations_));
  blockSignals(false);
}

bool Grub::apply() {

  if (doApply) {
    auto&& var = generateGrubConfig(data_);

    DBGMSG("Grub::apply(): New value for /etc/default/grub:")
    DBGMSG("Grub::apply():" << var.c_str())

    /* write the /etc/default/grub file */
    vsCfg_[static_cast<size_t>(lineno_)] = std::move(var);
    std::ofstream ofs(TabSettings::GrubCfgPath, std::ofstream::trunc);
    if (ofs.good()) for (auto& line : vsCfg_) ofs << line.c_str() << std::endl;
    if (!ofs.good()) {
      QMessageBox::critical(nullptr, "Core Adjust",
          "Error, could not write /etc/default/grub.");
      return false;
    }
    ofs.flush();
    ofs.close();

    /* Run the update-grub command. */
    DBGMSG("Grub::apply(): Running update-grub...")
    int retv = shell_.run({ TabSettings::Cmd_UpdateGrub });
    DBGMSG("Grub::apply(): exit value:" << retv)

    if (retv == 0) {
      backup_ = data_;
      doApply = false;
    }

    return retv;
  }

  return true;
}

bool Grub::compare() {
  /* Generate the GRUB_CMDLINE_LINUX_DEFAULT from the currently applied values */
  auto&& s1 = generateGrubConfig(data_);

  /* Generate the GRUB_CMDLINE_LINUX_DEFAULT from the current UI values */
  GrubData d(data_);
  store(d);
  auto&& s2 = generateGrubConfig(d);

  /* Test if the two GRUB_CMDLINE_LINUX_DEFAULT strings are the same. */
  if (s1.compare(s2) != 0) {
    doApply = true;
    DBGMSG("Grub::compare(): GRUB_CMDLINE_LINUX_DEFAULT does not match")
    return false;
  }
  doApply = false;
  DBGMSG("Grub::compare(): Settings match")
  return true;
}

/* Store the current UI values in the provided GrubData instance. */
void Grub::store(GrubData& d) {
  d.intel_pstate_ = intel_pstate_t::Null;

  if (pstateDisable_->isChecked()) {
    d.intel_pstate_ = d.intel_pstate_ | intel_pstate_t::Disable;
  }
  if (pstatePassive_->isChecked()) {
    d.intel_pstate_ = d.intel_pstate_ | intel_pstate_t::Passive;
  }
  if (pstateForce_->isChecked()) {
    d.intel_pstate_ = d.intel_pstate_ | intel_pstate_t::Force;
  }
  if (pstateNoHwp_->isChecked()) {
    d.intel_pstate_ = d.intel_pstate_ | intel_pstate_t::No_hwp;
  }
  if (pstateHwpOnly_->isChecked()) {
    d.intel_pstate_ = d.intel_pstate_ | intel_pstate_t::Hwp_only;
  }
  if (pstateAcpiPpc_->isChecked()) {
    d.intel_pstate_ = d.intel_pstate_ | intel_pstate_t::Support_acpi_ppc;
  }
  if (pstatePerCpu_->isChecked()) {
    d.intel_pstate_ = d.intel_pstate_ | intel_pstate_t::Per_cpu_perf_limits;
  }

  d.mitigations_ = static_cast<mitigations_t>(mitCombo_->currentIndex());
  d.spectre_v1_ = static_cast<spectre_v1_t>(v1Combo_->currentIndex());
  d.spectre_v2_ = static_cast<spectre_v2_t>(v2Combo_->currentIndex());
  d.spectre_v2_user_ = static_cast<spectre_v2_user_t>(v2usrCombo_->currentIndex());
  d.spec_store_bypass_disable_ = static_cast<spec_store_bypass_disable_t>(specCombo_->currentIndex());
  d.pti_ = static_cast<pti_t>(ptiCombo_->currentIndex());
  d.mds_ = static_cast<mds_t>(mdsCombo_->currentIndex());
  d.tsx_async_abort_ = static_cast<tsx_async_abort_t>(taaCombo_->currentIndex());
  d.l1tf_ = static_cast<l1tf_t>(l1tfCombo_->currentIndex());
  d.nx_huge_pages_ = static_cast<nx_huge_pages_t>(nxCombo_->currentIndex());
}

/* Generate the GRUB_CMDLINE_LINUX_DEFAULT from the provided GrubData instance. */
std::string Grub::generateGrubConfig(GrubData& data) {
  /* create a string(stream) with the new value
   * for GRUB_CMDLINE_LINUX_DEFAULT */

  std::stringstream ss;
  bool need_seperator = false;

  ss << "GRUB_CMDLINE_LINUX_DEFAULT=\"";

  /* prepend the 'other' kernel parameters */
  for (auto& str : vsOther_) ss << str << ' ';

  /* append the intel_pstate= parameter */
  if (data.intel_pstate_ != intel_pstate_t::Null) {
    if (need_seperator) ss << ' ';
    need_seperator = false;
    ss << "intel_pstate=";
    if (data.intel_pstate_ && intel_pstate_t::Disable) {
      if (need_seperator) ss << ',';
      ss << "disable";
      need_seperator = true;
    }
    if (data.intel_pstate_ && intel_pstate_t::Passive) {
      if (need_seperator) ss << ',';
      ss << "passive";
      need_seperator = true;
    }
    if (data.intel_pstate_ && intel_pstate_t::Force) {
      if (need_seperator) ss << ',';
      ss << "force";
      need_seperator = true;
    }
    if (data.intel_pstate_ && intel_pstate_t::No_hwp) {
      if (need_seperator) ss << ',';
      ss << "no_hwp";
      need_seperator = true;
    }
    if (data.intel_pstate_ && intel_pstate_t::Hwp_only) {
      if (need_seperator) ss << ',';
      ss << "hwp_only";
      need_seperator = true;
    }
    if (data.intel_pstate_ && intel_pstate_t::Support_acpi_ppc) {
      if (need_seperator) ss << ',';
      ss << "support_acpi_ppc";
      need_seperator = true;
    }
    if (data.intel_pstate_ && intel_pstate_t::Per_cpu_perf_limits) {
      if (need_seperator) ss << ',';
      ss << "per_cpu_perf_limits";
      need_seperator = true;
    }
    need_seperator = true;
  }

  /* append the mitigations= parameter */
  switch (data.mitigations_) {
    case mitigations_t::Auto:
      break;
    case mitigations_t::Off:
      if (need_seperator) ss << ' ';
      ss << "mitigations=off";
      need_seperator = true;
      break;
    case mitigations_t::AutoNosmt:
      if (need_seperator) ss << ' ';
      ss << "mitigations=auto,nosmt";
      need_seperator = true;
      break;
    case mitigations_t::Manual:
      {
        if (data.spectre_v1_ == spectre_v1_t::Off) {
          if (need_seperator) ss << ' ';
          ss << "nospectre_v1";
          need_seperator = true;
        }

        if (data.spectre_v2_ != spectre_v2_t::Auto) {
          if (need_seperator) ss << ' ';
          switch (data.spectre_v2_) {
            case spectre_v2_t::Auto:
              break;
            case spectre_v2_t::Off:
              ss << "nospectre_v2";
              break;
            case spectre_v2_t::On:
              ss << "spectre_v2=on";
              break;
            case spectre_v2_t::Retpoline:
              ss << "spectre_v2=retpoline";
              break;
            case spectre_v2_t::RetpolineGeneric:
              ss << "spectre_v2=retpoline,generic";
              break;
            case spectre_v2_t::RetpolineAmd:
              ss << "spectre_v2=retpoline,amd";
              break;
          }
          need_seperator = true;
        }

        if (data.spectre_v2_user_ != spectre_v2_user_t::Auto) {
          if (need_seperator) ss << ' ';
          switch (data.spectre_v2_user_) {
            case spectre_v2_user_t::Auto:
              break;
            case spectre_v2_user_t::Off:
              ss << "spectre_v2_user=off";
              break;
            case spectre_v2_user_t::On:
              ss << "spectre_v2_user=on";
              break;
            case spectre_v2_user_t::Prctl:
              ss << "spectre_v2_user=prctl";
              break;
            case spectre_v2_user_t::PrctlIbpb:
              ss << "spectre_v2_user=prctl,ibpb";
              break;
            case spectre_v2_user_t::Seccomp:
              ss << "spectre_v2_user=seccomp";
              break;
            case spectre_v2_user_t::SeccompIbpb:
              ss << "spectre_v2_user=seccomp,ibpb";
              break;
          }
          need_seperator = true;
        }

        if (data.spec_store_bypass_disable_ != spec_store_bypass_disable_t::Auto) {
          if (need_seperator) ss << ' ';
          switch (data.spec_store_bypass_disable_) {
            case spec_store_bypass_disable_t::Auto:
              break;
            case spec_store_bypass_disable_t::Off:
              ss << "spec_store_bypass_disable=off";
              break;
            case spec_store_bypass_disable_t::On:
              ss << "nospec_store_bypass_disable";
              break;
            case spec_store_bypass_disable_t::Prctl:
              ss << "spec_store_bypass_disable=prctl";
              break;
            case spec_store_bypass_disable_t::Seccomp:
              ss << "spec_store_bypass_disable=seccomp";
              break;
          }
          need_seperator = true;
        }

        if (data.pti_ != pti_t::Auto) {
          if (need_seperator) ss << ' ';
          switch (data.pti_) {
            case pti_t::Auto:
              break;
            case pti_t::Off:
              ss << "nopti";
              break;
            case pti_t::On:
              ss << "pti=on";
              break;
          }
          need_seperator = true;
        }

        if (data.mds_ != mds_t::Full) {
          if (need_seperator) ss << ' ';
          switch (data.mds_) {
            case mds_t::Full:
              break;
            case mds_t::Off:
              ss << "mds=off";
              break;
            case mds_t::FullNosmt:
              ss << "mds=full,nosmt";
              break;
          }
          need_seperator = true;
        }

        if (data.tsx_async_abort_ != tsx_async_abort_t::Full) {
          if (need_seperator) ss << ' ';
          switch (data.tsx_async_abort_) {
            case tsx_async_abort_t::Full:
              break;
            case tsx_async_abort_t::Off:
              ss << "tsx_async_abort=off";
              break;
            case tsx_async_abort_t::FullNosmt:
              ss << "tsx_async_abort=full,nosmt";
              break;
          }
          need_seperator = true;
        }

        if (data.l1tf_ != l1tf_t::Flush) {
          if (need_seperator) ss << ' ';
          switch (data.l1tf_) {
            case l1tf_t::Flush:
              break;
            case l1tf_t::Off:
              ss << "l1tf=off";
              break;
            case l1tf_t::Full:
              ss << "l1tf=full";
              break;
            case l1tf_t::FullForce:
              ss << "l1tf=full,force";
              break;
            case l1tf_t::FlushNosmt:
              ss << "l1tf=flush,nosmt";
              break;
            case l1tf_t::FlushNowarn:
              ss << "l1tf=flush,nowarn";
              break;
          }
          need_seperator = true;
        }
      }
      break;
  }

  /* append the kvm.nx_huge_pages= parameter */
  switch (data.nx_huge_pages_) {
    case nx_huge_pages_t::Auto:
      /* mitigations=auto[,nosmt] implies nx_huge_pages=auto unless forced */
      if ((data.mitigations_ != mitigations_t::Auto) &&
          (data.mitigations_ != mitigations_t::AutoNosmt)) {
        if (need_seperator) ss << ' ';
        ss << "kvm.nx_huge_pages=auto";
        need_seperator = true;
      }
      break;
    case nx_huge_pages_t::Off:
      /* mitigations=off implies nx_huge_pages=off unless forced */
      if (data.mitigations_ != mitigations_t::Off) {
        if (need_seperator) ss << ' ';
        ss << "kvm.nx_huge_pages=off";
        need_seperator = true;
      }
      break;
    case nx_huge_pages_t::Force:
      /* nx_huge_pages=force overrides mitigations= */
      if (need_seperator) ss << ' ';
      ss << "kvm.nx_huge_pages=force";
      need_seperator = true;
      break;
  }

  /* 'close' the GRUB_CMDLINE_LINUX_DEFAULT variable*/
  auto&& var = ss.str();
  if (var.back() == 32) var.pop_back();
  var.push_back('"');

  return std::move(var);
}

/* Read /etc/default/grub, parse the GRUB_CMDLINE_LINUX_DEFAULT variable and
 * store the result in data_ and backup_ */
int Grub::parseGrubConfig() {
  /* load /etc/default/grub */
  int max_lines = 1000;
  std::ifstream ifs("/etc/default/grub");
  if (!ifs.good()) {
    /* error, could not open file */
    qDebug() << tr("Warning: failed to read /etc/default/grub, disabling GRUB tab.");
    return 1;
  }

  /* read the file line by line */
  while (ifs.good()) {
    if (--max_lines < 0) {
      /* Error, file larger then 1000 lines? */
      qDebug() << tr("Warning: File /etc/default/grub exceeds 1000 lines, disabling GRUB tab.");
      return 1;
    }
    std::string input;
    std::getline(ifs, input);
    vsCfg_.push_back(std::move(input));
  }
  while (vsCfg_.back().empty()) vsCfg_.pop_back();

  /* locate the line we are interested in */
  for (auto iter = vsCfg_.begin(); iter != vsCfg_.end(); ++iter) {
    if (iter->find("GRUB_CMDLINE_LINUX_DEFAULT") == 0) {
      lineno_ = std::distance(vsCfg_.begin(), iter);
      break;
    }
  }

  /* entry not found?, add it */
  if (lineno_ < 0) {
    vsCfg_.emplace_back("GRUB_CMDLINE_LINUX_DEFAULT=\"\"\n");
    lineno_ = std::distance(vsCfg_.begin(), vsCfg_.end());
  }

  /* copy the entry content */
  std::string line(vsCfg_[static_cast<size_t>(lineno_)], 27); // copy, stripping var name
  xxx::trim(line);
  if (line[0] == '"') {
    line.pop_back();
    line.erase(0, 1);
  }
  /* line == name=value name=value name=value */

  /* tokenize and parse the content */
  xxx::tokenize(line, " ", [this](auto&& var){ // split line into name/value pairs
    auto&& tokens = xxx::tokenize(var, "="); // split pairs into name and value
    const auto& name = tokens.front();
    const auto& value = tokens.back();

    if (name.compare("mitigations") == 0) {
      if (value.compare("off") == 0) {
        data_.mitigations_ = mitigations_t::Off;
      }
      else if (value.compare("auto") == 0) {
        data_.mitigations_ = mitigations_t::Auto;
      }
      else if (value.compare("auto,nosmt") == 0) {
        data_.mitigations_ = mitigations_t::AutoNosmt;
      }
      else {
        vsOther_.emplace_back(std::move(var));
        data_.mitigations_ = mitigations_t::Auto;
      }
    }

    else if (name.compare("nospectre_v1") == 0) {
      data_.spectre_v1_ = spectre_v1_t::Off;
    }

    else if (name.compare("nospectre_v2") == 0) {
      data_.spectre_v2_ = spectre_v2_t::Off;
    }
    else if (name.compare("spectre_v2") == 0) {
      if (value.compare("on") == 0) {
        data_.spectre_v2_ = spectre_v2_t::On;
      }
      else if (value.compare("off") == 0) {
        data_.spectre_v2_ = spectre_v2_t::Off;
      }
      else if (value.compare("retpoline") == 0) {
        data_.spectre_v2_ = spectre_v2_t::Retpoline;
      }
      else if (value.compare("retpoline,generic") == 0) {
        data_.spectre_v2_ = spectre_v2_t::RetpolineGeneric;
      }
      else if (value.compare("retpoline,amd") == 0) {
        data_.spectre_v2_ = spectre_v2_t::RetpolineAmd;
      }
      else if (value.compare("auto") == 0) {
        data_.spectre_v2_ = spectre_v2_t::Auto;
      }
      else {
        vsOther_.emplace_back(std::move(var));
        data_.spectre_v2_ = spectre_v2_t::Auto;
      }
    }

    else if (name.compare("spectre_v2_user") == 0) {
      if (value.compare("on") == 0) {
        data_.spectre_v2_user_ = spectre_v2_user_t::On;
      }
      else if (value.compare("off") == 0) {
        data_.spectre_v2_user_ = spectre_v2_user_t::Off;
      }
      else if (value.compare("prctl") == 0) {
        data_.spectre_v2_user_ = spectre_v2_user_t::Prctl;
      }
      else if (value.compare("prctl,ibpb") == 0) {
        data_.spectre_v2_user_ = spectre_v2_user_t::PrctlIbpb;
      }
      else if (value.compare("seccomp") == 0) {
        data_.spectre_v2_user_ = spectre_v2_user_t::Seccomp;
      }
      else if (value.compare("seccomp,ibpb") == 0) {
        data_.spectre_v2_user_ = spectre_v2_user_t::SeccompIbpb;
      }
      else if (value.compare("auto") == 0) {
        data_.spectre_v2_user_ = spectre_v2_user_t::Auto;
      }
      else {
        vsOther_.emplace_back(std::move(var));
        data_.spectre_v2_user_ = spectre_v2_user_t::Auto;
      }
    }

    else if (name.compare("nospec_store_bypass_disable") == 0) {
      data_.spec_store_bypass_disable_ = spec_store_bypass_disable_t::On;
    }
    else if (name.compare("spec_store_bypass_disable") == 0) {
      if (value.compare("on") == 0) {
        data_.spec_store_bypass_disable_ = spec_store_bypass_disable_t::On;
      }
      else if (value.compare("off") == 0) {
        data_.spec_store_bypass_disable_ = spec_store_bypass_disable_t::Off;
      }
      else if (value.compare("prctl") == 0) {
        data_.spec_store_bypass_disable_ = spec_store_bypass_disable_t::Prctl;
      }
      else if (value.compare("seccomp") == 0) {
        data_.spec_store_bypass_disable_ = spec_store_bypass_disable_t::Seccomp;
      }
      else if (value.compare("auto") == 0) {
        data_.spec_store_bypass_disable_ = spec_store_bypass_disable_t::Auto;
      }
      else {
        vsOther_.emplace_back(std::move(var));
        data_.spec_store_bypass_disable_ = spec_store_bypass_disable_t::Auto;
      }
    }

    else if (name.compare("nopti") == 0) {
      data_.pti_ = pti_t::Off;
    }
    else if (name.compare("pti") == 0) {
      if (value.compare("on") == 0) {
        data_.pti_ = pti_t::On;
      }
      else if (value.compare("off") == 0) {
        data_.pti_ = pti_t::Off;
      }
      else if (value.compare("auto") == 0) {
        data_.pti_ = pti_t::Auto;
      }
      else {
        vsOther_.emplace_back(std::move(var));
        data_.pti_ = pti_t::Auto;
      }
    }

    else if (name.compare("mds") == 0) {
      if (value.compare("full") == 0) {
        data_.mds_ = mds_t::Full;
      }
      else if (value.compare("full,nosmt") == 0) {
        data_.mds_ = mds_t::FullNosmt;
      }
      else if (value.compare("off") == 0) {
        data_.mds_ = mds_t::Off;
      }
      else {
        vsOther_.emplace_back(std::move(var));
        data_.mds_ = mds_t::Full;
      }
    }

    else if (name.compare("tsx_async_abort") == 0) {
      if (value.compare("full") == 0) {
        data_.tsx_async_abort_ = tsx_async_abort_t::Full;
      }
      else if (value.compare("full,nosmt") == 0) {
        data_.tsx_async_abort_ = tsx_async_abort_t::FullNosmt;
      }
      else if (value.compare("off") == 0) {
        data_.tsx_async_abort_ = tsx_async_abort_t::Off;
      }
      else {
        vsOther_.emplace_back(std::move(var));
        data_.tsx_async_abort_ = tsx_async_abort_t::Full;
      }
    }

    else if (name.compare("l1tf") == 0) {
      if (value.compare("full") == 0) {
        data_.l1tf_ = l1tf_t::Full;
      }
      else if (value.compare("full,force") == 0) {
        data_.l1tf_ = l1tf_t::FullForce;
      }
      else if (value.compare("flush") == 0) {
        data_.l1tf_ = l1tf_t::Flush;
      }
      else if (value.compare("flush,nosmt") == 0) {
        data_.l1tf_ = l1tf_t::FlushNosmt;
      }
      else if (value.compare("flush,nowarn") == 0) {
        data_.l1tf_ = l1tf_t::FlushNowarn;
      }
      else if (value.compare("off") == 0) {
        data_.l1tf_ = l1tf_t::Off;
      }
      else {
        vsOther_.emplace_back(std::move(var));
        data_.l1tf_ = l1tf_t::Flush;
      }
    }

    else if (name.compare("kvm.nx_huge_pages") == 0) {
      if (value.compare("force") == 0) {
        data_.nx_huge_pages_ = nx_huge_pages_t::Force;
      }
      else if (value.compare("off") == 0) {
        data_.nx_huge_pages_ = nx_huge_pages_t::Off;
      }
      else if (value.compare("auto") == 0) {
        data_.nx_huge_pages_ = nx_huge_pages_t::Auto;
      }
      else {
        vsOther_.emplace_back(std::move(var));
        data_.nx_huge_pages_ = nx_huge_pages_t::Auto;
      }
    }

    else if (name.compare("intel_pstate") == 0) {
      auto&& options = xxx::tokenize(value, ",");
      for (auto& opt : options) {
        if (opt.compare("disable") == 0) {
          data_.intel_pstate_ = data_.intel_pstate_ | intel_pstate_t::Disable;
        }
        else if (opt.compare("passive") == 0) {
          data_.intel_pstate_ = data_.intel_pstate_ | intel_pstate_t::Passive;
        }
        else if (opt.compare("force") == 0) {
          data_.intel_pstate_ = data_.intel_pstate_ | intel_pstate_t::Force;
        }
        else if (opt.compare("no_hwp") == 0) {
          data_.intel_pstate_ = data_.intel_pstate_ | intel_pstate_t::No_hwp;
        }
        else if (opt.compare("hwp_only") == 0) {
          data_.intel_pstate_ = data_.intel_pstate_ | intel_pstate_t::Hwp_only;
        }
        else if (opt.compare("support_acpi_ppc") == 0) {
          data_.intel_pstate_ = data_.intel_pstate_ | intel_pstate_t::Support_acpi_ppc;
        }
        else if (opt.compare("per_cpu_perf_limits") == 0) {
          data_.intel_pstate_ = data_.intel_pstate_ | intel_pstate_t::Per_cpu_perf_limits;
        }
        else {
          vsOther_.emplace_back(std::move(var));
          data_.intel_pstate_ = intel_pstate_t::Null;
          break;
        }
      }
    }

    else vsOther_.emplace_back(std::move(var));
    return 0;
  });

  /* mitigations=off implies nx_huge_pages=off unless forced */
  if (data_.mitigations_ == mitigations_t::Off) {
    if (data_.nx_huge_pages_ != nx_huge_pages_t::Force) {
      data_.nx_huge_pages_ = nx_huge_pages_t::Off;
    }
  }

  /* mitigations=auto[,nosmt] implies nx_huge_pages=auto unless forced */
  if (data_.mitigations_ == mitigations_t::Auto ||
      data_.mitigations_ == mitigations_t::AutoNosmt ) {
    if (data_.nx_huge_pages_ != nx_huge_pages_t::Force) {
      data_.nx_huge_pages_ = nx_huge_pages_t::Auto;
    }
  }

  /* 'disable' cannot be selected at the same time as any other option */
  if (data_.intel_pstate_ && intel_pstate_t::Disable) {
    if ((data_.intel_pstate_ & ~intel_pstate_t::Disable) != intel_pstate_t::Null) {
      data_.intel_pstate_ = data_.intel_pstate_ & ~intel_pstate_t::Disable;
    }
  }

  /* 'hwp_only' and 'no_hwp' cannot be selected at the same time */
  if (data_.intel_pstate_ && intel_pstate_t::No_hwp) {
    data_.intel_pstate_ = data_.intel_pstate_ & ~intel_pstate_t::Hwp_only;
  }

  /* 'passive' and 'hwp_only'/'no_hwp' cannot be selected at the same time */
  if ((data_.intel_pstate_ && intel_pstate_t::Passive) &&
      ((data_.intel_pstate_ && intel_pstate_t::Hwp_only) ||
        (data_.intel_pstate_ && intel_pstate_t::No_hwp))) {
    data_.intel_pstate_ = data_.intel_pstate_ & ~intel_pstate_t::Passive;
  }

  backup_ = data_;

  return 0;
}

/* SLOTS */

void Grub::pstateDisableChanged(int state) {
  if (state) {
    pstatePassive_->setEnabled(false);
    pstateForce_->setEnabled(false);
    pstateNoHwp_->setEnabled(false);
    pstateHwpOnly_->setEnabled(false);
    pstateAcpiPpc_->setEnabled(false);
    pstatePerCpu_->setEnabled(false);
  }
  else {
    pstatePassive_->setEnabled(true);
    pstateForce_->setEnabled(true);
    pstateNoHwp_->setEnabled(true);
    pstateHwpOnly_->setEnabled(true);
    pstateAcpiPpc_->setEnabled(true);
    pstatePerCpu_->setEnabled(true);
  }
  emit valueChanged(this);
}

void Grub::pstatePassiveChanged(int state) {
  if (state) {
    pstateNoHwp_->setEnabled(false);
    pstateHwpOnly_->setEnabled(false);
    pstateDisable_->setEnabled(false);
  }
  else {
    pstateNoHwp_->setEnabled(true);
    if (pstateNoHwp_->isChecked() == false) {
      pstateHwpOnly_->setEnabled(true);
    }
    if (pstateForce_->isChecked() == false &&
        pstateNoHwp_->isChecked() == false &&
        pstateHwpOnly_->isChecked() == false &&
        pstateAcpiPpc_->isChecked() == false &&
        pstatePerCpu_->isChecked() == false
    ) {
      pstateDisable_->setEnabled(true);
    }
  }
  emit valueChanged(this);
}

void Grub::pstateForceChanged(int state) {
  if (state) {
    pstateDisable_->setEnabled(false);
  }
  else {
    if (pstatePassive_->isChecked() == false &&
        pstateNoHwp_->isChecked() == false &&
        pstateHwpOnly_->isChecked() == false &&
        pstateAcpiPpc_->isChecked() == false &&
        pstatePerCpu_->isChecked() == false
    ) {
      pstateDisable_->setEnabled(true);
    }
  }
  emit valueChanged(this);
}

void Grub::pstateNoHwpChanged(int state) {
  if (state) {
    pstateHwpOnly_->setEnabled(false);
    pstatePassive_->setEnabled(false);
    pstateDisable_->setEnabled(false);
  }
  else {
    pstateHwpOnly_->setEnabled(true);
    pstatePassive_->setEnabled(true);
    if (pstatePassive_->isChecked() == false &&
        pstateForce_->isChecked() == false &&
        pstateHwpOnly_->isChecked() == false &&
        pstateAcpiPpc_->isChecked() == false &&
        pstatePerCpu_->isChecked() == false
    ) {
      pstateDisable_->setEnabled(true);
    }
  }
  emit valueChanged(this);
}

void Grub::pstateHwpOnlyChanged(int state) {
  if (state) {
    pstateNoHwp_->setEnabled(false);
    pstatePassive_->setEnabled(false);
    pstateDisable_->setEnabled(false);
  }
  else {
    pstateNoHwp_->setEnabled(true);
    pstatePassive_->setEnabled(true);
    if (pstatePassive_->isChecked() == false &&
        pstateForce_->isChecked() == false &&
        pstateNoHwp_->isChecked() == false &&
        pstateAcpiPpc_->isChecked() == false &&
        pstatePerCpu_->isChecked() == false
    ) {
      pstateDisable_->setEnabled(true);
    }
  }
  emit valueChanged(this);
}

void Grub::pstateAcpiPpcChanged(int state) {
  if (state) {
    pstateDisable_->setEnabled(false);
  }
  else {
    if (pstatePassive_->isChecked() == false &&
        pstateForce_->isChecked() == false &&
        pstateNoHwp_->isChecked() == false &&
        pstateHwpOnly_->isChecked() == false &&
        pstatePerCpu_->isChecked() == false
    ) {
      pstateDisable_->setEnabled(true);
    }
  }
  emit valueChanged(this);
}

void Grub::pstatePerCpuChanged(int state) {
  if (state) {
    pstateDisable_->setEnabled(false);
  }
  else {
    if (pstatePassive_->isChecked() == false &&
        pstateForce_->isChecked() == false &&
        pstateNoHwp_->isChecked() == false &&
        pstateHwpOnly_->isChecked() == false &&
        pstateAcpiPpc_->isChecked() == false
    ) {
      pstateDisable_->setEnabled(true);
    }
  }
  emit valueChanged(this);
}

void Grub::mitIndexChanged(int index) {

  auto setEnabled = [this](bool state) -> void {
    v1Combo_->setEnabled(state);
    v2Combo_->setEnabled(state);
    v2usrCombo_->setEnabled(state);
    specCombo_->setEnabled(state);
    ptiCombo_->setEnabled(state);
    mdsCombo_->setEnabled(state);
    taaCombo_->setEnabled(state);
    l1tfCombo_->setEnabled(state);
    v1Label_->setEnabled(state);
    v2Label_->setEnabled(state);
    v2usrLabel_->setEnabled(state);
    specLabel_->setEnabled(state);
    ptiLabel_->setEnabled(state);
    mdsLabel_->setEnabled(state);
    taaLabel_->setEnabled(state);
    l1tfLabel_->setEnabled(state);
  };

  mitCombo_->blockSignals(true);
  v1Combo_->blockSignals(true);
  v2Combo_->blockSignals(true);
  v2usrCombo_->blockSignals(true);
  specCombo_->blockSignals(true);
  ptiCombo_->blockSignals(true);
  mdsCombo_->blockSignals(true);
  taaCombo_->blockSignals(true);
  l1tfCombo_->blockSignals(true);
  nxCombo_->blockSignals(true);

  switch (static_cast<mitigations_t>(index)) {
    case mitigations_t::Auto:
      v1Combo_->setCurrentIndex(enum_cast(spectre_v1_t::Auto));
      v2Combo_->setCurrentIndex(enum_cast(spectre_v2_t::Auto));
      v2usrCombo_->setCurrentIndex(enum_cast(spectre_v2_user_t::Auto));
      specCombo_->setCurrentIndex(enum_cast(spec_store_bypass_disable_t::Auto));
      ptiCombo_->setCurrentIndex(enum_cast(pti_t::Auto));
      mdsCombo_->setCurrentIndex(enum_cast(mds_t::Full));
      taaCombo_->setCurrentIndex(enum_cast(tsx_async_abort_t::Full));
      l1tfCombo_->setCurrentIndex(enum_cast(l1tf_t::Flush));
      if (nxCombo_->currentIndex() != enum_cast(nx_huge_pages_t::Force)) {
        nxCombo_->setCurrentIndex(enum_cast(nx_huge_pages_t::Auto));
      }
      setEnabled(false);
      break;
    case mitigations_t::AutoNosmt:
      v1Combo_->setCurrentIndex(enum_cast(spectre_v1_t::Auto));
      v2Combo_->setCurrentIndex(enum_cast(spectre_v2_t::Auto));
      v2usrCombo_->setCurrentIndex(enum_cast(spectre_v2_user_t::Auto));
      specCombo_->setCurrentIndex(enum_cast(spec_store_bypass_disable_t::Auto));
      ptiCombo_->setCurrentIndex(enum_cast(pti_t::Auto));
      mdsCombo_->setCurrentIndex(enum_cast(mds_t::FullNosmt));
      taaCombo_->setCurrentIndex(enum_cast(tsx_async_abort_t::FullNosmt));
      l1tfCombo_->setCurrentIndex(enum_cast(l1tf_t::FlushNosmt));
      if (nxCombo_->currentIndex() != enum_cast(nx_huge_pages_t::Force)) {
        nxCombo_->setCurrentIndex(enum_cast(nx_huge_pages_t::Auto));
      }
      setEnabled(false);
      break;
    case mitigations_t::Off:
      v1Combo_->setCurrentIndex(enum_cast(spectre_v1_t::Off));
      v2Combo_->setCurrentIndex(enum_cast(spectre_v2_t::Off));
      v2usrCombo_->setCurrentIndex(enum_cast(spectre_v2_user_t::Off));
      specCombo_->setCurrentIndex(enum_cast(spec_store_bypass_disable_t::Off));
      ptiCombo_->setCurrentIndex(enum_cast(pti_t::Off));
      mdsCombo_->setCurrentIndex(enum_cast(mds_t::Off));
      taaCombo_->setCurrentIndex(enum_cast(tsx_async_abort_t::Off));
      l1tfCombo_->setCurrentIndex(enum_cast(l1tf_t::Off));
      if (nxCombo_->currentIndex() != enum_cast(nx_huge_pages_t::Force)) {
        nxCombo_->setCurrentIndex(enum_cast(nx_huge_pages_t::Off));
      }
      setEnabled(false);
      break;
    default:
    //case mitigations_t::Manual:
      setEnabled(true);
      break;
  }

  mitCombo_->blockSignals(false);
  v1Combo_->blockSignals(false);
  v2Combo_->blockSignals(false);
  v2usrCombo_->blockSignals(false);
  specCombo_->blockSignals(false);
  ptiCombo_->blockSignals(false);
  mdsCombo_->blockSignals(false);
  taaCombo_->blockSignals(false);
  l1tfCombo_->blockSignals(false);
  nxCombo_->blockSignals(false);

  emit valueChanged(this);
}

void Grub::indexChanged(int) {
  emit valueChanged(this);
}

