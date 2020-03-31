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
 * @file src/core-adjust-qt/Msr.hpp 
 * @brief Read and write Machine Specific Registers of Intel processors.
 * @file src/core-adjust-qt/Msr.cpp 
 * @brief Read and write Machine Specific Registers of Intel processors.
 */
#ifndef CoreAdjust_Msr
#define CoreAdjust_Msr

#include <cstdint>
#include "CpuNumber.hpp"

 /*
  * Scope: Shared       ===  one MSR for all logical processors
  * Scope: Unique       ===  each logical processor has its own MSR
  * Scope: Package      ===  one MSR for all cores in the package
  * Scope: Module       ===  one MSR for a pair of cores in the package
  * Scope: Core         ===  one MSR for each core in the package
  */

/** @brief Read an MSR by calling the 'rdmsr' shell-command.
  * @param cpu The logical CPU number.
  * @param address The address of the MSR.
  * @returns The value read from the MSR.
  * @note Throws a std::exception if the MSR could not be read. */
uint64_t readMsr(LogicalCpuNr cpu, int address);

/** @brief Write an MSR by calling the 'wrmsr' shell-command.
  * @param cpu The logical CPU number.
  * @param address The address of the MSR.
  * @param value The value to write to the MSR.
  * @returns 0 on success, non-zero on error. */
int writeMsr(LogicalCpuNr cpu, int address, uint64_t value);

/** @class MsrBase
  * @brief Base class that must be inherited by all MSR classes.
  *
  * @var LogicalCpuNr MsrBase::processor_number
  * @brief The logical CPU number this MsrBase instance belongs to.
  *
  * @fn static T MsrBase::Read(LogicalCpuNr processor)
  * @brief Read the MSR value from the processor.
  * @note Silently sets 'value' to 0 if the MSR could not be read.
  *
  * @fn int MsrBase::read()
  * @brief Read the MSR value from the processor.
  * @returns 0 on success, 1 on error
  *
  * @fn void MsrBase::write()
  * @brief Write the MSR value back to the processor.
  * @returns 0 on success, 1 on error
 */
template<class T, int ADDRESS>
class MsrBase {
  protected:
    LogicalCpuNr processor_number_;
  public:
    MsrBase() = delete;
    MsrBase(LogicalCpuNr processor, uint64_t v = 0);
    ~MsrBase() = default;
    static constexpr int Address = ADDRESS;
    static T Read(LogicalCpuNr processor);
    int read();
    int write();
    bool operator==(const T& rhs);
    bool operator!=(const T& rhs);
};

template<class T, int ADDRESS>
MsrBase<T,ADDRESS>::MsrBase(LogicalCpuNr processor, uint64_t v)
  : processor_number_(processor) {
  static_cast<T*>(this)->value = v;
}

template<class T, int ADDRESS>
inline T MsrBase<T,ADDRESS>::Read(LogicalCpuNr processor) {
  try { return T(processor, readMsr(processor, Address)); }
  catch (...) { return T(processor); }
}

template<class T, int ADDRESS>
inline int MsrBase<T,ADDRESS>::read() {
  static_cast<T*>(this)->value = 0;
  try { static_cast<T*>(this)->value = readMsr(processor_number_, Address); }
  catch (...) { return 1; }
  return 0;
}

template<class T, int ADDRESS>
inline int MsrBase<T,ADDRESS>::write() {
  return writeMsr(processor_number_, Address, static_cast<T*>(this)->value);
}

template<class T, int ADDRESS>
inline bool MsrBase<T,ADDRESS>::operator==(const T& rhs) {
  return static_cast<T*>(this)->value == rhs.value;
}

template<class T, int ADDRESS>
inline bool MsrBase<T,ADDRESS>::operator!=(const T& rhs) {
  return static_cast<T*>(this)->value != rhs.value;
}

/** @class MSR_PLATFORM_ID
  * @brief MSR 0x17 : Platform ID (RO)
  * <p>The operating system can use this MSR to determine “slot” information for the
  * processor and the proper microcode update to load.</p>
  * @note Scope: Package (one MSR for all cores in the package)
  *
  * @var uint64_t MSR_PLATFORM_ID::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t MSR_PLATFORM_ID::reserved_7_0;
  * <p>Reserved bits 7:0</p>
  *
  * @var uint64_t MSR_PLATFORM_ID::Maximum_Qualified_Ratio;
  * <p>The maximum allowed busratio.</p>
  *
  * @var uint64_t MSR_PLATFORM_ID::reserved_49_13;
  * <p>Reserved bits 49:13</p>
  *
  * @var uint64_t MSR_PLATFORM_ID::Platform_Id;
  * <p>Contains information concerning the
  * intended platform for the processor.</p>
  *
  * @var uint64_t MSR_PLATFORM_ID::reserved_63_53;
  * <p>Reserved bits 63:53</p> */
class MSR_PLATFORM_ID : public MsrBase<MSR_PLATFORM_ID, 23> {
  public:
    using MsrBase<MSR_PLATFORM_ID, 23>::MsrBase;
    int write() = delete; /**< Deleted because this MSR is read-only. */
    union {
      uint64_t value;
      struct {
        uint64_t reserved_7_0 : 8;            /**< @brief MSR_PLATFORM_ID bits 7:0 (8) */
        uint64_t Maximum_Qualified_Ratio : 5; /**< @brief MSR_PLATFORM_ID bits 12:8 (5) */
        uint64_t reserved_49_13 : 37;         /**< @brief MSR_PLATFORM_ID bits 49:13 (37) */
        uint64_t Platform_Id : 3;             /**< @brief MSR_PLATFORM_ID bits 52:50 (3) */
        uint64_t reserved_63_53 : 11;         /**< @brief MSR_PLATFORM_ID bits  63:53 (11) */
      };
    };
};

/** @class MSR_FSB_FREQ
  * @brief MSR 0xCD : Scaleable Bus Speed (RO)
  * @note Scope: Shared: (one MSR for all logical processors)
  *
  * @var uint64_t MSR_FSB_FREQ::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t MSR_FSB_FREQ::Bus_Speed;
  * <p>This field indicates the intended scaleable bus clock speed
  * for processors based on Enhanced Intel Core microarchitecture.</p>
  * <table>
  * <tr><td>101B</td><td>100 MHz (FSB 400)</td></tr>
  * <tr><td>001B</td><td>133 MHz(FSB 533)</td></tr>
  * <tr><td>011B</td><td>167 MHz (FSB 667)</td></tr>
  * <tr><td>010B</td><td>200 MHz(FSB 800)</td></tr>
  * <tr><td>000B</td><td>267 MHz (FSB 1067)</td></tr>
  * <tr><td>100B</td><td>333 MHz (FSB 1333)</td></tr>
  * <tr><td>110B</td><td>400 MHz (FSB 1600)</td></tr>
  * </table>
  *
  * @var uint64_t MSR_FSB_FREQ::reserved_63_3;
  * <p>Reserved bits 63:3</p> */
class MSR_FSB_FREQ : public MsrBase<MSR_FSB_FREQ, 205> {
  public:
    using MsrBase<MSR_FSB_FREQ, 205>::MsrBase;
    int write() = delete; /**< Deleted because this MSR is read-only. */
    union {
      uint64_t value;
      struct {
        uint64_t Bus_Speed : 3;      /**< @brief MSR_FSB_FREQ bits 2:0 (3) */
        uint64_t reserved_63_3 : 61; /**< @brief MSR_FSB_FREQ bits 63:3 (61) */
      };
    };
};

