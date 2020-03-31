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
#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <vector>
// Qt
#include <QApplication>
#include <QMessageBox>
// App
#include "CpuInfo.hpp"
#include "Dbg.hpp"
#include "Shell.hpp"
#include "TabMember.hpp"
#include "Strings.hpp"

const std::unordered_map<const char*, SingleCpuInfo::MARCH>
SingleCpuInfo::march_map_ {
  /* "Big Core" Processors (Branded as Core, Xeon, etc...) */
  { "core",             MARCH::CORE },
  { "core2",            MARCH::CORE2 },
  { "nehalem",          MARCH::NEHALEM },
  { "westmere",         MARCH::WESTMERE },
  { "sandybridge",      MARCH::SANDYBRIDGE },
  { "ivybridge",        MARCH::IVYBRIDGE },
  { "haswell",          MARCH::HASWELL },
  { "broadwell",        MARCH::BROADWELL },
  { "skylake",          MARCH::SKYLAKE },
  { "kabylake",         MARCH::KABYLAKE },
  { "cannonlake",       MARCH::CANNONLAKE },
  { "icelake",          MARCH::ICELAKE },
  { "tigerlake",        MARCH::TIGERLAKE },
  { "cometlake",        MARCH::COMETLAKE },
  /* "Small Core" Processors (Atom) */
  { "bonnell",          MARCH::BONNELL },
  { "saltwell",         MARCH::SALTWELL },
  { "silvermont",       MARCH::SILVERMONT },
  { "airmont",          MARCH::AIRMONT },
  { "goldmont",         MARCH::GOLDMONT },
  { "tremont",          MARCH::TREMONT },
  /* Xeon Phi */
  { "knights landing",  MARCH::KNIGHTS_LANDING },
  { "knights mill",     MARCH::KNIGHTS_MILL },
  /* unknown family 6 member */
  { "unknown fam6",     MARCH::FAM6 },
  /* unknown processor */
  { "unknown",          MARCH::UNKNOWN },
};

std::vector<std::string> SingleCpuInfo::IvrNames(MARCH march) {
  std::vector<std::string> vs;
  switch (march) {
    default:
      vs.emplace_back("Plane 0");
      vs.emplace_back("Plane 1");
      vs.emplace_back("Plane 2");
      vs.emplace_back("Plane 3");
      vs.emplace_back("Plane 4");
      vs.emplace_back("Plane 5");
      break;
    case MARCH::HASWELL:
    case MARCH::BROADWELL:
      vs.emplace_back("Core");
      vs.emplace_back("iGPU");
      vs.emplace_back("Cache");
      vs.emplace_back("UnCore");
      vs.emplace_back("Analog IO");
      vs.emplace_back("Digital IO");
      break;
    case MARCH::SKYLAKE:
    case MARCH::KABYLAKE:
    case MARCH::CANNONLAKE:
    case MARCH::ICELAKE:
    case MARCH::TIGERLAKE:
    case MARCH::COMETLAKE:
    case MARCH::FAM6: /* assume unknown fam6h is newer than the latest known 'big core' cpu */
      vs.emplace_back("Core");
      vs.emplace_back("iGPU Slice");
      vs.emplace_back("Cache");
      vs.emplace_back("iGPU UnSlice");
      vs.emplace_back("UnCore");
      vs.emplace_back("n.a.");
      break;
  }
  return vs;
}

size_t SingleCpuInfo::TargetTempRange(MARCH march) {
  switch (march) {
    case MARCH::IVYBRIDGE:
    case MARCH::HASWELL:
    case MARCH::BROADWELL:
    case MARCH::SKYLAKE:
    case MARCH::KABYLAKE:
    case MARCH::CANNONLAKE:
    case MARCH::ICELAKE:
    case MARCH::TIGERLAKE:
    case MARCH::COMETLAKE:
      return 15;
    case MARCH::SILVERMONT:
    case MARCH::AIRMONT:
    case MARCH::GOLDMONT:
    case MARCH::TREMONT:
    case MARCH::KNIGHTS_LANDING:
    case MARCH::KNIGHTS_MILL:
      return 63;
    default:
      break;
  }
  return 0;
}

