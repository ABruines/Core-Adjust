#
# This file is part of 'Core Adjust'.
#
# Core Adjust - Adjust various settings of Intel Processors.
# Copyright (C) 2020, Alexander Bruines <alexander.bruines@gmail.com>
#
# Core Adjust is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Core Adjust is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Core Adjust. If not, see <https://www.gnu.org/licenses/>.
#
##############################################################################
#
# !!! Bash 4.3 does not support 'local -n' with arrays !!!
#
# Or better stated, writing to a referenced variable does not
# work as expected when that variable is the indice of an array.
#
# I first used:
#
#  func() {
#    local -n REF=$1
#    REF="hello"
#  }
#
#  local -a ARRAY
#  func ARRAY[indice]
#
# This works fine on bash 5.x, but not on 4.3.
# Work arround is to call func with normal variable and
# copying the value to the array afterwards, like:
#
# local -a ARRAY
# local X
# func X
# ARRAY[indice]=$X
#
##############################################################################
# Required Debian packages to run this script:
#
# Executable                 Package
#-------------------------------------------
# bash:                      bash
# cat, id, seq:              coreutils
# modprobe:                  kmod
# on_ac_power:               powermgmt-base
# logger:                    bsdutils
# cpufreq-info, cpufreq-set: cpufrequtils
# bc:                        bc
# cpuid:                     cpuid
# rdmsr, wrmsr:              msr-tools
# irqbalance:                irqbalance
#
##############################################################################
# External files (directly) accessed by this script:
#
# /dev/cpu/0/msr
# /dev/null
# /proc/cpuinfo
# /sys/devices/system/cpu/cpu*/online
# /sys/devices/system/cpu/cpu*/topology/core_id
# /sys/devices/system/cpu/present
# /sys/devices/system/cpu/rescan
# /sys/devices/system/cpu/smt 
# /sys/devices/system/cpu/smt/active
# /sys/devices/system/cpu/smt/control
#
##############################################################################
#
Main() {
  #
  # Note: every 'local' variable in Main() is not only available in Main but
  # also from all the functions it calls...
  #
  local version="@PROJECT_VERSION@"
  local configuration_file="@CONFIG_FILE@"
#  local configuration_file="./core-adjust.ini"
  #
  # These are initialised by 'ParseCommandLineArguments':
  #
  local -i isCpu=0
  local -i isVerbose=0
  local -i isForce=0
  local -i isDryRun=0
  local -i haveProcessor=0
  local +i force_march=""
  local -i doPrintHelp=0
  local -i doPrintVersion=0
  local -i doListProcessors=0
  local -i doCpuid=0
  local -i doBootOrResume=0
  local -i doPower=0
  local -i doRead=0
  local -i doReadSingleCpu=0
  local -i doFivr=0
  local -i doTargetTemp=0
  local -i doEnableSmt=0
  local -i doDisableSmt=0
  local -i doEnableSmtCpu=0
  local -i doDisableSmtCpu=0
  local -i doHttDetect=0
  local -i doTmSelect=0
  local -i doTm2Enable=0
  local -i doEistEnable=0
  local -i doEistLock=0
  local -i doTbtDisable=0
  local -i doTbtActivationRatio=0
  local -i doTbtActivationRatioLock=0
  local -i doMaxTurboRatio=0
  local -i doFreqScale=0
  local -i doFreqScaleAll=0
#
  local -i doFastStringsEnable=0
  local -i doHwPrefetcherDisable=0
  local -i doFerrMpxEnable=0
  local -i doMonitorFsmEnable=0
  local -i doAdjCacheLinePrefetchDisable=0
  local -i doCpuidMaxvalEnable=0
  local -i doXtprMessageDisable=0
  local -i doXdBitDisable=0
  local -i doDcuPrefetcherDisable=0
  local -i doIpPrefetcherDisable=0
  #
  # These variables are (also) present in the configuration file and are
  # first initialised by 'DefaultCpuConfiguration' and then read from the
  # configuration file by 'ParseConfigurationFile' when nessesary or modified
  # by using commandline arguments.
  #
  # 'allow_overvolting' and '???_enable' variables are either 'true' or 'false'
  # 'voltage_???' and 'target_???' variables hold a numeric value or the text 'NaN'
  #
  # Note: All the arrays will have the same indices after parsing the commandline
  #       and configuration file. ie. Each indice in '${!varname[@]}' represents
  #       a physical cpu number that needs to be programmed.
  #
  # Note 2: The frequency scaling variables (fs_XXX) have indices for every detected
  #         logical cpu.
  #
  # [Common] section
  local apply_on_boot_and_resume
  local apply_on_acpi_power_event
  # [Processor] section
  # FIVR
  local -a allow_overvolting
  local -a voltage_offset_plane_0_enable
  local -a voltage_offset_plane_1_enable
  local -a voltage_offset_plane_2_enable
  local -a voltage_offset_plane_3_enable
  local -a voltage_offset_plane_4_enable
  local -a voltage_offset_plane_5_enable
  local -a voltage_offset_plane_0
  local -a voltage_offset_plane_1
  local -a voltage_offset_plane_2
  local -a voltage_offset_plane_3
  local -a voltage_offset_plane_4
  local -a voltage_offset_plane_5
  # Target Temperature
  local -a target_temperature_enable
  local -a target_temperature_battery_enable
  local -a target_temperature
  local -a target_temperature_battery
  # MSR_THERM2_CTL
  local -a tm_select_enable
  local -a tm_select
  # IA32_MISC_ENABLE
  local -a tm2_enable_enable
  local -a tm2_enable
  local -a eist_enable_enable
  local -a eist_enable
  local -a eist_lock_enable
  local -a eist_lock
  local -a tbt_disable_enable
  local -a tbt_disable
  local -a fs_enable_enable
  local -a fs_enable
  local -a hwp_disable_enable
  local -a hwp_disable
  local -a ferr_enable_enable
  local -a ferr_enable
  local -a fsm_enable_enable
  local -a fsm_enable
  local -a adj_disable_enable
  local -a adj_disable
  local -a cpuid_maxval_enable
  local -a cpuid_maxval
  local -a xtpr_disable_enable
  local -a xtpr_disable
  local -a xd_disable_enable
  local -a xd_disable
  local -a dcu_disable_enable
  local -a dcu_disable
  local -a ipp_disable_enable
  local -a ipp_disbale
  # MSR_TURBO_ACTIVATION_RATIO
  local -a tbt_activation_ratio_enable
  local -a tbt_activation_ratio
  local -a tbt_activation_ratio_lock_enable
  local -a tbt_activation_ratio_lock
  # MSR_TURBO_RATIO_LIMIT
  local -a tbt_ratio_limit_enable
  local -a tbt_ratio_limit_1c
  local -a tbt_ratio_limit_2c
  local -a tbt_ratio_limit_3c
  local -a tbt_ratio_limit_4c
  local -a tbt_ratio_limit_5c
  local -a tbt_ratio_limit_6c
  local -a tbt_ratio_limit_7c
  local -a tbt_ratio_limit_8c
  local -a tbt_ratio_limit_9c
  local -a tbt_ratio_limit_10c
  local -a tbt_ratio_limit_11c
  local -a tbt_ratio_limit_12c
  local -a tbt_ratio_limit_13c
  local -a tbt_ratio_limit_14c
  local -a tbt_ratio_limit_15c
  local -a tbt_ratio_limit_16c
  local -a tbt_ratio_limit_17c
  local -a tbt_ratio_limit_18c
  # SMP/HTT
  local smp_disable_enable
  local smp_disable
  local per_cpu_htt_enable
  local -a htt_disable_enable
  local -a htt_disable
  # Frequency Scaling
  local -a fs_enabled_g
  local -a fs_batt_g
  local -a fs_per_thread
  local -a fs_enabled
  local -a fs_governor_ac
  local -a fs_min_freq_ac
  local -a fs_max_freq_ac
  local -a fs_freq_ac
  local -a fs_batt
  local -a fs_governor_dc
  local -a fs_min_freq_dc
  local -a fs_max_freq_dc
  local -a fs_freq_dc
  #
  # These are filled by 'ParseProcCpuinfo'
  #
  # num_logical_cpus        : number of logical cpus (0-based !)
  # num_physical_cpus       : number of physical cpus (0-based !)
  #
  # And arrays with indices for each physical processor containing:
  # cpuid_vendor_id         : the cpuid 'vendor_id'
  # cpuid_family            : the cpuid 'family'
  # cpuid_model             : the cpuid 'model'
  # cpuid_model_name        : the cpuid 'model name'
  # cpuid_stepping          : the cpuid 'stepping'
  # cpuid_num_cores         : the number of cores
  # synth_microarchitecture : the microarchitecture name (fam 6 only)
  # cpuid_logical_cpus      : The logical cpus for this processor
  #
  # Note: cpuid_logical_cpus is a two dimensional array, use 'GetLogicalCpu'
  #       to retrieve a logical cpu number.
  #
  local num_logical_cpus
  local num_physical_cpus
  local -a cpuid_vendor_id
  local -a cpuid_family
  local -a cpuid_model
  local -a cpuid_model_name
  local -a cpuid_stepping
  local -a cpuid_num_cores
  local -a cpuid_siblings
  local -a synth_microarchitecture
  local -a cpuid_logical_cpus
  #
  # These are initialised by 'ParseCPUID' and contain the CPUID
  # information for all logical cpus.
  # Each indice is a 'leaf' and contains four values,
  # one each for the EAX, EBX, ECX and EDX registers.
  #
  local -a cpuid_eax00_ecx00
  local -a cpuid_eax01_ecx00
  local -a cpuid_eax02_ecx00
  local -a cpuid_eax03_ecx00
  local -a cpuid_eax04_ecx00
  local -a cpuid_eax04_ecx01
  local -a cpuid_eax04_ecx02
  local -a cpuid_eax04_ecx03
  local -a cpuid_eax05_ecx00
  local -a cpuid_eax06_ecx00
  local -a cpuid_eax07_ecx00
  local -a cpuid_eax08_ecx00
  local -a cpuid_eax09_ecx00
  local -a cpuid_eax0A_ecx00
  local -a cpuid_eax0B_ecx00
  local -a cpuid_eax0B_ecx01
  local -a cpuid_eax0C_ecx00
  local -a cpuid_eax0D_ecx00
  local -a cpuid_eax0D_ecx01
  local -a cpuid_eax0D_ecx02
  local -a cpuid_eax1F_ecx01
  local -a cpuid_eax80000000
  local -a cpuid_eax80000001
  local -a cpuid_eax80000002
  local -a cpuid_eax80000003
  local -a cpuid_eax80000004
  local -a cpuid_eax80000005
  local -a cpuid_eax80000006
  local -a cpuid_eax80000007
  local -a cpuid_eax80000008
  local -a cpuid_eax80860000
  local -a cpuid_eaxc0000000
  #
  # (original) number of arguments on the commandline
  local numargs=$#
  #
  # Start by parsing both /proc/cpuinfo and the ouput of the CPUID instruction.
  GetProcessorsInformation
  #
  # Load the default configuration values
  # for each cpu that was found.
  DefaultCpuConfiguration
  #
  # Parse the entire commandline filling the rest of the local variables
  ParseCommandLineArguments "$@" || return 1
  #
  # Perform mutually exclusive actions that do not require anything special
  # like root privileges or reading/writing MSRs
  [ $doPrintHelp -ne 0 ] && { PrintHelp; return 0; }
  [ $doPrintVersion -ne 0 ] && { echo "$version"; return 0; }
  [ $doListProcessors -ne 0 ] && { ListProcessors; return 0; }
  [ $doCpuid -ne 0 ] && { PrintCPUID; return 0; }
  [ $doHttDetect -ne 0 ] && { PrintHyperThreadingSupport $isCpu; return 0; }
  #
  # Force the cpu type?
  [ -z "$force_march" ] && {
    # No, require at least something made by Intel
    IsSupportedProcessor || return 1
  } || {
    # Yes, force the microarchitecture
    [ $isVerbose -gt 1 ] && {
      echo "Forcing CPU #$isCpu microarchitecture to: '$force_march'"
    }
    synth_microarchitecture[$isCpu]=$force_march
  }
  #
  # Root privileges are required beyond this point.
  [ `id -u` -eq 0 ] || {
    >&2 echo -e "This action requires root privileges!\n"
    return 1
  }
  #
  # Load the msr kernel module if needed
  [ -e /dev/cpu/0/msr ] || {
    modprobe msr || {
      >&2 echo -e "'modprobe' failed to load the msr module!\n"
      return 1
    }
  }
  #
  # Validate the configuration specified on the commandline
  ValidateCmdlineConfiguration || return 1
  #
  # Load and validate the configuration file (values) when --boot, --resume
  # or --acpi are given as a command-line option.
  [ $doRead -ne 0 ] || {
    [[ $doBootOrResume -ne 0 || $doPower -ne 0 || $doFreqScaleAll -ne 0 ]] && {
      ParseConfigurationFile $configuration_file || return 1
      # Set isCpu to -1 to signal usage of the configuration file
      # instead of the cmdline configuration
      [ $doFreqScaleAll -eq 0 ] && isCpu=-1
    }
  }
  #
  # Validate the configuration of all cpus
  for i in ${!cpuid_model[@]}; do
    ValidateCpuConfiguration $i || return 1
  done
  #
  # Perform the actions specified on the commandline
  #
  [ $doRead -ne 0 ] && {
    # Read the values for all processors
    DoRead || return 1
  } || {
    [ $isCpu -ge 0 ] && {
      # Program values that were set via command-line arguments... (single cpu)
      DoProgramSingleCpu || return 1
    } || {
      # Program values for all cpu's defined in the configuration file...
      [ $doBootOrResume -ne 0 ] && {
        # --boot or --resume
        DoBootOrResume || return 1
      }
      [ $doPower -ne 0 ] && {
        # --acpi
        DoPower || return 1
      }
    }
  }
  return 0
}