/** @class MSR_PLATFORM_INFO
  * @brief MSR 0xCE : Platform Information (RO)
  * <p>For 'big core' processors (Core, Xeon, Xeon Phi):<br/>
  *  - This MSR is available on Nehalem and later.<br/>
  *  - The PPIN_CAP bit is available on Broadwell and later.<p/>
  * <p>For 'small core' processors (Atom):<br/>
  *  - Silvermont/Airmont: only Maximum_Non_Turbo_Ratio is valid.<br/>
  *  - Goldmont+: all fields are valid except where noted.<p/>
  * @note Scope: Package (one MSR for all cores in the package)
  *
  * @var uint64_t MSR_PLATFORM_INFO::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t MSR_PLATFORM_INFO::reserved_7_0;
  * <p>Reserved bits 7:0</p>
  *
  * @var uint64_t MSR_PLATFORM_INFO::Maximum_Non_Turbo_Ratio
  * <p>Maximum Non-Turbo Ratio (RO)</p>
  * <p>This is the ratio of the frequency that invariant TSC runs at.<br/>
  * Frequency = ratio * 100 MHz.</p>
  * @note big core: Nehalem and later, small core: Silvermont and later.
  *
  * @var uint64_t MSR_PLATFORM_INFO::reserved_22_16;
  * <p>Reserved bits 22:16</p>
  *
  * @var uint64_t MSR_PLATFORM_INFO::PPIN_CAP; 
  * <p>PPIN_CAP (RO)</p>
  * <p>When set to 1, indicates that Protected Processor
  * Inventory Number (PPIN) capability can be enabled for
  * a privileged system inventory agent to read PPIN from MSR_PPIN.<br/>
  * When set to 0, PPIN capability is not supported,
  * an attempt to access MSR_PPIN_CTL or MSR_PPIN will cause #GP.</p>
  * @note big core: Broadwell and later, small core: Goldmont and later.
  *
  * @var uint64_t MSR_PLATFORM_INFO::reserved_27_24;
  * <p>Reserved bits 27:24</p>
  *
  * @var uint64_t MSR_PLATFORM_INFO::Programmable_Ratio_Limit_For_Turbo_Mode;
  * <p>Programmable Ratio Limit for Turbo Mode (RO)</p>
  * <p>When set to 1, indicates that Programmable Ratio
  * Limit for Turbo mode is enabled.<br/> When set to 0,
  * indicates Programmable Ratio Limit for Turbo mode
  * is disabled.</p>
  * @note big core: Nehalem and later, small core: Goldmont and later.
  *
  * @var uint64_t MSR_PLATFORM_INFO::Programmable_TDP_Limit_For_Turbo_Mode;
  * <p>Programmable TDP Limit for Turbo Mode (RO)</p>
  * <p>When set to 1, indicates that TDP Limit for Turbo
  * mode is programmable.<br/> When set to 0, indicates TDP
  * Limit for Turbo mode is not programmable.</p>
  * @note big core: Nehalem and later, small core: Goldmont and later.
  *
  * @var uint64_t MSR_PLATFORM_INFO::Programmable_TJ_OFFSET;
  * <p>Programmable TJ OFFSET</p>
  * <p>When set to 1, indicates that MSR_TEMPERATURE_TARGET::Temperature_Target
  * is valid and writeable to specifiy a temperature offset.</p>
  * @note big core: Ivybridge-E, Haswell and later, small core: Goldmont and later.
  *
  * @var uint64_t MSR_PLATFORM_INFO::reserved_31;
  * <p>Reserved bit 31</p>
  *
  * @var uint64_t MSR_PLATFORM_INFO::Low_Power_Mode_Support;
  * <p>Low Power Mode Support (LPM) (RO)</p>
  * <p>When set to 1, indicates that LPM is supported.<br/>
  * When set to 0, indicates LPM is not supported.</p>
  * @note big core: Ivybridge only, small core: not available.
  *
  * @var uint64_t MSR_PLATFORM_INFO::Number_Of_ConfigTDP_Levels;
  * <p>Number of ConfigTDP Levels (RO)</p>
  * <p>00: Only Base TDP level available.<br/>
  * 01: One additional TDP level available.<br/>
  * 02: Two additional TDP level available.<br/>
  * 11: Reserved</p>
  * @note Ivybridge and Haswell only
  *
  * @var uint64_t MSR_PLATFORM_INFO::reserved_39_35;
  * <p>Reserved bits 39:35</p>
  *
  * @var uint64_t MSR_PLATFORM_INFO::Maximum_Efficiency_Ratio;
  * <p>Maximum Efficiency Ratio (RO)</p>
  * <p>This is the minimum ratio (maximum efficiency) that
  * the processor can operate, in units of 100MHz.</p>
  * @note big core: Nehalem and later, small core:
  *
  * @var uint64_t MSR_PLATFORM_INFO::Minimum_Operating_Ratio;
  * <p>Minimum Operating Ratio (RO)</p>
  * <p>Contains the minimum supported operating ratio in
  * units of 100 MHz.</p>
  * @note Ivybridge and Haswell only
  *
  * @var uint64_t MSR_PLATFORM_INFO::reserved_63_56;
  * <p>Reserved bits 63:56</p> */
class MSR_PLATFORM_INFO : public MsrBase<MSR_PLATFORM_INFO, 206> {
  public:
    using MsrBase<MSR_PLATFORM_INFO, 206>::MsrBase;
    int write() = delete; /**< Deleted because this MSR is read-only. */
    union {
      uint64_t value;
      struct {
        uint64_t reserved_7_0 : 8;                            /**< @brief MSR_PLATFORM_INFO bits 7:0 (8) */
        uint64_t Maximum_Non_Turbo_Ratio : 8;                 /**< @brief MSR_PLATFORM_INFO bits 15:8 (8) */
        uint64_t reserved_22_16 : 7;                          /**< @brief MSR_PLATFORM_INFO bits 22:16 (7) */
        uint64_t PPIN_CAP : 1;                                /**< @brief MSR_PLATFORM_INFO bit 23 */
        uint64_t reserved_27_24 : 4;                          /**< @brief MSR_PLATFORM_INFO bits 27:24 (4) */
        uint64_t Programmable_Ratio_Limit_For_Turbo_Mode : 1; /**< @brief MSR_PLATFORM_INFO bit 28 */
        uint64_t Programmable_TDP_Limit_For_Turbo_Mode : 1;   /**< @brief MSR_PLATFORM_INFO bit 29 */
        uint64_t Programmable_TJ_OFFSET : 1;                  /**< @brief MSR_PLATFORM_INFO bit 30 */
        uint64_t reserved_31 : 1;                             /**< @brief MSR_PLATFORM_INFO bit 31 */
        uint64_t Low_Power_Mode_Support : 1;                  /**< @brief MSR_PLATFORM_INFO bit 32 */
        uint64_t Number_Of_ConfigTDP_Levels : 2;              /**< @brief MSR_PLATFORM_INFO bits 34:33 (2) */
        uint64_t reserved_39_35 : 5;                          /**< @brief MSR_PLATFORM_INFO bits 39:35 (5) */
        uint64_t Maximum_Efficiency_Ratio : 8;                /**< @brief MSR_PLATFORM_INFO bits 47:40 (8) */
        uint64_t Minimum_Operating_Ratio : 8;                 /**< @brief MSR_PLATFORM_INFO bits 55:48 (8) */
        uint64_t reserved_63_56 : 8;                          /**< @brief MSR_PLATFORM_INFO bits 63:56 (8) */
      };
    };
};

