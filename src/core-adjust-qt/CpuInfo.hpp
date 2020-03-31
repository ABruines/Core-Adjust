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
 * @file src/core-adjust-qt/CpuInfo.hpp
 * @brief Provide processor model information.
 *
 * @file src/core-adjust-qt/CpuInfo.cpp
 * @brief Provide processor model information (implementation).
 */
#ifndef CoreAdjust_CpuInfo_hpp
#define CoreAdjust_CpuInfo_hpp

// STL
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>
// App
#include "CpuNumber.hpp"
#include "Dbg.hpp"

/** @brief Model information for a single processor (read from /proc/cpuinfo). */
class SingleCpuInfo {
  public:
    /** @brief Enum of Intel 'family 0x06' microarchitectures */
    enum class MARCH {
      UNKNOWN,         /**< Unknown processor */
      FAM6,            /**< CPUID family 0x06, unknown model */
      /* big core */
      CORE,            /**< CPUID family 0x06, model 0x0E */
      CORE2,           /**< CPUID family 0x06, model 0x0F | 0x16 | 0x17 | 0x1D */
      NEHALEM,         /**< CPUID family 0x06, model 0x1E | 0x1F | 0x1A | 0x2E */
      WESTMERE,        /**< CPUID family 0x06, model 0x25 | 0x2C | 0x2F */
      SANDYBRIDGE,     /**< CPUID family 0x06, model 0x2A | 0x2D */
      IVYBRIDGE,       /**< CPUID family 0x06, model 0x3A | 0x3E */
      HASWELL,         /**< CPUID family 0x06, model 0x3C | 0x3F | 0x45 | 0x46 */
      BROADWELL,       /**< CPUID family 0x06, model 0x3D | 0x47 | 0x4F | 0x56 */
      SKYLAKE,         /**< CPUID family 0x06, model 0x4E | 0x5E | 0x55 */
      KABYLAKE,        /**< CPUID family 0x06, model 0x8E | 0x9E */
      CANNONLAKE,      /**< CPUID family 0x06, model 0x66 */
      ICELAKE,         /**< CPUID family 0x06, model 0x6A | 0x6C | 0x7D | 0x7E | 0x9D */
      TIGERLAKE,       /**< CPUID family 0x06, model 0x8C | 0x8D */
      COMETLAKE,       /**< CPUID family 0x06, model 0xA5 | 0xA6 */
      /* small core */
      BONNELL,         /**< @brief CPUID family 0x06, model 0x1C | 0x26 */
      SALTWELL,        /**< @brief CPUID family 0x06, model 0x36 | 0x27 | 0x35 */
      SILVERMONT,      /**< @brief CPUID family 0x06, model 0x37 | 0x4D | 0x4A */
      AIRMONT,         /**< @brief CPUID family 0x06, model 0x4C | 0x5A */
      GOLDMONT,        /**< @brief CPUID family 0x06, model 0x5C | 0x5F | 0x7A */
      TREMONT,         /**< @brief CPUID family 0x06, model 0x86 | 0x96 */
      /* Xeon Phi */
      KNIGHTS_LANDING, /**< @brief CPUID family 0x06, model 0x57 */
      KNIGHTS_MILL     /**< @brief CPUID family 0x06, model 0x85 */
    };

    /** @brief List the names of the CPU's FIVR voltage planes
      * @param march The CPU microarchitecture
      * @returns Vector of FIVR voltage plane names. */
    static std::vector<std::string> IvrNames(MARCH march);
    /** @brief Get the CPU Target Temperature Offset range.
      * @param march The CPU microarchitecture
      * @returns The range of the Temperature Target Offset. */
    static size_t TargetTempRange(MARCH march);

    /** @brief The microarchitecture of this CPU. */
    MARCH MicroArch;

    SingleCpuInfo() = delete;
    ~SingleCpuInfo() = default;
    SingleCpuInfo(const SingleCpuInfo&) = delete;
    SingleCpuInfo& operator=(const SingleCpuInfo&) = delete;
    SingleCpuInfo(SingleCpuInfo&&) = default;
    SingleCpuInfo& operator=(SingleCpuInfo&&) = default;

    explicit SingleCpuInfo(std::string&& _vendor_id,
        unsigned int _family,
        unsigned int _model,
        unsigned int _stepping,
        unsigned int _cores,
        unsigned int _siblings,
        std::string&& _model_name,
        std::string&& _micro_arch,
        std::vector<LogicalCpuNr>&& logical,
        unsigned long phys_cpu_nr);

