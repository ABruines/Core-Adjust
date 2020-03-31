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
# Read all values from cpu $1 and print them to standard output.
# $1 == cpu number
# no return value
DoReadSingleCpu() {
  # Read the target_temperature for cpu $1
  ReadTargetTemperature target_temperature[$1] $1 || return 1
  # Read the FIVR voltage offsets for cpu $1
  ReadVoltageOffset voltage_offset_plane_0[$1] $1 || return 1
  ReadVoltageOffset voltage_offset_plane_1[$1] $1 || return 1
  ReadVoltageOffset voltage_offset_plane_2[$1] $1 || return 1
  ReadVoltageOffset voltage_offset_plane_3[$1] $1 || return 1
  ReadVoltageOffset voltage_offset_plane_4[$1] $1 || return 1
  ReadVoltageOffset voltage_offset_plane_5[$1] $1 || return 1
  # Print the values to STDOUT
  echo "processor #$1"
  [ $isVerbose -gt 0 ] && {
    IsNumber ${target_temperature[$1]} && {
      local ttemp
      local range
      TargetTemperature ttemp $1
      TargetTemperatureRange range $1
      printf "  default target temperature : %4d °C\n" $ttemp
      printf "  temperature offset range   : %4d °C\n" $range
    } || {
      echo   "  default target temperature : n.a."
      echo   "  temperature offset range   : n.a."
    }
  }
  IsNumber ${target_temperature[$1]} &&\
    printf "  target temperature         : %4d °C\n" ${target_temperature[$1]} ||\
    echo   "  target temperature         : ${target_temperature[$1]}"
  IsNumber ${voltage_offset_plane_0[$1]} &&\
    printf "  voltage plane #0 offset    : %+12.7f mV\n" ${voltage_offset_plane_0[$1]} ||\
    echo   "  voltage plane #0 offset    : ${voltage_offset_plane_0[$1]}"
  IsNumber ${voltage_offset_plane_1[$1]} &&\
    printf "  voltage plane #1 offset    : %+12.7f mV\n" ${voltage_offset_plane_1[$1]} ||\
    echo   "  voltage plane #1 offset    : ${voltage_offset_plane_1[$1]}"
  IsNumber ${voltage_offset_plane_2[$1]} &&\
    printf "  voltage plane #2 offset    : %+12.7f mV\n" ${voltage_offset_plane_2[$1]} ||\
    echo   "  voltage plane #2 offset    : ${voltage_offset_plane_2[$1]}"
  IsNumber ${voltage_offset_plane_3[$1]} &&\
    printf "  voltage plane #3 offset    : %+12.7f mV\n" ${voltage_offset_plane_3[$1]} ||\
    echo   "  voltage plane #3 offset    : ${voltage_offset_plane_3[$1]}"
  IsNumber ${voltage_offset_plane_4[$1]} &&\
    printf "  voltage plane #4 offset    : %+12.7f mV\n" ${voltage_offset_plane_4[$1]} ||\
    echo   "  voltage plane #4 offset    : ${voltage_offset_plane_4[$1]}"
  IsNumber ${voltage_offset_plane_5[$1]} &&\
    printf "  voltage plane #5 offset    : %+12.7f mV\n" ${voltage_offset_plane_5[$1]} ||\
    echo   "  voltage plane #5 offset    : ${voltage_offset_plane_5[$1]}"
}

# Loop over all selected cpus and read their values
# printing them to the standard output.
# No arguments, always returns 0
DoRead() {
  [ $doReadSingleCpu -ne 1 ] && {
    for i in ${!cpuid_model[@]}; do
      DoReadSingleCpu $i
    done
    return 0
  }
  DoReadSingleCpu $isCpu
  return 0
}