/** @class MSR_PKG_CST_CONFIG_CONTROL
  * @brief MSR 0xE2 : C-State Configuration Control
  * @note Scope: Package (one MSR for all cores in the package)
  * @note C-state values are processor specific C-state
  * code names, unrelated to MWAIT extension C-state
  * parameters or ACPI C-states. See http://biosbits.org.
  *
  * @var uint64_t MSR_PKG_CST_CONFIG_CONTROL::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t MSR_PKG_CST_CONFIG_CONTROL::Package_C_State_Limit;
  * <p>Package C-State Limit (R/W)</p>
  * <p>Specifies the lowest processor-specific C-state code
  * name (consuming the least power) for the package.<br/>
  * The default is set as factory-configured package C-state limit.</p>
  * <table>
  * <tr><th>Encoding</th><th>Name</th><th>Remarks</th></tr>
  * <tr><td>0000b</td><td>CO/C1</td><td>No package C-state support</td></tr>
  * <tr><td>0001b</td><td>C2</td><td>Haswell+</td></tr>
  * <tr><td>0010b</td><td>C3</td><td>Haswell+</td></tr>
  * <tr><td>0011b</td><td>C6</td><td>Haswell+</td></tr>
  * <tr><td>0100b</td><td>C7</td><td>Haswell+</td></tr>
  * <tr><td>0101b</td><td>C7s</td><td>Haswell+</td></tr>
  * <tr><td>0110b</td><td>C8</td><td>Haswell 06_45H</td></tr>
  * <tr><td>0111b</td><td>C9</td><td>Haswell 06_45H</td></tr>
  * <tr><td>1000b</td><td>C10</td><td>Haswell 06_45H</td></tr>
  * <tr><th colspan="3">Supported C-State encodings.</th></tr>
  * </table>
  * @note Package C-states C7 and higher are not available to processors
  * with a signature of 06_3CH.
  *
  * @var uint64_t MSR_PKG_CST_CONFIG_CONTROL::reserved_9_4;
  * <p>Reserved bits 9:4</p>
  *
  * @var uint64_t MSR_PKG_CST_CONFIG_CONTROL::IO_MWAIT_Redirection_Enable;
  *
  * @var uint64_t MSR_PKG_CST_CONFIG_CONTROL::reserved_14_11;
  * <p>Reserved bits 14:11</p>
  *
  * @var uint64_t MSR_PKG_CST_CONFIG_CONTROL::CFG_Lock;
  *
  * @var uint64_t MSR_PKG_CST_CONFIG_CONTROL::reserved_24_16;
  * <p>Reserved bits 24:16</p>
  *
  * @var uint64_t MSR_PKG_CST_CONFIG_CONTROL::C3_State_Auto_Demotion_Enable;
  *
  * @var uint64_t MSR_PKG_CST_CONFIG_CONTROL::C1_State_Auto_Demotion_Enable;
  *
  * @var uint64_t MSR_PKG_CST_CONFIG_CONTROL::Enable_C3_Undemotion;
  *
  * @var uint64_t MSR_PKG_CST_CONFIG_CONTROL::Enable_C1_Undemotion;
  *
  * @var uint64_t MSR_PKG_CST_CONFIG_CONTROL::reserved_63_29;
  * <p>Reserved bits 63:29</p> */
class MSR_PKG_CST_CONFIG_CONTROL : public MsrBase<MSR_PKG_CST_CONFIG_CONTROL, 226> {
  public:
    using MsrBase<MSR_PKG_CST_CONFIG_CONTROL, 226>::MsrBase;
    union {
      uint64_t value;
      struct {
        uint64_t Package_C_State_Limit : 4;         /**< @brief MSR_PKG_CST_CONFIG_CONTROL bits 3:0 (4) */
        uint64_t reserved_9_4 : 6;                  /**< @brief MSR_PKG_CST_CONFIG_CONTROL bits 9:4 (6) */
        uint64_t IO_MWAIT_Redirection_Enable : 1;   /**< @brief MSR_PKG_CST_CONFIG_CONTROL bit 10 */
        uint64_t reserved_14_11 : 4;                /**< @brief MSR_PKG_CST_CONFIG_CONTROL bits 14:11 (4) */
        uint64_t CFG_Lock : 1;                      /**< @brief MSR_PKG_CST_CONFIG_CONTROL bit 15 */
        uint64_t reserved_24_16 : 9;                /**< @brief MSR_PKG_CST_CONFIG_CONTROL bits 24:16 (9) */
        uint64_t C3_State_Auto_Demotion_Enable : 1; /**< @brief MSR_PKG_CST_CONFIG_CONTROL bit 25 */
        uint64_t C1_State_Auto_Demotion_Enable : 1; /**< @brief MSR_PKG_CST_CONFIG_CONTROL bit 26 */
        uint64_t Enable_C3_Undemotion : 1;          /**< @brief MSR_PKG_CST_CONFIG_CONTROL bit 27 */
        uint64_t Enable_C1_Undemotion : 1;          /**< @brief MSR_PKG_CST_CONFIG_CONTROL bit 28 */
        uint64_t reserved_63_29 : 35;               /**< @brief MSR_PKG_CST_CONFIG_CONTROL bits 63:29 (35) */
      };
    };
};

/** @class IA32_THERM_INTERRUPT
  * @brief MSR 0x19B : Thermal Interrupt Control (RW)
  * @note Scope: Core (one MSR for each core in the package)
  * @note If CPUID.01H:EDX[22] = 1 (ACPI feature flag)
  * <p>Enables and disables the generation of an
  * interrupt on temperature transitions
  * detected with the processor's thermal
  * sensors and thermal monitor.<br/>
  * See Section 14.7.2, “Thermal Monitor.”</p>
  *
  * @var uint64_t IA32_THERM_INTERRUPT::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t IA32_THERM_INTERRUPT::High_Temperature_Interrupt_Enable;
  * <p>High-Temperature Interrupt Enable</p>
  * @note If CPUID.O1H:EDX[22] = 1 (ACPI feature flag)
  *
  * @var uint64_t IA32_THERM_INTERRUPT::Low_Temperature_Interrupt_Enable;
  * <p>Low-Temperature Interrupt Enable</p>
  * @note If CPUID.O1H:EDX[22] = 1 (ACPI feature flag)
  *
  * @var uint64_t IA32_THERM_INTERRUPT::PROCHOT_Interrupt_Enable;
  * <p>PROCHOT# Interrupt Enable</p>
  * @note If CPUID.O1H:EDX[22] = 1 (ACPI feature flag)
  *
  * @var uint64_t IA32_THERM_INTERRUPT::FORCEPR_Interrupt_Enable;
  * <p>FORCEPR# Interrupt Enable</p>
  * @note If CPUID.01H:EDX[22] = 1 (ACPI feature flag)
  *
  * @var uint64_t IA32_THERM_INTERRUPT::Critical_Temperature_Interrupt_Enable;
  * <p>Critical Temperature Interrupt Enable</p>
  * @note If CPUID.01H:EDX[22] = 1 (ACPI feature flag)
  *
  * @var uint64_t IA32_THERM_INTERRUPT::reserved_7_5;
  * <p>Reserved bits 7:5</p>
  *
  * @var uint64_t IA32_THERM_INTERRUPT::Threshold_1_Value;
  * <p>Threshold #1 Value</p>
  * @note If CPUID.01H:EDX[22] = 1 (ACPI feature flag)
  *
  * @var uint64_t IA32_THERM_INTERRUPT::Threshold_1_Interrupt_Enable;
  * <p>Threshold #1 Interrupt Enable</p>
  * @note If CPUID.01H:EDX[22] = 1 (ACPI feature flag)
  *
  * @var uint64_t IA32_THERM_INTERRUPT::Threshold_2_Value;
  * <p>Threshold #2 Value</p>
  * @note If CPUID.01H:EDX[22] = 1 (ACPI feature flag)
  *
  * @var uint64_t IA32_THERM_INTERRUPT::Threshold_2_Interrupt_Enable;
  * <p>Threshold #2 Interrupt Enable</p>
  * @note If CPUID.01H:EDX[22] = 1 (ACPI feature flag)
  *
  * @var uint64_t IA32_THERM_INTERRUPT::Power_Limit_Notification_Enable;
  * <p>Power Limit Notification Enable</p>
  * @note If CPUID.06H:EAX[4] = 1
  *
  * @var uint64_t IA32_THERM_INTERRUPT::reserved_63_25;
  * <p>Reserved bit 63:25</p> */
class IA32_THERM_INTERRUPT : public MsrBase<IA32_THERM_INTERRUPT, 411> {
  public:
    using MsrBase<IA32_THERM_INTERRUPT, 411>::MsrBase;
    union {
      uint64_t value;
      struct {
        uint64_t High_Temperature_Interrupt_Enable : 1;     /**< @brief IA32_THERM_INTERRUPT bit 0 */
        uint64_t Low_Temperature_Interrupt_Enable : 1;      /**< @brief IA32_THERM_INTERRUPT bit 1 */
        uint64_t PROCHOT_Interrupt_Enable : 1;              /**< @brief IA32_THERM_INTERRUPT bit 2 */
        uint64_t FORCEPR_Interrupt_Enable : 1;              /**< @brief IA32_THERM_INTERRUPT bit 3 */
        uint64_t Critical_Temperature_Interrupt_Enable : 1; /**< @brief IA32_THERM_INTERRUPT bit 4 */
        uint64_t reserved_7_5 : 3;                          /**< @brief IA32_THERM_INTERRUPT bits 7:5 (3) */
        uint64_t Threshold_1_Value : 7;                     /**< @brief IA32_THERM_INTERRUPT bits 14:8 (7) */
        uint64_t Threshold_1_Interrupt_Enable : 1;          /**< @brief IA32_THERM_INTERRUPT bit 15 */
        uint64_t Threshold_2_Value : 7;                     /**< @brief IA32_THERM_INTERRUPT bits 22:16 (7) */
        uint64_t Threshold_2_Interrupt_Enable : 1;          /**< @brief IA32_THERM_INTERRUPT bit 23 */
        uint64_t Power_Limit_Notification_Enable : 1;       /**< @brief IA32_THERM_INTERRUPT bit 24 */
        uint64_t reserved_63_25 : 39;                       /**< @brief IA32_THERM_INTERRUPT bits 63:25 (39) */
      };
    };
};