    /** @brief Get the CPUID VendorID for this processor.
      * @returns String with the CPU VendorID */
    const std::string& vendorId() const;
    /** @brief Get the CPUID Family for this processor.
      * @returns Integer value of the CPU Family. */
    unsigned int family() const;
    /** @brief Get the CPUID Model for this processor.
      * @returns Integer value of the CPU Model. */
    unsigned int model() const;
    /** @brief Get the CPUID Stepping for this processor.
      * @returns Integer value of the CPU Stepping. */
    unsigned int stepping() const;
    /** @brief Get the number of cores in this processor.
      * @returns The number of cores. */
    unsigned int cores() const;
    /** @brief Get the number of siblings in this processor.
      * @returns The number of siblings. */
    unsigned int siblings() const;
    /** @brief Get the CPUID Model Name for this processor.
      * @returns String with the CPU Model Name */
    const std::string& modelName() const;
    /** @brief Get the name of the microarchitecture of this processor.
      * @returns String with the CPU microarchitecture name. */
    const std::string& microArch() const;
    /** @brief Get the the first logical cpu for this processor.
      * @returns The first logical cpu number. */
    LogicalCpuNr firstLogicalCpu() const;
    /** @brief Get the LogicalCpuNr for a given sibling.
      * @param n The (0 based) sibling.
      * @return The LogicalCpuNr. */
    LogicalCpuNr getLogicalCpu(CpuSiblingNr sibling) const;
    /** @brief Get the LogicalCpuNr for a given core.
      * @param n The (0 based) core nr.
      * @return The LogicalCpuNr. */
    LogicalCpuNr getLogicalCpu(CpuCoreNr core) const;
    /** @brief Get the LogicalCpuNr for a all siblings.
      * @return The LogicalCpuNr. */
    std::vector<LogicalCpuNr> getLogicalCpu() const;
    /** @brief Get the physical Id for this processor.
      * @returns The cpu number. */
    PhysCpuNr physicalId() const;

    /** @brief Is this CPU a 'big core' processor? (Core, Xeon).
      * @returns \c true or \c false */
    bool isBigCore() const;
    /** @brief Is this CPU a 'small core' processor? (Atom).
      * @returns \c true or \c false */
    bool isSmallCore() const;
    /** @brief Is this CPU a 'Xeon Phi' processor?
      * @returns \c true or \c false */
    bool isXeonPhi() const;
    /** @brief Is this CPU a 'Haswell' or later processor?
      * @returns \c true or \c false */
    bool isHaswellOrLater() const;
    /** @brief Is this CPU a 'Skylake' or later processor?
      * @returns \c true or \c false */
    bool isSkyLakeOrLater() const;
    /** @brief Is this CPU a 'Silvermont' or later Atom processor?
      * @returns \c true or \c false */
    bool isSilvermontOrLater() const;
    /** @brief Is this CPU a 'Goldmont' or later Atom processor?
      * @returns \c true or \c false */
    bool isGoldmontOrLater() const;
    /** @brief Is this CPU a 'fam 06H' processor?
      * @returns \c true or \c false */
    bool isFam6() const;
    /** @brief Does this CPU have the 'Core' microarchitecture?
      * @returns \c true or \c false */
    bool isCore() const;
    /** @brief Does this CPU have the 'Core2' microarchitecture?
      * @returns \c true or \c false */
    bool isCore2() const;
    /** @brief Does this CPU have the 'Nehalem' microarchitecture?
      * @returns \c true or \c false */
    bool isNehalem() const;
    /** @brief Does this CPU have the 'Westmere' microarchitecture?
      * @returns \c true or \c false */
    bool isWestmere() const;
    /** @brief Does this CPU have the 'Sandybridge' microarchitecture?
      * @returns \c true or \c false */
    bool isSandybridge() const;
    /** @brief Does this CPU have the 'Ivybridge' microarchitecture?
      * @returns \c true or \c false */
    bool isIvybridge() const;
    /** @brief Does this CPU have the 'Haswell' microarchitecture?
      * @returns \c true or \c false */
    bool isHaswell() const;
    /** @brief Does this CPU have the 'Broadwell' microarchitecture?
      * @returns \c true or \c false */
    bool isBroadwell() const;
    /** @brief Does this CPU have the 'Skylake' microarchitecture?
      * @returns \c true or \c false */
    bool isSkylake() const;
    /** @brief Does this CPU have the 'Kabylake' microarchitecture?
      * @returns \c true or \c false */
    bool isKabylake() const;
    /** @brief Does this CPU have the 'Cannonlake' microarchitecture?
      * @returns \c true or \c false */
    bool isCannonlake() const;
    /** @brief Does this CPU have the 'Icelake' microarchitecture?
      * @returns \c true or \c false */
    bool isIcelake() const;
    /** @brief Does this CPU have the 'Tigerlake' microarchitecture?
      * @returns \c true or \c false */
    bool isTigerlake() const;
    /** @brief Does this CPU have the 'Cometlake' microarchitecture?
      * @returns \c true or \c false */
    bool isCometlake() const;
    /** @brief Does this CPU have the 'Bonnell' microarchitecture?
      * @returns \c true or \c false */
    bool isBonnell() const;
    /** @brief Does this CPU have the 'Saltwell' microarchitecture?
      * @returns \c true or \c false */
    bool isSaltwell() const;
    /** @brief Does this CPU have the 'Silvermont' microarchitecture?
      * @returns \c true or \c false */
    bool isSilvermont() const;
    /** @brief Does this CPU have the 'Airmont' microarchitecture?
      * @returns \c true or \c false */
    bool isAirmont() const;
    /** @brief Does this CPU have the 'Goldmont' microarchitecture?
      * @returns \c true or \c false */
    bool isGoldmont() const;
    /** @brief Does this CPU have the 'Tremont' microarchitecture?
      * @returns \c true or \c false */
    bool isTremont() const;
    /** @brief Does this CPU have the 'Knights Landing' microarchitecture?
      * @returns \c true or \c false */
    bool isKnightsLanding() const;
    /** @brief Does this CPU have the 'Knights Mill' microarchitecture?
      * @returns \c true or \c false */
    bool isKnightsMill() const;