SingleCpuInfo::SingleCpuInfo(
    std::string&& _vendor_id,
    unsigned int _family,
    unsigned int _model,
    unsigned int _stepping,
    unsigned int _cores,
    unsigned int _siblings,
    std::string&& _model_name,
    std::string&& _micro_arch,
    std::vector<LogicalCpuNr>&& _logical,
    unsigned long _phys_cpu_nr)
    : MicroArch(MARCH::UNKNOWN),
      vendor_id_(std::move(_vendor_id)),
      family_(_family),
      model_(_model),
      stepping_(_stepping),
      cores_(_cores),
      siblings_(_siblings),
      model_name_(std::move(_model_name)),
      micro_arch_(std::move(_micro_arch)),
      logical_(std::move(_logical)),
      physical_id_(_phys_cpu_nr),
      is_big_core_(false),
      is_small_core_(false),
      is_xeon_phi_(false),
      is_haswell_or_later_(false),
      is_skylake_or_later_(false),
      is_silvermont_or_later_(false),
      is_goldmont_or_later_(false) {

  auto m = micro_arch_;
  std::transform(
      m.begin(), m.end(), m.begin(),
      [](unsigned char c){ return std::tolower(c); }
  );
  for (auto& pair : march_map_) {
    if (m.compare(pair.first) == 0) {
      MicroArch = pair.second;
      break;
    }
  }
  switch (MicroArch) {
    case MARCH::UNKNOWN:
      break;
    // big core
    case MARCH::CORE:
      is_big_core_ = true;
      break;
    case MARCH::CORE2:
      is_big_core_ = true;
      break;
    case MARCH::NEHALEM:
      is_big_core_ = true;
      break;
    case MARCH::WESTMERE:
      is_big_core_ = true;
      break;
    case MARCH::SANDYBRIDGE:
      is_big_core_ = true;
      break;
    case MARCH::IVYBRIDGE:
      is_big_core_ = true;
      break;
    case MARCH::HASWELL:
      is_big_core_ = true;
      is_haswell_or_later_ = true;
      break;
    case MARCH::BROADWELL:
      is_big_core_ = true;
      is_haswell_or_later_ = true;
      break;
    case MARCH::SKYLAKE:
      is_big_core_ = true;
      is_haswell_or_later_ = true;
      is_skylake_or_later_ = true;
      break;
    case MARCH::KABYLAKE:
      is_big_core_ = true;
      is_haswell_or_later_ = true;
      is_skylake_or_later_ = true;
      break;
    case MARCH::CANNONLAKE:
      is_big_core_ = true;
      is_haswell_or_later_ = true;
      is_skylake_or_later_ = true;
      break;
    case MARCH::ICELAKE:
      is_big_core_ = true;
      is_haswell_or_later_ = true;
      is_skylake_or_later_ = true;
      break;
    case MARCH::TIGERLAKE:
      is_big_core_ = true;
      is_haswell_or_later_ = true;
      is_skylake_or_later_ = true;
      break;
    case MARCH::COMETLAKE:
      is_big_core_ = true;
      is_haswell_or_later_ = true;
      is_skylake_or_later_ = true;
      break;
    // small core
    case MARCH::BONNELL:
      is_small_core_ = true;
      break;
    case MARCH::SALTWELL:
      is_small_core_ = true;
      break;
    case MARCH::SILVERMONT:
      is_small_core_ = true;
      is_silvermont_or_later_ = true;
      break;
    case MARCH::AIRMONT:
      is_small_core_ = true;
      is_silvermont_or_later_ = true;
      break;
    case MARCH::GOLDMONT:
      is_small_core_ = true;
      is_silvermont_or_later_ = true;
      is_goldmont_or_later_ = true;
      break;
    case MARCH::TREMONT:
      is_small_core_ = true;
      is_silvermont_or_later_ = true;
      is_goldmont_or_later_ = true;
      break;
    // Xeon Phi
    case MARCH::KNIGHTS_LANDING:
      is_big_core_ = true;
      is_xeon_phi_ = true;
      break;
    case MARCH::KNIGHTS_MILL:
      is_big_core_ = true;
      is_xeon_phi_ = true;
      break;
    // Unknown fam6
    case MARCH::FAM6:
      break;
  }

  DBGMSG("SingleCpuInfo(): vendorID:" << vendorId().c_str())
  DBGMSG("SingleCpuInfo(): family:" << family())
  DBGMSG("SingleCpuInfo(): model:" << model())
  DBGMSG("SingleCpuInfo(): stepping:" << stepping())
  DBGMSG("SingleCpuInfo(): cores:" << cores())
  DBGMSG("SingleCpuInfo(): siblings:" << siblings())
  DBGMSG("SingleCpuInfo(): modelName:" << modelName().c_str())
  DBGMSG("SingleCpuInfo(): microArch:" << microArch().c_str())
#ifdef DEBUG
  {
    auto dbg = qDebug();
    dbg << "SingleCpuInfo(): logical:";
    for (auto l : getLogicalCpu()) dbg << l.value;
  }
#endif
}