/** @class IA32_THERM_STATUS
  * @brief Thermal Status Information (RO)
  * <p>Contains status information about the processor's thermal sensor and automatic
  * thermal monitoring facilities.<br/>See Section 14.72, “Thermal Monitor”</p>
  * @note Scope: Core (one MSR for each core in the package)
  * @note If CPUID.01H:EDX[22] = 1 (ACPI feature flag)
  *
  * @var uint64_t IA32_THERM_STATUS::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t IA32_THERM_STATUS::Thermal_Status;
  * <p>Thermal Status (RO)</p>
  * @note If CPUID.01H:EDX[22] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Thermal_Status_Log;
  * <p>Thermal Status Log (R/W)</p>
  * @note If CPUID.01H:EDX[22] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::PROCHOT_Or_FORCEPR_Event;
  * <p>PROCHOT# or FORCEPR# event (RO)</p>
  * @note If CPUID.01H:EDX[22] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::PROCHOT_Or_FORCEPR_Log;
  * <p>PROCHOT# or FORCEPRH# log (R/WCO)</p>
  * @note If CPUID.01H:EDX[22] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Critical_Temperature_Status;
  * <p>Critical Temperature Status (RO)</p>
  * @note If CPUID.01H:EDX[22] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Critical_Temperature_Status_Log;
  * <p>Critical Temperature Status log (R/WCO)</p>
  * @note If CPUID.01H:EDX[22] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Thermal_Threshold_1_Status;
  * <p>Thermal Threshold #1 Status (RO)</p>
  * @note If CPUID.01H:ECX[8] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Thermal_Threshold_1_Log;
  * <p>Thermal Threshold #1 log (R/WCO)</p>
  * @note If CPUID.01H:ECX[8] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Thermal_Threshold_2_Status;
  * <p>Thermal Threshold #2 Status (RO)</p>
  * @note If CPUID.01H:ECX[8] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Thermal_Threshold_2_Log;
  * <p>Thermal Threshold #2 log (R/WCO)</p>
  * @note If CPUID.01H:ECX[8] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Power_Limitation_Status;
  * <p>Power Limitation Status (RO)</p>
  * @note If CPUID.06H:EAX[4] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Power_Limitation_Log;
  * <p>Power Limitation log (R/WCO)</p>
  * @note If CPUID.06H:EAX[4] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Current_Limit_Status;
  * <p>Current Limit Status (RO)</p>
  * @note If CPUID.06H:EAX[7] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Current_Limit_Log;
  * <p>Current Limit log (R/WCO)</p>
  * @note If CPUID.06H:EAX[7] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Cross_Domain_Limit_Status;
  * <p>Cross Domain Limit Status (RO)</p>
  * @note If CPUID.06H:EAX[7] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Cross_Domain_Limit_Log;
  * <p>Cross Domain Limit log (R/WCO)</p>
  * @note If CPUID.06H:EAX[7] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Digital_Readout;
  * <p>Digital Readout (RO)</p>
  * @note If CPUID.06H:EAX[0] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::reserved_26_23;
  * <p>Reserved bits 26:23</p>
  *
  * @var uint64_t IA32_THERM_STATUS::Resolution_In_Degrees_Celsius;
  * <p>Resolution in Degrees Celsius (RO)</p>
  * @note If CPUID.06H:EAX[0] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::Reading_Valid;
  * <p>Reading Valid (RO)</p>
  * @note If CPUID.06H:EAX[0] = 1
  *
  * @var uint64_t IA32_THERM_STATUS::reserved_63_32;
  * <p>Reserved bits 63:32</p> */
class IA32_THERM_STATUS : public MsrBase<IA32_THERM_STATUS, 412> {
  public:
    using MsrBase<IA32_THERM_STATUS, 412>::MsrBase;
    union {
      uint64_t value;
      struct {
        uint64_t Thermal_Status : 1;                  /**< @brief IA32_THERM_STATUS bit 0 */
        uint64_t Thermal_Status_Log : 1;              /**< @brief IA32_THERM_STATUS bit 1 */
        uint64_t PROCHOT_Or_FORCEPR_Event : 1;        /**< @brief IA32_THERM_STATUS bit 2 */
        uint64_t PROCHOT_Or_FORCEPR_Log : 1;          /**< @brief IA32_THERM_STATUS bit 3 */
        uint64_t Critical_Temperature_Status : 1;     /**< @brief IA32_THERM_STATUS bit 4 */
        uint64_t Critical_Temperature_Status_Log : 1; /**< @brief IA32_THERM_STATUS bit 5 */
        uint64_t Thermal_Threshold_1_Status : 1;      /**< @brief IA32_THERM_STATUS bit 6 */
        uint64_t Thermal_Threshold_1_Log : 1;         /**< @brief IA32_THERM_STATUS bit 7 */
        uint64_t Thermal_Threshold_2_Status : 1;      /**< @brief IA32_THERM_STATUS bit 8 */
        uint64_t Thermal_Threshold_2_Log : 1;         /**< @brief IA32_THERM_STATUS bit 9 */
        uint64_t Power_Limitation_Status : 1;         /**< @brief IA32_THERM_STATUS bit 10 */
        uint64_t Power_Limitation_Log : 1;            /**< @brief IA32_THERM_STATUS bit 11 */
        uint64_t Current_Limit_Status : 1;            /**< @brief IA32_THERM_STATUS bit 12 */
        uint64_t Current_Limit_Log : 1;               /**< @brief IA32_THERM_STATUS bit 13 */
        uint64_t Cross_Domain_Limit_Status : 1;       /**< @brief IA32_THERM_STATUS bit 14 */
        uint64_t Cross_Domain_Limit_Log : 1;          /**< @brief IA32_THERM_STATUS bit 15 */
        uint64_t Digital_Readout : 7;                 /**< @brief IA32_THERM_STATUS bits 22:16 (7) */
        uint64_t reserved_26_23 : 4;                  /**< @brief IA32_THERM_STATUS bits 26:23 (4) */
        uint64_t Resolution_In_Degrees_Celsius : 4;   /**< @brief IA32_THERM_STATUS bits 30:27 (4) */
        uint64_t Reading_Valid : 1;                   /**< @brief IA32_THERM_STATUS bit 31 */
        uint64_t reserved_63_32 : 32;                 /**< @brief IA32_THERM_STATUS bits 63:32 (32) */
      };
    };
};

/** @class IA32_CLOCK_MODULATION
  * @brief MSR 0x19A : Clock Modulation Control (RW)
  * @see Intel SDM Section 14.73, “Software Controlled Clock Modulation.”
  * @note If CPUID.O1H:EDX[22] = 1 (acpi feature-flag)
  * @note Scope: Core (one MSR for each core in the package)
  *
  * @var uint64_t IA32_CLOCK_MODULATION::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t IA32_CLOCK_MODULATION::Extended_OnDemand_ClockModulation_DutyCycle;
  * <p>Extended On-Demand Clock Modulation Duty Cycle (RW)</p>
  * @note If CPUID.O6H:EAX[5] = 1 (ECMD feature-flag)
  *
  * @var uint64_t IA32_CLOCK_MODULATION::OnDemand_ClockModulation_DutyCycle;
  * <p>On-Demand Clock Modulation Duty Cycle (RW)</p>
  * <p>Specific encoded values for target duty cycle modulation.</p>
  * @note If CPUID.O1H:EDX[22] = 1 (acpi feature-flag)
  *
  * @var uint64_t IA32_CLOCK_MODULATION::OnDemand_ClockModulation_Enable;
  * <p>On-Demand Clock Modulation Enable (RW)</p>
  * <p>Set 1 to enable modulation, 0 to disable.</p>
  * @note If CPUID.O1H:EDX[22] = 1 (acpi feature-flag)
  *
  * @var uint64_t IA32_CLOCK_MODULATION::reserved_63_5;
  * <p>Reserved bits 63:5 (59)</p> */