  private:
    /* Maps the CPU microarchitecture names to enum MARCH */
    static const std::unordered_map<const char*, MARCH> march_map_;
    std::string vendor_id_;    /* CPUID VendorID */
    unsigned int family_;      /* CPUID Family */
    unsigned int model_;       /* CPUID Model */
    unsigned int stepping_;    /* CPUID Stepping */
    unsigned int cores_;       /* Number of cores per CPU package */
    unsigned int siblings_;    /* (HT per CPU package) * (# of cores per CPU package) */
    std::string flags_;        /* CPUID feature-flags set in /proc/cpuinfo*/
    std::string model_name_;   /* CPUID Model Name */
    std::string micro_arch_;   /* CPU microarchitecture name */
    std::vector<LogicalCpuNr> logical_; /* The logical cpu number for each sibling of this processor. */
    unsigned long physical_id_; /* The physical id for this processor. */
    bool is_big_core_;         /* 'Core'/'Xeon' processor? */
    bool is_small_core_;       /* 'Atom' processor? */
    bool is_xeon_phi_;         /* 'Xeon Phi' processor? */
    bool is_haswell_or_later_; /* Haswell or later microarchitecture? */
    bool is_skylake_or_later_; /* Skylake or later microarchitecture? */
    bool is_silvermont_or_later_; /* Silvermont or later microarchitecture (Atom)? */
    bool is_goldmont_or_later_; /* Goldmont or later microarchitecture (Atom)? */
};

/** @brief Vector of SingleCpuInfo, one for each detected processor. */
class CpuInfo : private std::vector<SingleCpuInfo> {
  public:
    CpuInfo(bool haveEventLoop);
    ~CpuInfo() = default;
    using std::vector<SingleCpuInfo>::size;
    using std::vector<SingleCpuInfo>::begin;
    using std::vector<SingleCpuInfo>::end;
    using std::vector<SingleCpuInfo>::cbegin;
    using std::vector<SingleCpuInfo>::cend;
    SingleCpuInfo& operator[](const PhysCpuNr& idx);
    const SingleCpuInfo& operator[](const PhysCpuNr& idx) const;
    SingleCpuInfo& at(const PhysCpuNr& idx);
    const SingleCpuInfo& at(const PhysCpuNr& idx) const;
    void refresh(bool haveEventLoop = true);
#ifdef DEBUG
private:
    static CpuInfo* singleton_;
#endif
};

#endif