#ifdef DEBUG
CpuInfo* CpuInfo::singleton_ = nullptr;
#endif

CpuInfo::CpuInfo(bool haveEventLoop) {
#ifdef DEBUG
  if (singleton_ != nullptr) {
    throw std::runtime_error("There can only be one CpuInfo instance!");
  }
  singleton_ = this;
#endif
  refresh(haveEventLoop);
}

void CpuInfo::refresh(bool haveEventLoop) {
  /* Delete the current data */
  clear();

  /* The shell command 'core-adjust --list --verbose' lists the 
   * physical processors in numerical order. */
  std::vector<std::string> output;
  auto rv = xxx::shell_command(
      { TabSettings::ScriptPath, "--list", "--verbose" },
      [&output](auto, auto str){
    output.push_back(std::move(str));
    return 0;
  });

  if (rv) {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, unexpected output from command:</b></p>"
      "<nobr>core-adjust --list --verbose</nobr>");
    if (haveEventLoop) {
      qApp->exit();
    }
    else {
      /* The Qt event loop is not running yet, so use exit() to quit */
      exit(EXIT_FAILURE);
    }
  }

  /* Parse the output of the shell command */
  for (auto iter = output.begin(); iter != output.end();) {
    /* expecting 10 lines of output per processor */
    if (std::distance(iter, output.end()) < 10) {
      QMessageBox::critical(nullptr, "Core Adjust",
        "<p><b>Error, unexpected output from command:</b></p>"
        "<nobr>core-adjust --list --verbose</nobr>");
      if (haveEventLoop) {
        qApp->exit();
      }
      else {
        exit(EXIT_FAILURE);
      }
    }
    std::string&& s0 = (*iter++).substr(11);  // Processor id
    std::string&& s1 = (*iter++).substr(18);  // vendor id
    std::string&& s2 = (*iter++).substr(18);  // family
    std::string&& s3 = (*iter++).substr(18);  // model
    std::string&& s4 = (*iter++).substr(18);  // stepping
    std::string&& s5 = (*iter++).substr(18);  // cores
    std::string&& s6 = (*iter++).substr(18);  // siblings
    std::string&& s7 = (*iter++).substr(18);  // model name
    std::string&& s8 = (*iter++).substr(18);  // micro arch
    std::string&& s9 = (*iter++).substr(18);  // logical cpus
    s1.pop_back();                            // strip trailing newline
    s7.pop_back();
    s8.pop_back();
    auto logical_str = xxx::tokenize(s9, " ");
    std::vector<LogicalCpuNr> logical;
    for (auto& str : logical_str) {
      logical.emplace_back(std::stoul(str, nullptr, 10));
    }
    emplace_back(
        std::move(s1),
        std::stoul(s2, nullptr, 10),
        std::stoul(s3, nullptr, 10),
        std::stoul(s4, nullptr, 10),
        std::stoul(s5, nullptr, 10),
        std::stoul(s6, nullptr, 10),
        std::move(s7),
        std::move(s8),
        std::move(logical),
        std::stoul(s0, nullptr, 10));
  }

  DBGMSG("CpuInfo::refresh(): Got model information for" << size() << "processor(s)")
}

SingleCpuInfo& CpuInfo::operator[](const PhysCpuNr& idx) {
  return std::vector<SingleCpuInfo>::operator[](idx.value);
}

const SingleCpuInfo& CpuInfo::operator[](const PhysCpuNr& idx) const {
  return std::vector<SingleCpuInfo>::operator[](idx.value);
}


SingleCpuInfo& CpuInfo::at(const PhysCpuNr& idx) {
  return std::vector<SingleCpuInfo>::at(idx.value);
}

const SingleCpuInfo& CpuInfo::at(const PhysCpuNr& idx) const {
  return std::vector<SingleCpuInfo>::at(idx.value);
}


const std::string& SingleCpuInfo::vendorId() const {
  return vendor_id_;
}

unsigned int SingleCpuInfo::family() const {
  return family_;
}

unsigned int SingleCpuInfo::model() const {
  return model_;
}

unsigned int SingleCpuInfo::stepping() const {
  return stepping_;
}

unsigned int SingleCpuInfo::cores() const {
  return cores_;
}

unsigned int SingleCpuInfo::siblings() const {
  return siblings_;
}

const std::string& SingleCpuInfo::modelName() const {
  return model_name_;
}

const std::string& SingleCpuInfo::microArch() const {
  return micro_arch_;
}