class IA32_CLOCK_MODULATION : public MsrBase<IA32_CLOCK_MODULATION, 410> {
  public:
    using MsrBase<IA32_CLOCK_MODULATION, 410>::MsrBase;
    union {
      uint64_t value;
      struct {
        uint64_t Extended_OnDemand_ClockModulation_DutyCycle : 1; /**< @brief IA32_CLOCK_MODULATION bit 0 */
        uint64_t OnDemand_ClockModulation_DutyCycle : 3;          /**< @brief IA32_CLOCK_MODULATION bits 3:1 (3) */
        uint64_t OnDemand_ClockModulation_Enable : 1;             /**< @brief IA32_CLOCK_MODULATION bit 4 */
        uint64_t reserved_63_5 : 59;                              /**< @brief IA32_CLOCK_MODULATION bits 63:5 (59) */
      };
    };
};

/** @class MSR_THERM2_CTL
  * @brief MSR 0x19D : Thermal Monitor 2 Control (RW)
  * @note Scope: Shared (one MSR for all logical processors)
  *
  * @var uint64_t MSR_THERM2_CTL::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t MSR_THERM2_CTL::TM2_transistion_target;
  * <p>The target operating frequency and voltage for the TM2 transition after TM2
  * is triggered is specified by the value written to MSR_THERM2_CTL, bits 15:0</p>
  *
  * @var uint64_t MSR_THERM2_CTL::TM_SELECT;
  * <p>Mode of automatic thermal monitor:</p>
  * <p>0 = Thermal Monitor 1 (thermally-initiated on-die
  *     modulation of the stop-clock duty cycle).<br/>
  * 1 = Thermal Monitor 2 (thermally-initiated frequency
  *     transitions).</p>
  * <p>If IA32_MISC_ENABLE::Automatic_Thermal_Control_Circuit_Enable is cleared,
  * TM_SELECT has no effect. Neither TM1 nor TM2 are enabled.</p>
  *
  * @var uint64_t MSR_THERM2_CTL::reserved_63_17;
  * <p>Reserved bit 63:17</p> */
class MSR_THERM2_CTL : public MsrBase<MSR_THERM2_CTL, 413> {
  public:
    using MsrBase<MSR_THERM2_CTL, 413>::MsrBase;
    union {
      uint64_t value;
      struct {
        uint64_t TM2_transistion_target : 16;  /**< @brief MSR_THERM2_CTL bits 15:0 (16) */
        uint64_t TM_SELECT : 1;                /**< @brief MSR_THERM2_CTL bit 16 */
        uint64_t reserved_63_17 : 47;          /**< @brief MSR_THERM2_CTL bits 63:17 (47) */
      };
    };
};

/** @class IA32_MISC_ENABLE
  * @brief MSR 0x1A0 : Enable Misc. Processor Features (RW)
  * @note Scope: Shared (one MSR for all logical processors)<br/><br/>
  * <b>BUT</b> (From Intel SDM volume 3, section 8.7.8):<br/>
  * The IA32_MISC_ENABLE MSR (MSR address 1A0H) is generally shared between the logical
  * processors in a processor core supporting Intel Hyper-Threading Technology.<br/>
  * However, some bit fields within IA32_MISC_ENABLE MSR may be duplicated per logical
  * processor.<br/>
  * The partition of shared or duplicated bit fields within IA32_MISC_ENABLE is
  * implementation dependent.<br/>
  * Software should program duplicated fields carefully on all logical processors
  * in the system to ensure consistent behavior.
  *
  * @var uint64_t IA32_MISC_ENABLE::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t IA32_MISC_ENABLE::Fast_Strings_Enable;
  * <p>Fast-Strings Enable (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::reserved_2_1;
  * <p>Reserved bits 2:1</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::Automatic_Thermal_Control_Circuit_Enable;
  * <p>Automatic Thermal Control Circuit Enable (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::reserved_6_4;
  * <p>Reserved bits 6:4</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::Performance_Monitoring_Available;
  * <p>Performance Monitoring Available (RO)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::reserved_8;
  * <p>Reserved bit 8</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::Hardware_Prefetcher_Disable;
  * <p>Hardware Prefetcher Disable (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::FERR_Multiplexing_Enable;
  * <p>FERR# Multiplexing Enable (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::Branch_Trace_Storage_Unavailable;
  * <p>Branch Trace Storage Unavailable (RO)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::Processor_Event_Based_Sampling;
  * <p>Processor Event Based Sampling (RO)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::TM2_ENABLE;
  * <p>TM2 Enable (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::reserved_15_14;
  * <p>Reserved bits 15:14</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::EIST_Enable;
  * <p>Enhanced Intel SpeedStep Technology Enable (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::reserved_17;
  * <p>Reserved bit 17</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::ENABLE_MONITOR_FSM;
  * <p>ENABLE MONITOR FSM (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::Adjacent_Cache_Line_Prefetch_Disable;
  * <p>Adjacent Cache Line Prefetch Disable (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::EIST_Select_Lock;
  * <p>Enhanced Intel SpeedStep Technology Select Lock (R/WO)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::reserved_21;
  * <p>Reserved bit 21</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::Limit_CPUID_Maxval;
  * <p>Limit CPUID Maxval (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::xTPR_Message_Disable;
  * <p>xTPR Message Disable (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::reserved_33_24;
  * <p>Reserved bits 33:24</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::XD_Bit_Disable;
  * <p>XD Bit Disable (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::reserved_36_35;
  * <p>Reserved bits 36:35</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::DCU_Prefetcher_Disable;
  * <p>DCU Prefetcher Disable (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::IDA_Disable;
  * <p>IDA (Turbo Boost Technology) Disable (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::IP_Prefetcher_Disable;
  * <p>IP Prefetcher Disable (R/W)</p>
  *
  * @var uint64_t IA32_MISC_ENABLE::reserved_63_40;
  * <p>Reserved bits 63:40</p> */
class IA32_MISC_ENABLE : public MsrBase<IA32_MISC_ENABLE, 416> {
  public:
    using MsrBase<IA32_MISC_ENABLE, 416>::MsrBase;
    union {
      uint64_t value;
      struct {
        uint64_t Fast_Strings_Enable : 1;              /**< @brief IA32_MISC_ENABLE bit 0 */
        uint64_t reserved_2_1 : 2;                     /**< @brief IA32_MISC_ENABLE bits 2:1 (2) */
        uint64_t Automatic_Thermal_Control_Circuit_Enable : 1; /**< @brief IA32_MISC_ENABLE bit 3 */
        uint64_t reserved_6_4 : 3;                     /**< @brief IA32_MISC_ENABLE bits 6:4 (3) */
        uint64_t Performance_Monitoring_Available : 1; /**< @brief IA32_MISC_ENABLE bit 7 */
        uint64_t reserved_8 : 1;                       /**< @brief IA32_MISC_ENABLE bit 8 */
        uint64_t Hardware_Prefetcher_Disable : 1;      /**< @brief IA32_MISC_ENABLE bit 9 */
        uint64_t FERR_Multiplexing_Enable : 1;         /**< @brief IA32_MISC_ENABLE bit 10 */
        uint64_t Branch_Trace_Storage_Unavailable : 1; /**< @brief IA32_MISC_ENABLE bit 11 */
        uint64_t Processor_Event_Based_Sampling : 1;   /**< @brief IA32_MISC_ENABLE bit 12 */
        uint64_t TM2_ENABLE : 1;                       /**< @brief IA32_MISC_ENABLE bit 13 */
        uint64_t reserved_15_14 : 2;                   /**< @brief IA32_MISC_ENABLE bits 15:14 (2) */
        uint64_t EIST_Enable : 1;                      /**< @brief IA32_MISC_ENABLE bit 16 */
        uint64_t reserved_17 : 1;                      /**< @brief IA32_MISC_ENABLE bit 17 */
        uint64_t ENABLE_MONITOR_FSM : 1;               /**< @brief IA32_MISC_ENABLE bit 18 */
        uint64_t Adjacent_Cache_Line_Prefetch_Disable : 1; /**< @brief IA32_MISC_ENABLE bit 19 */
        uint64_t EIST_Select_Lock : 1;                 /**< @brief IA32_MISC_ENABLE bit 20 */
        uint64_t reserved_21 : 1;                      /**< @brief IA32_MISC_ENABLE bit 21 */
        uint64_t Limit_CPUID_Maxval : 1;               /**< @brief IA32_MISC_ENABLE bit 22 */
        uint64_t xTPR_Message_Disable : 1;             /**< @brief IA32_MISC_ENABLE bit 23 */
        uint64_t reserved_33_24 : 10;                  /**< @brief IA32_MISC_ENABLE bits 33:24 (10) */
        uint64_t XD_Bit_Disable : 1;                   /**< @brief IA32_MISC_ENABLE bit 34 */
        uint64_t reserved_36_35 : 2;                   /**< @brief IA32_MISC_ENABLE bits 36:35 (2) */
        uint64_t DCU_Prefetcher_Disable : 1;           /**< @brief IA32_MISC_ENABLE bit 37 */
        uint64_t IDA_Disable : 1;                      /**< @brief IA32_MISC_ENABLE bit 38 */
        uint64_t IP_Prefetcher_Disable : 1;            /**< @brief IA32_MISC_ENABLE bit 39 */
        uint64_t reserved_63_40 : 24;                  /**< @brief IA32_MISC_ENABLE bits 63:40 (24) */
      };
    };
};