# Program the cpu values specified on the commandline
# (ie. Not a Boot/Resume/Power event).
DoProgramSingleCpu() {
  local -i rv=0
  #
  # Enable/disable SMT globally/processor
  [ $doEnableSmt -ne 0 ] && {
    EnableSMT || {
      >&2 echo -e "Could not enable SMT.\n"
      rv=1
    }
  }
  [ $doDisableSmt -ne 0 ] && {
    DisableSMT || {
      >&2 echo -e "Could not disable SMT.\n"
      rv=1
    }
  }
  [ $doEnableSmtCpu -ne 0 ] && {
    EnableLogicalCores $isCpu || {
      >&2 echo -e "Could not enable HTT for cpu #$isCpu.\n"
      rv=1
    }
  }
  [ $doDisableSmtCpu -ne 0 ] && {
    DisableLogicalCores $isCpu || {
      >&2 echo -e "Could not disable HTT for cpu #$isCpu.\n"
      rv=1
    }
  }
  #
  [[ $doFivr -ne 0 || \
     $doTargetTemp -ne 0 || \
     $doTmSelect -ne 0 || \
     $doTm2Enable -ne 0 || \
     $doEistEnable -ne 0 || \
     $doEistLock -ne 0 || \
     $doTbtDisable -ne 0 || \
     $doFastStringsEnable -ne 0 || \
     $doHwPrefetcherDisable -ne 0 || \
     $doFerrMpxEnable -ne 0 || \
     $doMonitorFsmEnable -ne 0 || \
     $doAdjCacheLinePrefetchDisable -ne 0 || \
     $doCpuidMaxvalEnable -ne 0 || \
     $doXtprMessageDisable -ne 0 || \
     $doXdBitDisable -ne 0 || \
     $doDcuPrefetcherDisable -ne 0 || \
     $doIpPrefetcherDisable -ne 0 || \
     $doMaxTurboRatio -ne 0 || \
     $doTbtActivationRatio -ne 0 || \
     $doTbtActivationRatioLock -ne 0 ]] && {
    # Warn if the microarchitecture could not be (precisely) determined
    [ "${synth_microarchitecture[$isCpu]}" = "Unknown Fam6" ] && {
      echo "Programming unknown 'family 6' CPU #${!allow_overvolting[@]}, assuming 'Skylake' or newer..."
    } || {
      [ $isVerbose -gt 0 ] && {
        echo "Programming CPU #${isCpu}"
      }
    }
  }
  #
  # FIVR
  WriteEnabledVoltageOffsets || rv=1
  #
  # Target Temperature
  WriteEnabledTargetTemperatures || rv=1
  #
  # (Re)set MSR_THERM2_CTL.TM_SELECT
  [ $doTmSelect -ne 0 ] && {
    Prg_TM_SELECT $isCpu || rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.TM2_ENABLE
  [ $doTm2Enable -ne 0 ] && {
    Prg_TM2_SELECT $isCpu || rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.EIST_Enable
  [ $doEistEnable -ne 0 ] && {
    Prg_EIST_Enable $isCpu || rv=1
  }
  #
  # Set IA32_MISC_ENABLE.EIST_Select_Lock
  [ $doEistLock -ne 0 ] && {
    Prg_EIST_Lock $isCpu || rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.IDA_Disable
  [ $doTbtDisable -ne 0 ] && {
    Prg_TBT_Disable $isCpu || rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.Fast_Strings_Enable
  [ $doFastStringsEnable -ne 0 ] && {
    Prg_Fast_Strings_Enable $isCpu || rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.Hardware_Prefetcher_Disable
  [ $doHwPrefetcherDisable -ne 0 ] && {
    Prg_Hardware_Prefetcher_Disable $isCpu || rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.FERR_Multiplexing_Enable
  [ $doFerrMpxEnable -ne 0 ] && {
    Prg_FERR_Multiplexing_Enable $isCpu || rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.ENABLE_MONITOR_FSM
  [ $doMonitorFsmEnable -ne 0 ] && {
    Prg_ENABLE_MONITOR_FSM $isCpu || rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.Adjacent_Cache_Line_Prefetch_Disable
  [ $doAdjCacheLinePrefetchDisable -ne 0 ] && {
    Prg_Adjacent_Cache_Line_Prefetch_Disable $isCpu || rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.Limit_CPUID_Maxval
  [ $doCpuidMaxvalEnable -ne 0 ] && {
    Prg_Limit_CPUID_Maxval $isCpu || rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.xTPR_Message_Disable
  [ $doXtprMessageDisable -ne 0 ] && {
    Prg_xTPR_Message_Disable $isCpu || rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.XD_Bit_Disable
  [ $doXdBitDisable -ne 0 ] && {
    Prg_XD_Bit_Disable $isCpu || rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.DCU_Prefetcher_Disable
  [ $doDcuPrefetcherDisable -ne 0 ] && {
    Prg_DCU_Prefetcher_Disable $isCpu || rv=1
    rv=1
  }
  #
  # (Re)set IA32_MISC_ENABLE.IP_Prefetcher_Disable
  [ $doIpPrefetcherDisable -ne 0 ] && {
    Prg_IP_Prefetcher_Disable $isCpu || rv=1
  }
  #
  # Set MSR_TURBO_ACTIVATION_RATIO.MAX_NON_TURBO_RATIO
  [ $doTbtActivationRatio -ne 0 ] && {
    Prg_TBT_activation_ratio $isCpu || rv=1
  }
  #
  # Set MSR_TURBO_ACTIVATION_RATIO.TURBO_ACTIVATION_RATIO_Lock
  [ $doTbtActivationRatioLock -ne 0 ] && {
    Prg_TBT_activation_ratio_lock $isCpu || rv=1
  }
  #
  # Set MSR_TURBO_RATIO_LIMIT
  [ $doMaxTurboRatio -ne 0 ] && {
    Prg_TBT_ratio_limit $isCpu || rv=1
  }
  #
  # Frequency Scaling
  [[ $doFreqScale -ne 0 || $doFreqScaleAll -ne 0 ]] && {
    FrequencyScalingApply || rv=1
  }
  #
  return $rv
}

# Program the cpu values in response to a 'Boot/Resume' event
# (ie. program all values found in the INI file).
DoBootOrResume() {
  local -i rv=0
  [[ "$apply_on_boot_and_resume" = "false" && $isForce -eq 0 ]] && {
    [ $isVerbose -gt 0 ] && {
      echo "Apply on Boot and Resume is disabled in the configuration file!"
    }
    logger -t core-adjust.service "Apply_On_Boot_And_Resume is disabled!"
  } || {
    # If we need to enable SMT then do this first to ensure all
    # logical cpus are programmed if IA32_MISC_ENABLE is modified.
    # If we need to disable SMT then do this after everything else
    # for the same resason.
    [ "$smp_disable_enable" = "true" ] && {
      [ "$smp_disable" = "false" ] && {
        EnableSMT || rv=1
      }
    }
    # Adjust the processors settings:
    [ $isVerbose -gt 0 ] && echo "Adjusting FIVR Voltage Offsets."
    WriteEnabledVoltageOffsets || rv=1
    [ $isVerbose -gt 0 ] && echo "Adjusting Target Temperature Offsets."
    WriteEnabledTargetTemperatures || rv=1
    # - loop over all processors
    for i in ${!cpuid_model[@]}; do
      [ $isVerbose -gt 0 ] && echo "Adjusting MSR values for processor #$i."
      # - programm MSR values
      Prg_TM_SELECT $i || rv=1
      Prg_TM2_SELECT $i || rv=1
      Prg_EIST_Enable $i || rv=1
      Prg_EIST_Lock $i || rv=1
      Prg_TBT_Disable $i || rv=1
      Prg_Fast_Strings_Enable $i || rv=1
      Prg_Hardware_Prefetcher_Disable $i || rv=1
      Prg_FERR_Multiplexing_Enable $i || rv=1
      Prg_ENABLE_MONITOR_FSM $i || rv=1
      Prg_Adjacent_Cache_Line_Prefetch_Disable $i || rv=1
      Prg_Limit_CPUID_Maxval $i || rv=1
      Prg_xTPR_Message_Disable $i || rv=1
      Prg_XD_Bit_Disable $i || rv=1
      Prg_DCU_Prefetcher_Disable $i || rv=1
      Prg_IP_Prefetcher_Disable $i || rv=1
      Prg_TBT_activation_ratio $i || rv=1
      Prg_TBT_activation_ratio_lock $i || rv=1
      Prg_TBT_ratio_limit $i || rv=1
      # - program HTT settings (after adjusting the MSRs)
      [[ "$per_cpu_htt_enable" = "true" && "${htt_disable_enable[$i]}" = "true" ]] && {
        [ $isVerbose -gt 0 ] && echo "Adjusting HTT settings for CPU #$i"
        [ "$htt_disable" = "true" ] && {
          DisableLogicalCores $i || rv=1
        } || {
          EnableLogicalCores $i || rv=1
        }
      }
    done
    # Apply frequency scaling settings
    FrequencyScalingApply || rv=1
    # Disable SMT?
    [ "$smp_disable_enable" = "true" ] && {
      [ "$smp_disable" = "true" ] && {
        [ $isVerbose -gt 0 ] && echo "Adjusting SMT settings."
        DisableSMT || rv=1
      }
    }
  }
  [[ $rv -ne 0 && $isVerbose -gt 0 ]] && {
    >&2 echo -e "One or more processor settings could not be adjusted!\n"
  }
  return $rv
}

# Program the cpu values in response to a 'Power' event
# (ie. program the values found in the INI file).
DoPower() {
  local -i rv=0
  [ $isVerbose -gt 0 ] && {
    echo "ACPI Power event, adjusting target temperature(s):"
  }
  [ "x$apply_on_acpi_power_event" == "xtrue" ] && {
    WriteEnabledTargetTemperatures || rv=1
    FrequencyScalingApply || rv=1
  } || {
    [ $isVerbose -gt 0 ] && {
      echo "Apply on ACPI Power Event is disabled!"
    }
    logger -t core-adjust.service "Apply_On_Acpi_Power_Event is disabled!"
  }
  [[ $rv -ne 0 && $isVerbose -gt 0 ]] && {
    >&2 echo -e "One or more processor settings could not be adjusted!\n"
  }
  return $rv
}