LogicalCpuNr SingleCpuInfo::firstLogicalCpu() const {
  return logical_.front();
}

LogicalCpuNr SingleCpuInfo::getLogicalCpu(CpuSiblingNr sibling) const {
  return logical_[sibling.value];
}

LogicalCpuNr SingleCpuInfo::getLogicalCpu(CpuCoreNr core) const {
  if (cores_ == siblings_) {
    return logical_[core.value];
  }
  size_t n = siblings_ / cores_;
  return logical_[n * core.value];
}

std::vector<LogicalCpuNr> SingleCpuInfo::getLogicalCpu() const {
  return logical_;
}

PhysCpuNr SingleCpuInfo::physicalId() const {
  return PhysCpuNr(physical_id_);
}

bool SingleCpuInfo::isBigCore() const {
  return is_big_core_;
}

bool SingleCpuInfo::isSmallCore() const {
  return is_small_core_;
}

bool SingleCpuInfo::isXeonPhi() const {
  return is_xeon_phi_;
}

bool SingleCpuInfo::isHaswellOrLater() const {
  return is_haswell_or_later_;
}

bool SingleCpuInfo::isSkyLakeOrLater() const {
  return is_skylake_or_later_;
}

bool SingleCpuInfo::isSilvermontOrLater() const {
  return is_silvermont_or_later_;
}

bool SingleCpuInfo::isGoldmontOrLater() const {
  return is_goldmont_or_later_;
}

bool SingleCpuInfo::isFam6() const {
  switch (MicroArch) {
    default: break;
    case MARCH::UNKNOWN:
      return false;
  }
  return true;
}

bool SingleCpuInfo::isCore() const {
  if (MicroArch == MARCH::CORE) return true;
  return false;
}

bool SingleCpuInfo::isCore2() const {
  if (MicroArch == MARCH::CORE2) return true;
  return false;
}

bool SingleCpuInfo::isNehalem() const {
  if (MicroArch == MARCH::NEHALEM) return true;
  return false;
}

bool SingleCpuInfo::isWestmere() const {
  if (MicroArch == MARCH::WESTMERE) return true;
  return false;
}

bool SingleCpuInfo::isSandybridge() const {
  if (MicroArch == MARCH::SANDYBRIDGE) return true;
  return false;
}

bool SingleCpuInfo::isIvybridge() const {
  if (MicroArch == MARCH::IVYBRIDGE) return true;
  return false;
}

bool SingleCpuInfo::isHaswell() const {
  if (MicroArch == MARCH::HASWELL) return true;
  return false;
}

bool SingleCpuInfo::isBroadwell() const {
  if (MicroArch == MARCH::BROADWELL) return true;
  return false;
}

bool SingleCpuInfo::isSkylake() const {
  if (MicroArch == MARCH::SKYLAKE) return true;
  return false;
}

bool SingleCpuInfo::isKabylake() const {
  if (MicroArch == MARCH::KABYLAKE) return true;
  return false;
}

bool SingleCpuInfo::isCannonlake() const {
  if (MicroArch == MARCH::CANNONLAKE) return true;
  return false;
}

bool SingleCpuInfo::isIcelake() const {
  if (MicroArch == MARCH::ICELAKE) return true;
  return false;
}

bool SingleCpuInfo::isTigerlake() const {
  if (MicroArch == MARCH::TIGERLAKE) return true;
  return false;
}

bool SingleCpuInfo::isCometlake() const {
  if (MicroArch == MARCH::COMETLAKE) return true;
  return false;
}

bool SingleCpuInfo::isBonnell() const {
  if (MicroArch == MARCH::BONNELL) return true;
  return false;
}

bool SingleCpuInfo::isSaltwell() const {
  if (MicroArch == MARCH::SALTWELL) return true;
  return false;
}

bool SingleCpuInfo::isSilvermont() const {
  if (MicroArch == MARCH::SILVERMONT) return true;
  return false;
}

bool SingleCpuInfo::isAirmont() const {
  if (MicroArch == MARCH::AIRMONT) return true;
  return false;
}

bool SingleCpuInfo::isGoldmont() const {
  if (MicroArch == MARCH::GOLDMONT) return true;
  return false;
}

bool SingleCpuInfo::isTremont() const {
  if (MicroArch == MARCH::TREMONT) return true;
  return false;
}

bool SingleCpuInfo::isKnightsLanding() const {
  if (MicroArch == MARCH::KNIGHTS_LANDING) return true;
  return false;
}

bool SingleCpuInfo::isKnightsMill() const {
  if (MicroArch == MARCH::KNIGHTS_MILL) return true;
  return false;
}