/** @class MSR_TEMPERATURE_TARGET
  * @brief MSR 0x1A2 : Temperature Target (RW)
  * @note Scope: Package (one MSR for all cores in the package)
  *
  * @var uint64_t MSR_TEMPERATURE_TARGET::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t MSR_TEMPERATURE_TARGET::reserved_15_0;
  * <p>Reserved bits 15:0</p>
  *
  * @var uint64_t MSR_TEMPERATURE_TARGET::Temperature_Target;
  * <p>The default thermal throttling or PROCHOT# activation
  * temperature in degrees C. The effective temperature
  * for thermal throttling or PROCHOT# activation is
  * “Temperature Target’ + “Target Offset”.</p>
  *
  * @var uint64_t MSR_TEMPERATURE_TARGET::Target_Offset_27_24;
  * <p>Specifies a temperature offset in degrees C from the
  * temperature target (bits 23:16). PROCHOT# will assert
  * at the offset target temperature. Write is permitted
  * only if MSR_PLATFORM_INFO.[30] is set.</p>
  * @note 'big-core' only, IvyBridge-E (06_3EH) and later.
  *
  * @var uint64_t MSR_TEMPERATURE_TARGET::Target_Offset_29_24;
  * <p>Specifies a temperature offset in degrees C from the
  * temperature target (bits 23:16). PROCHOT# will assert
  * at the offset target temperature.</p>
  * @note Xeon Phi and 'small-core' only (Silvermont and later).
  *
  * @var uint64_t MSR_TEMPERATURE_TARGET::reserved_63_28;
  * <p>Reserved bits 63:28</p>
  *
  * @var uint64_t MSR_TEMPERATURE_TARGET::reserved_63_30;
  * <p>Reserved bits 63:30</p> */
class MSR_TEMPERATURE_TARGET : public MsrBase<MSR_TEMPERATURE_TARGET, 418> {
  public:
    using MsrBase<MSR_TEMPERATURE_TARGET, 418>::MsrBase;
    union {
      uint64_t value;
      struct {
        uint64_t reserved_15_0 : 16;      /**< @brief MSR_TEMPERATURE_TARGET bits 15:0 (16) */
        uint64_t Temperature_Target : 8;  /**< @brief MSR_TEMPERATURE_TARGET bit 23:16 (8) */
        uint64_t Target_Offset_27_24 : 4; /**< @brief MSR_TEMPERATURE_TARGET bits 27:24 (4) */
        uint64_t reserved_63_28 : 36;     /**< @brief MSR_TEMPERATURE_TARGET bits 63:28 (36) */
      };
      struct {
        uint64_t reserved_23_0 : 24;      /**< @brief bits 23:0 (24) */
        uint64_t Target_Offset_29_24 : 6; /**< @brief MSR_TEMPERATURE_TARGET bits 29:24 (6) */
        uint64_t reserved_63_30 : 34;     /**< @brief MSR_TEMPERATURE_TARGET bits 63:30 (34) */
      };
    };
};

/** @class MSR_TURBO_RATIO_LIMIT3
  * @brief MSR 0x1AC : Config Ratio Limit of Turbo Mode (RW)
  * @note Broadwell 06_56H + 06_4FH
  * @note RO if MSR_PLATFORM_INFO::Programmable_Ratio_Limit_For_Turbo_Mode = 0, RW if MSR_PLATFORM_INFO::Programmable_Ratio_Limit_For_Turbo_Mode = 1.
  * @note Scope: Package (one MSR for all cores in the package)
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT3::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT3::reserved;
  * <p>Reserved bits 62:0</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT3::Ratio_Limit_Semaphore;
  * <p> Semaphore for Turbo Ratio Limit Configuration</p>
  * <p>If 1, the processor uses override configuration
  * specified in MSR_TURBO_RATIO_LIMIT, MSR_TURBO_RATIO_LIMIT1.</br>
  * If 0, the processor uses factory-set configuration (Default).</p> */
class MSR_TURBO_RATIO_LIMIT3 : public MsrBase<MSR_TURBO_RATIO_LIMIT3, 428> {
  public:
    using MsrBase<MSR_TURBO_RATIO_LIMIT3, 428>::MsrBase;
    union {
      uint64_t value;
      struct { 
        uint64_t reserved : 63;             /**< @brief MSR_TURBO_RATIO_LIMIT3 bits 62:0 (63) */
        uint64_t Ratio_Limit_Semaphore : 1; /**< @brief MSR_TURBO_RATIO_LIMIT3 bit 63 */
      };
    };
};

/** @class MSR_TURBO_RATIO_LIMIT
  * @brief MSR 0x1AD : Maximum Ratio Limit of Turbo Mode (cores 1 - 8)
  * @note RO if MSR_PLATFORM_INFO::Programmable_Ratio_Limit_For_Turbo_Mode = 0, RW if MSR_PLATFORM_INFO::Programmable_Ratio_Limit_For_Turbo_Mode = 1.
  * @note Scope: Package (one MSR for all cores in the package)
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT::Ratio_Limit_1C;
  * <p>Maximum turbo ratio limit of 1 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT::Ratio_Limit_2C;
  * <p>Maximum turbo ratio limit of 2 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT::Ratio_Limit_3C;
  * <p>Maximum turbo ratio limit of 3 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT::Ratio_Limit_4C;
  * <p>Maximum turbo ratio limit of 4 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT::Ratio_Limit_5C;
  * <p>Maximum turbo ratio limit of 5 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT::Ratio_Limit_6C;
  * <p>Maximum turbo ratio limit of 6 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT::Ratio_Limit_7C;
  * <p>Maximum turbo ratio limit of 7 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT::Ratio_Limit_8C;
  * <p>Maximum turbo ratio limit of 8 core active.</p> */
class MSR_TURBO_RATIO_LIMIT : public MsrBase<MSR_TURBO_RATIO_LIMIT, 429> {
  public:
    using MsrBase<MSR_TURBO_RATIO_LIMIT, 429>::MsrBase;
    union {
      uint64_t value;
      struct { 
        uint64_t Ratio_Limit_1C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT bits 7:0 (8) */
        uint64_t Ratio_Limit_2C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT bits 15:8 (8) */
        uint64_t Ratio_Limit_3C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT bits 23:16 (8) */
        uint64_t Ratio_Limit_4C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT bits 31:24 (8) */
        uint64_t Ratio_Limit_5C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT bits 39:32 (8) */
        uint64_t Ratio_Limit_6C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT bits 47:40 (8) */
        uint64_t Ratio_Limit_7C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT bits 55:48 (8) */
        uint64_t Ratio_Limit_8C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT bits 63:56 (8) */
      };
    };
};

