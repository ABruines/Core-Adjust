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
  * @file src/core-adjust-qt/CpuId.hpp
  * @brief Provide CPUID information.
  *
  * @file src/core-adjust-qt/CpuId.cpp
  * @brief Provide CPUID information (implementation).
  */
#ifndef CoreAdjust_CpuId
#define CoreAdjust_CpuId

#include <cstddef>
#include <cstdint>
#include <vector>
#include "CpuNumber.hpp"
#include "config.h"

/*
 * See: Intel® 64 and IA-32 Architectures Developer's Manual: Vol. 2A
 *      Table 3-8. Information Returned by CPUID Instruction.
 *      Page 3-191
 *
 * I will (try to) document all bits used by this application in this
 * comment-block:
 *
 * EAX_00H.EAX          - Maximum Input Value for Basic CPUID Information.
 *
 * EAX_01H.EBX[23-16]   - Maximum number of addressable IDs for logical processors
 *                        in this physical package.
 *
 * EAX_01H.ECX[0]  SSE3 - Streaming SIMD Extentions 3 (SSE3) is suported if 1
 * EAX_01H.ECX[7]  EIST - Enhanced Intel SpeedStep Technology is suported if 1
 * EAX_01H.ECX[8]  TM2  - Thermal Monitor #2 is suported if 1
 *
 * EAX_01H.EDX[22] ACPI - Thermal Monitor and Software Controlled Clock Facilities if 1.
 * EAX_01H.EDX[28] HTT  - A value of 1 for indicates the value in CPUID.1.EBX[23:16] is valid
 *
 * EAX_06H.EAX[0] ???  - Digital temperature sensor is supported if set.
 * EAX_06H.EAX[4] PLN  - Power limit notification controls are supported if set.
 * EAX_06H.EAX[5] ECMD - (Clock modulation duty cycle extension) is supported if 1.
 * EAX_06H.EAX[6] PTM  - Package thermal management is supported if set.
 * EAX_06H.EAX[7] HWP  - HWP base registers (IA32_PM_ENABLE[bit 0], IA32_HWP_CAPABILITIES,
 *                       IA32_HWP_REQUEST, IA32_HWP_STATUS) are supported if set.
 *
 */

/** @brief CPUID information for a single processor. */
struct SingleCpuId {
  /** @brief Container for the values of the EAX, EBX,
    * ECX and EDX registers for a given CPUID leaf */
  struct Entry {
    explicit Entry(uint32_t eax=0, uint32_t ebx=0, uint32_t ecx=0, uint32_t edx=0)
      : EAX(eax), EBX(ebx), ECX(ecx), EDX(edx) { }
    uint32_t EAX, EBX, ECX, EDX;
  };
  Entry EAX_00H;
  Entry EAX_01H;
  Entry EAX_02H;
  Entry EAX_03H;
  Entry EAX_04H_ECX_00H;
  Entry EAX_04H_ECX_01H;
  Entry EAX_04H_ECX_02H;
  Entry EAX_04H_ECX_03H;
  Entry EAX_05H;
  Entry EAX_06H;
  Entry EAX_07H;
  Entry EAX_08H;
  Entry EAX_09H;
  Entry EAX_0AH;
  Entry EAX_0BH_ECX_00H;
  Entry EAX_0BH_ECX_01H;
  Entry EAX_0CH;
  Entry EAX_0DH_ECX_00H;
  Entry EAX_0DH_ECX_01H;
  Entry EAX_0DH_ECX_02H;
  Entry EAX_1FH_ECX_01H;
  Entry EAX_80000000H;
  Entry EAX_80000001H;
  Entry EAX_80000002H;
  Entry EAX_80000003H;
  Entry EAX_80000004H;
  Entry EAX_80000005H;
  Entry EAX_80000006H;
  Entry EAX_80000007H;
  Entry EAX_80000008H;
  Entry EAX_80860000H;
  Entry EAX_C0000000H;
};

/** @brief Vector of SingleCpuId, one for each physical processor */
class CpuId : private std::vector<SingleCpuId> {
  public:
    CpuId(bool haveEventLoop);
    ~CpuId() = default;
    using std::vector<SingleCpuId>::size;
    using std::vector<SingleCpuId>::begin;
    using std::vector<SingleCpuId>::end;
    using std::vector<SingleCpuId>::cbegin;
    using std::vector<SingleCpuId>::cend;
    SingleCpuId& operator[](const PhysCpuNr& idx);
    const SingleCpuId& operator[](const PhysCpuNr& idx) const;
    SingleCpuId& at(const PhysCpuNr& idx);
    const SingleCpuId& at(const PhysCpuNr& idx) const;
    void refresh(bool haveEventLoop = true);
#ifdef DEBUG
  private:
    static CpuId* singleton_;
#endif
};

#endif