/** @class MSR_TURBO_RATIO_LIMIT1
  * @brief MSR 0x1AE : Maximum Ratio Limit of Turbo Mode (cores 9 - 16)
  * @note RO if MSR_PLATFORM_INFO::Programmable_Ratio_Limit_For_Turbo_Mode = 0, RW if MSR_PLATFORM_INFO::Programmable_Ratio_Limit_For_Turbo_Mode = 1.
  * @note Scope: Package (one MSR for all cores in the package)
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT1::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT1::Ratio_Limit_9C;
  * <p>Maximum turbo ratio limit of 9 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT1::Ratio_Limit_10C;
  * <p>Maximum turbo ratio limit of 10 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT1::Ratio_Limit_11C;
  * <p>Maximum turbo ratio limit of 11 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT1::Ratio_Limit_12C;
  * <p>Maximum turbo ratio limit of 12 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT1::Ratio_Limit_13C;
  * <p>Maximum turbo ratio limit of 13 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT1::Ratio_Limit_14C;
  * <p>Maximum turbo ratio limit of 14 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT1::Ratio_Limit_15C;
  * <p>Maximum turbo ratio limit of 15 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT1::Ratio_Limit_16C;
  * <p>Maximum turbo ratio limit of 16 core active.</p> */
class MSR_TURBO_RATIO_LIMIT1 : public MsrBase<MSR_TURBO_RATIO_LIMIT1, 430> {
  public:
    using MsrBase<MSR_TURBO_RATIO_LIMIT1, 430>::MsrBase;
    union {
      uint64_t value;
      struct { 
        uint64_t Ratio_Limit_9C : 8;  /**< @brief MSR_TURBO_RATIO_LIMIT1 bits 7:0 (8) */
        uint64_t Ratio_Limit_10C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT1 bits 15:8 (8) */
        uint64_t Ratio_Limit_11C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT1 bits 23:16 (8) */
        uint64_t Ratio_Limit_12C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT1 bits 31:24 (8) */
        uint64_t Ratio_Limit_13C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT1 bits 39:32 (8) */
        uint64_t Ratio_Limit_14C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT1 bits 47:40 (8) */
        uint64_t Ratio_Limit_15C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT1 bits 55:48 (8) */
        uint64_t Ratio_Limit_16C : 8; /**< @brief MSR_TURBO_RATIO_LIMIT1 bits 63:56 (8) */
      };
    };
};

/** @class MSR_TURBO_RATIO_LIMIT2
  * @brief MSR 0x1AF : Maximum Ratio Limit of Turbo Mode (cores 17 - 18)
  * @note RO if MSR_PLATFORM_INFO::Programmable_Ratio_Limit_For_Turbo_Mode = 0, RW if MSR_PLATFORM_INFO::Programmable_Ratio_Limit_For_Turbo_Mode = 1.
  * @note Scope: Package (one MSR for all cores in the package)
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT2::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT2::Ratio_Limit_17C;
  * <p>Maximum turbo ratio limit of 17 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT2::Ratio_Limit_18C;
  * <p>Maximum turbo ratio limit of 18 core active.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT2::reserved
  * <p>Reserved bits 62:16.</p>
  *
  * @var uint64_t MSR_TURBO_RATIO_LIMIT2::Ratio_Limit_Semaphore;
  * <p> Semaphore for Turbo Ratio Limit Configuration</p>
  * <p>If 1, the processor uses override configuration specified in MSR_TURBO_RATIO_LIMIT,
  * MSR_TURBO_RATIO_LIMIT1 and MSR_TURBO_RATIO_LIMIT2.</br>
  * If 0, the processor uses factory-set configuration (Default).</p> */
class MSR_TURBO_RATIO_LIMIT2 : public MsrBase<MSR_TURBO_RATIO_LIMIT2, 431> {
  public:
    using MsrBase<MSR_TURBO_RATIO_LIMIT2, 431>::MsrBase;
    union {
      uint64_t value;
      struct { 
        uint64_t Ratio_Limit_17C : 8;       /**< @brief MSR_TURBO_RATIO_LIMIT2 bits 7:0 (8) */
        uint64_t Ratio_Limit_18C : 8;       /**< @brief MSR_TURBO_RATIO_LIMIT2 bits 15:8 (8) */
        uint64_t reserved : 47;             /**< @brief MSR_TURBO_RATIO_LIMIT2 bits 62:16 (47) */
        uint64_t Ratio_Limit_Semaphore : 1; /**< @brief MSR_TURBO_RATIO_LIMIT2 bit 63 */
      };
    };
};

/** @class IA32_ENERGY_PERF_BIAS
  * @brief MSR 0x1B0 : Performance Energy Bias Hint (R/W)
  * @note Scope: Package (one MSR for all cores in the package)
  * @note If CPUID.06H:ECX[3] = 1
  *
  * @var uint64_t IA32_ENERGY_PERF_BIAS::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t IA32_ENERGY_PERF_BIAS::Power_Policy_Preference;
  * <p>Power Policy Preference:<br/>
  * 0 indicates preference to highest performance.<br/>
  * 15 indicates preference to maximize energy saving.</p>
  *
  * @var uint64_t IA32_ENERGY_PERF_BIAS::reserved_63_4;
  * <p>Reserved bits 63:4</p> */
class IA32_ENERGY_PERF_BIAS : public MsrBase<IA32_ENERGY_PERF_BIAS, 432> {
  public:
    using MsrBase<IA32_ENERGY_PERF_BIAS, 432>::MsrBase;
    union {
      uint64_t value;
      struct {
        uint64_t Power_Policy_Preference : 4; /**< @brief IA32_ENERGY_PERF_BIAS bits 3:0 (4) */
        uint64_t reserved_63_4 : 60;          /**< @brief IA32_ENERGY_PERF_BIAS bits 63:4 (60) */
      };
    };
};

/** @class IA32_PACKAGE_THERM_STATUS
  * @brief MSR 0x1B1 : Package Thermal Status Information (R/WCO)
  * @note Scope: Package (one MSR for all cores in the package)
  * @note If CPUID.06H: EAX[6] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::Thermal_Status;
  * <p>Thermal Status (RO)</p>
  * @note If CPUID.01H:EDX[22] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::Thermal_Status_Log;
  * <p>Thermal Status Log (R/W)</p>
  * @note If CPUID.01H:EDX[22] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::PROCHOT_Event;
  * <p>PROCHOT# or FORCEPR# event (RO)</p>
  * @note If CPUID.01H:EDX[22] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::PROCHOT_Log;
  * <p>PROCHOT# or FORCEPRH# log (R/WCO)</p>
  * @note If CPUID.01H:EDX[22] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::Critical_Temperature_Status;
  * <p>Critical Temperature Status (RO)</p>
  * @note If CPUID.01H:EDX[22] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::Critical_Temperature_Status_Log;
  * <p>Critical Temperature Status log (R/WCO)</p>
  * @note If CPUID.01H:EDX[22] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::Thermal_Threshold_1_Status;
  * <p>Thermal Threshold #1 Status (RO)</p>
  * @note If CPUID.01H:ECX[8] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::Thermal_Threshold_1_Log;
  * <p>Thermal Threshold #1 log (R/WCO)</p>
  * @note If CPUID.01H:ECX[8] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::Thermal_Threshold_2_Status;
  * <p>Thermal Threshold #2 Status (RO)</p>
  * @note If CPUID.01H:ECX[8] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::Thermal_Threshold_2_Log;
  * <p>Thermal Threshold #2 log (R/WCO)</p>
  * @note If CPUID.01H:ECX[8] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::Power_Limitation_Status;
  * <p>Power Limitation Status (RO)</p>
  * @note If CPUID.06H:EAX[4] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::Power_Limitation_Log;
  * <p>Power Limitation log (R/WCO)</p>
  * @note If CPUID.06H:EAX[4] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::reserved_15_12;
  * <p>Reserved bits 15:12</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::Digital_Readout;
  * <p>Digital Readout (RO)</p>
  * @note If CPUID.06H:EAX[0] = 1
  *
  * @var uint64_t IA32_PACKAGE_THERM_STATUS::reserved_63_23;
  * <p>Reserved bits 63:23</p> */
class IA32_PACKAGE_THERM_STATUS : public MsrBase<IA32_PACKAGE_THERM_STATUS, 433> {
  public:
    using MsrBase<IA32_PACKAGE_THERM_STATUS, 433>::MsrBase;
    union {
      uint64_t value;
      struct {
        uint64_t Thermal_Status : 1;                  /**< @brief IA32_PACKAGE_THERM_STATUS bit 0 */
        uint64_t Thermal_Status_Log : 1;              /**< @brief IA32_PACKAGE_THERM_STATUS bit 1 */
        uint64_t PROCHOT_Event : 1;                   /**< @brief IA32_PACKAGE_THERM_STATUS bit 2 */
        uint64_t PROCHOT_Log : 1;                     /**< @brief IA32_PACKAGE_THERM_STATUS bit 3 */
        uint64_t Critical_Temperature_Status : 1;     /**< @brief IA32_PACKAGE_THERM_STATUS bit 4 */
        uint64_t Critical_Temperature_Status_Log : 1; /**< @brief IA32_PACKAGE_THERM_STATUS bit 5 */
        uint64_t Thermal_Threshold_1_Status : 1;      /**< @brief IA32_PACKAGE_THERM_STATUS bit 6 */
        uint64_t Thermal_Threshold_1_Log : 1;         /**< @brief IA32_PACKAGE_THERM_STATUS bit 7 */
        uint64_t Thermal_Threshold_2_Status : 1;      /**< @brief IA32_PACKAGE_THERM_STATUS bit 8 */
        uint64_t Thermal_Threshold_2_Log : 1;         /**< @brief IA32_PACKAGE_THERM_STATUS bit 9 */
        uint64_t Power_Limitation_Status : 1;         /**< @brief IA32_PACKAGE_THERM_STATUS bit 10 */
        uint64_t Power_Limitation_Log : 1;            /**< @brief IA32_PACKAGE_THERM_STATUS bit 11 */
        uint64_t reserved_15_12 : 4;                  /**< @brief IA32_PACKAGE_THERM_STATUS 15:12 (4) */
        uint64_t Digital_Readout : 7;                 /**< @brief IA32_PACKAGE_THERM_STATUS bits 22:16 (7) */
        uint64_t reserved_63_23 : 21;                 /**< @brief IA32_PACKAGE_THERM_STATUS bits */
      };
    };
};

/** @class IA32_PACKAGE_THERM_INTERRUPT
  * @brief MSR 0x1B2 : Package Thermal Interrupt Control (RW)
  * @note Scope: Package (one MSR for all cores in the package)
  * @note If CPUID.06H:EAX[6] = 1
  * <p>Enables and disables the generation of an
  * interrupt on temperature transitions
  * detected with the processor's thermal
  * sensors and thermal monitor.<br/>
  * See Section 14.8, “Package Level Thermal Management.”</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::High_Temperature_Interrupt_Enable;
  * <p>High-Temperature Interrupt Enable</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::Low_Temperature_Interrupt_Enable;
  * <p>Low-Temperature Interrupt Enable</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::PROCHOT_Interrupt_Enable;
  * <p>PROCHOT# Interrupt Enable</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::reserved_3;
  * <p>Reserved bit 3</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::Overheat_Interrupt_Enable;
  * <p>Package Overheat Interrupt Enable</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::reserved_7_5;
  * <p>Reserved bits 7:5</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::Threshold_1_Value;
  * <p>Threshold #1 Value</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::Threshold_1_Interrupt_Enable;
  * <p>Threshold #1 Interrupt Enable</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::Threshold_2_Value;
  * <p>Threshold #2 Value</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::Threshold_2_Interrupt_Enable;
  * <p>Threshold #2 Interrupt Enable</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::Power_Limit_Notification_Enable;
  * <p>Power Limit Notification Enable</p>
  *
  * @var uint64_t IA32_PACKAGE_THERM_INTERRUPT::reserved_63_25;
  * <p>Reserved bit 63:25</p> */
class IA32_PACKAGE_THERM_INTERRUPT : public MsrBase<IA32_PACKAGE_THERM_INTERRUPT, 434> {
  public:
    using MsrBase<IA32_PACKAGE_THERM_INTERRUPT, 434>::MsrBase;
    union {
      uint64_t value;
      struct {
        uint64_t High_Temperature_Interrupt_Enable : 1;     /**< @brief IA32_PACKAGE_THERM_INTERRUPT bit 0 */
        uint64_t Low_Temperature_Interrupt_Enable : 1;      /**< @brief IA32_PACKAGE_THERM_INTERRUPT bit 1 */
        uint64_t PROCHOT_Interrupt_Enable : 1;              /**< @brief IA32_PACKAGE_THERM_INTERRUPT bit 2 */
        uint64_t reserved_3 : 1;                            /**< @brief IA32_PACKAGE_THERM_INTERRUPT bit 3 */
        uint64_t Overheat_Interrupt_Enable : 1;             /**< @brief IA32_PACKAGE_THERM_INTERRUPT bit 4 */
        uint64_t reserved_7_5 : 3;                          /**< @brief IA32_PACKAGE_THERM_INTERRUPT bits 7:5 (3) */
        uint64_t Threshold_1_Value : 7;                     /**< @brief IA32_PACKAGE_THERM_INTERRUPT bits 14:8 (7) */
        uint64_t Threshold_1_Interrupt_Enable : 1;          /**< @brief IA32_PACKAGE_THERM_INTERRUPT bit 15 */
        uint64_t Threshold_2_Value : 7;                     /**< @brief IA32_PACKAGE_THERM_INTERRUPT bits 22:16 (7) */
        uint64_t Threshold_2_Interrupt_Enable : 1;          /**< @brief IA32_PACKAGE_THERM_INTERRUPT bit 23 */
        uint64_t Power_Limit_Notification_Enable : 1;       /**< @brief IA32_PACKAGE_THERM_INTERRUPT bit 24 */
        uint64_t reserved_63_25 : 39;                       /**< @brief IA32_PACKAGE_THERM_INTERRUPT bits 63:25 (39) */
      };
    };
};

/** @class MSR_TURBO_ACTIVATION_RATIO
  * @brief MSR 0x64C : Turbo Activation Ratio Control (RW)
  * @note Scope: Package (one MSR for all cores in the package)
  *
  * @var uint64_t MSR_TURBO_ACTIVATION_RATIO::value;
  * @brief The MSR contents as whole value, bits 63:0 
  *
  * @var uint64_t MSR_TURBO_ACTIVATION_RATIO::MAX_NON_TURBO_RATIO;
  * <p>System BIOS can program this field.</p>
  *
  * @var uint64_t MSR_TURBO_ACTIVATION_RATIO::reserved_30_8;
  * <p>Reserved bits 30:8</p>
  *
  * @var uint64_t MSR_TURBO_ACTIVATION_RATIO::TURBO_ACTIVATION_RATIO_Lock;
  * <p>When this bit is set, the content of this register is locked until a reset.</p>
  *
  * @var uint64_t MSR_TURBO_ACTIVATION_RATIO::reserved_63_32;
  * <p>Reserved bits 63:32</p> */
class MSR_TURBO_ACTIVATION_RATIO : public MsrBase<MSR_TURBO_ACTIVATION_RATIO, 1612> {
  public:
    using MsrBase<MSR_TURBO_ACTIVATION_RATIO, 1612>::MsrBase;
    union {
      uint64_t value;
      struct {
        uint64_t MAX_NON_TURBO_RATIO : 8;         /**< @brief MSR_TURBO_ACTIVATION_RATIO bits 7:0 (8) */
        uint64_t reserved_30_8 : 23;              /**< @brief MSR_TURBO_ACTIVATION_RATIO bits 30:8 (23) */
        uint64_t TURBO_ACTIVATION_RATIO_Lock : 1; /**< @brief MSR_TURBO_ACTIVATION_RATIO bit 31 */
        uint64_t reserved_63_32 : 32;             /**< @brief MSR_TURBO_ACTIVATION_RATIO bits 63:32 (32) */
      };
    };
};

#endif

