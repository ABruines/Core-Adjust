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
# Returns 0 if $1 is a number, 1 if it isn't.
IsNumber() {
  [[ $1 =~ ^[+-]?[0-9]+\.?[0-9]*$ ]] || return 1
  return 0
}

# $1 == name of cmdline option
# $2 == value of said option
# Returns 0 if $2 is a number, 1 if it isn't.
RequireNumber() {
  IsNumber $2 || {
    >&2 echo -e "Option '$1' requires an extra numeric argument.\n"
    return 1
  }
  return 0
}

# $1 == name of cmdline option
# $2 == value of said option
# Returns 0 if $2 is a non-empty string, 1 if it isn't.
RequireGovernorString() {
  local gov
  local available
  [ "x$2" = "x" ] && {
    >&2 echo -e "Option '$1' requires an extra string argument.\n"
    return 1
  }
  available=$(cpufreq-info --governors)
  [ $? -ne 0 ] && {
    >&2 echo -e "Could not get available frequency scaling governors.\n"
    return 1
  }
  for gov in $available; do
    [ "$2" = "$gov" ] && return 0
  done
  >&2 echo -e "Frequency scaling governor '$2' is not available!\n"
  return 1
}

# $1 == name of cmdline option
# $2 == value of said option
# Returns 0 if $2 is a valid logical cpu number, 1 if it isn't.
RequireLogicalCpu() {
  local p
  local l
  RequireNumber $1 $2 || return 1
  # Loop over all physical processors
  for p in ${!cpuid_logical_cpus[@]}; do
    # Loop over the logical cpus of this processor
    for l in ${cpuid_logical_cpus[$p]}; do
      # Return 0 on a match
      [ $2 -eq $l ] && return 0
    done
  done
  >&2 echo -e "Option '$1' requires a valid logical cpu number.\n"
  return 1
}

# $1 == name of cmdline option
# $2 == value of said option
# returns 0 on error(!) or 1 for 'on' and 2 for 'off'
RequireOnOff() {
  # convert $2 to lowercase then regex match 'on' or 'off'
  [[ ${2,,} =~ ^(on|off)$ ]] || {
    >&2 echo -e "Option '$1' requires a state argument (on/off).\n"
    return 0
  }
  case ${BASH_REMATCH[1]} in
  'on') return 1;;
  'off') return 2;;
  esac
  return 0
}

# Test if a given FIVR voltage offset is positive.
# $1 == value under test
# $2 == name of the voltage plane
# Returns 0 if the voltage is positive or 1 if the voltage is 0 or negative
IsPositiveVoltageOffset() {
  [ "x$1" = "xNaN" ] && {
    >&2 echo -e "Configuration error! The voltage offset for $2 has an invalid value.\n"
    return 0
  }
  [ `bc -l <<<"rv=0; if ($1 < -999.0234375) rv=1; if ($1 > 0) rv=1; print rv;"` -eq 1 ] && {
    >&2 echo -e "$2 voltage offset is out of range ($1 mV), it must be in-between -999.0234375 and 0 mV.\n"
    return 0
  }
  return 1
}

# Test if a given FIVR voltage offset is within the valid range.
# $1 == value under test
# $2 == name of the voltage plane
# Returns 0 if the voltage is within range or 1 if it is not.
IsValidVoltageOffset() {
  [ "x$1" = "xNaN" ] && {
    >&2 echo -e "Configuration error! The voltage offset for $2 has an invalid value.\n"
    return 1
  }
  [ `bc -l <<<"rv=0; if ($1 < -999.0234375) rv=1; if ($1 > 999.0234375) rv=1; print rv;"` -eq 1 ] && {
    >&2 echo -e "$2 voltage offset is out of range ($1 mV), it must be in-between -999.0234375 and 999.0234375 mV.\n"
    return 1
  }
  return 0
}

# Parse the numeric argument given to any of the --planeX options
#   $1 == the name of the enable_??? variable
#   $2 == the name of the variable holding the numeric value
#   $3 == the name of the commandline option
#   $4 == the option argument to test (can be empty)
# Returns 0 on success or 1 if the number value was invalid or missing.
ParseNumericOptionArgument() {
  # Get the correct variables by reference
  local -n enable_=$1
  local -n target_=$2
  # test if the option was was given more then once
  [ "$enable_" = "true" ] && {
    >&2 echo -e "$3 cannot be specified more then once!\n"
    return 1
  }
  # test if it is a numeric value
  [[ $4 =~ ^[+-]?[0-9]+\.?[0-9]*$ ]] || {
    >&2 echo -e "$3 requires a numeric value as argument.\n"
    return 1
  }
  # always matches, store the value removing any leading plus-sign (bc does not like those)
  [[ $4 =~ ^[+]?(.*) ]] && target_=${BASH_REMATCH[1]}
  # success, enable the voltage plane
  enable_="true"
  return 0
}

# Parse commandline arguments
# $@ == all commandline arguments
# returns 0 on success
ParseCommandLineArguments() {
  local enabled
  local plane
  [ $# = 0 ] && {
    PrintHelp
    return 1
  }
  while [ $# -ne 0 ]
  do
    case "$1" in
    '--selftest')
      # dry-runs the entire range of possible voltage offsets on plane #0
      # the output can be used to verify the calculated voltage offsets.
      # The 'written' offset should match the input offset of -999.0234375...+999.0234375 mV
      local output
      for i in {-1023..1023}; do
        output=$($0 -v -f -d -p0 `printf "%.7f" $(bc -l <<<"scale=7; $i * 0.9765625")`)
        [ $isVerbose -gt 0 ] && echo $output
        [[ $output =~ .*=[[:space:]]+([-]?[[:digit:]]+[.]?[[:digit:]]*).* ]] && {
          [[ `printf "%.7f" $(bc -l <<<"scale=7; $i * 0.9765625")` = `printf "%.7f" ${BASH_REMATCH[1]}` ]] && {
            echo -n "."
            continue
          }
        }
        echo -e "\nMatching failed, voltage offset calculation is broken!\n$output"
        exit 1
      done
      echo -e "\nSuccess!!"
      exit 0
      ;; 
    '--help' | '-h')
      doPrintHelp=1
      return 0
      ;;
    '--version')
      doPrintVersion=1
      return 0
      ;;
    '--verbose' | '-v')
      isVerbose=$(($isVerbose + 1))
      ;;
    '--processor' | '-p')
      [ $haveProcessor = 1 ] && {
        >&2 echo -e "'$1' may only be specified once!\n"
        return 1
      }
      RequireNumber $1 $2 || return 1
      CmdlineSwitchProcessor $2
      haveProcessor=1
      doReadSingleCpu=1
      shift
      ;;
    '--march' | '-m')
      # converts $2 to all lowercase then makes the first char of every word uppercase
      force_march=""
      for i in ${2,,}; do
        [ -z "$force_march" ] && {
          force_march=${i^}
        } || {
          force_march="$force_march ${i^}"
        }
      done
      shift
      ;;
    '--dry-run' | '-d')
      isDryRun=1
      ;;
    '--list' | '-l')
      doListProcessors=1
      ;;
    '--cpuid' | '-c')
      doCpuid=1
      ;;
    '--boot' | '--resume')
      doBootOrResume=1
      ;;
    '--acpi')
      doPower=1
      ;;
    '--read' | '-r')
      doRead=1
      ;;
    '--force' | '-f')
      isForce=1
      ;;
    '--plane0' | '-p0')
      doFivr=1
      enabled=${voltage_offset_plane_0_enable[$isCpu]}
      ParseNumericOptionArgument enabled plane $1 $2 || return 1
      voltage_offset_plane_0_enable[$isCpu]=$enabled
      voltage_offset_plane_0[$isCpu]=$plane
      shift
      ;;
    '--plane1' | '-p1')
      doFivr=1
      enabled=${voltage_offset_plane_1_enable[$isCpu]}
      ParseNumericOptionArgument enabled plane $1 $2 || return 1
      voltage_offset_plane_1_enable[$isCpu]=$enabled
      voltage_offset_plane_1[$isCpu]=$plane
      shift
      ;;
    '--plane2' | '-p2')
      doFivr=1
      enabled=${voltage_offset_plane_2_enable[$isCpu]}
      ParseNumericOptionArgument enabled plane $1 $2 || return 1
      voltage_offset_plane_2_enable[$isCpu]=$enabled
      voltage_offset_plane_2[$isCpu]=$plane
      shift
      ;;
    '--plane3' | '-p3')
      doFivr=1
      enabled=${voltage_offset_plane_3_enable[$isCpu]}
      ParseNumericOptionArgument enabled plane $1 $2 || return 1
      voltage_offset_plane_3_enable[$isCpu]=$enabled
      voltage_offset_plane_3[$isCpu]=$plane
      shift
      ;;
    '--plane4' | '-p4')
      doFivr=1
      enabled=${voltage_offset_plane_4_enable[$isCpu]}
      ParseNumericOptionArgument enabled plane $1 $2 || return 1
      voltage_offset_plane_4_enable[$isCpu]=$enabled
      voltage_offset_plane_4[$isCpu]=$plane
      shift
      ;;
    '--plane5' | '-p5')
      doFivr=1
      enabled=${voltage_offset_plane_5_enable[$isCpu]}
      ParseNumericOptionArgument enabled plane $1 $2 || return 1
      voltage_offset_plane_5_enable[$isCpu]=$enabled
      voltage_offset_plane_5[$isCpu]=$plane
      shift
      ;;
    '--temp' | '-t')
      doTargetTemp=1
      enabled=${target_temperature_enable[$isCpu]}
      ParseNumericOptionArgument enabled plane $1 $2 || return 1
      target_temperature_enable[$isCpu]=$enabled
      target_temperature[$isCpu]=$plane
      shift
      ;;
    '--smt')
      RequireOnOff $1 $2 && return 1
      [ $? -eq 1 ] && doEnableSmt=1 || doDisableSmt=1
      shift
      ;;
    '--htt')
      RequireOnOff $1 $2 && return 1
      [ $? -eq 1 ] && doEnableSmtCpu=1 || doDisableSmtCpu=1
      shift
      ;;
    '--htt-detect')
      doHttDetect=1
      ;;
    '--tm1-select')
      doTmSelect=1
      tm_select_enable[$isCpu]="true"
      tm_select[$isCpu]="false"
      ;;
    '--tm2-select')
      doTmSelect=1
      tm_select_enable[$isCpu]="true"
      tm_select[$isCpu]="true"
      ;;
    '--tm2-enable')
      doTm2Enable=1
      tm2_enable_enable[$isCpu]="true"
      tm2_enable[$isCpu]="true"
      ;;
    '--tm2-disable')
      doTm2Enable=1
      tm2_enable_enable[$isCpu]="true"
      tm2_enable[$isCpu]="false"
      ;;
    '--eist-enable')
      doEistEnable=1
      eist_enable_enable[$isCpu]="true"
      eist_enable[$isCpu]="true"
      ;;
    '--eist-disable')
      doEistEnable=1
      eist_enable_enable[$isCpu]="true"
      eist_enable[$isCpu]="false"
      ;;
    '--eist-lock')
      doEistLock=1
      eist_lock_enable[$isCpu]="true"
      eist_lock[$isCpu]="true"
      ;;
    '--tbt-disable')
      doTbtDisable=1
      tbt_disable_enable[$isCpu]="true"
      tbt_disable[$isCpu]="true"
      ;;
    '--tbt-enable')
      doTbtDisable=1
      tbt_disable_enable[$isCpu]="true"
      tbt_disable[$isCpu]="false"
      ;;
    '--tbt-activation-ratio')
      RequireNumber $1 $2 || return 1
      doTbtActivationRatio=1
      tbt_activation_ratio_enable[$isCpu]="true"
      tbt_activation_ratio[$isCpu]=$2
      shift
      ;;
    '--tbt-activation-ratio-lock')
      doTbtActivationRatioLock=1
      tbt_activation_ratio_lock_enable[$isCpu]="true"
      tbt_activation_ratio_lock[$isCpu]="true"
      ;;
    '-1c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_1c[$isCpu]=$2
      shift
      ;;
    '-2c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_2c[$isCpu]=$2
      shift
      ;;
    '-3c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_3c[$isCpu]=$2
      shift
      ;;
    '-4c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_4c[$isCpu]=$2
      shift
      ;;
    '-5c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_5c[$isCpu]=$2
      shift
      ;;
    '-6c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_6c[$isCpu]=$2
      shift
      ;;
    '-7c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_7c[$isCpu]=$2
      shift
      ;;
    '-8c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_8c[$isCpu]=$2
      shift
      ;;
    '-9c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_9c[$isCpu]=$2
      shift
      ;;
    '-10c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_10c[$isCpu]=$2
      shift
      ;;
    '-11c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_11c[$isCpu]=$2
      shift
      ;;
    '-12c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_12c[$isCpu]=$2
      shift
      ;;
    '-13c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_13c[$isCpu]=$2
      shift
      ;;
    '-14c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_14c[$isCpu]=$2
      shift
      ;;
    '-15c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_15c[$isCpu]=$2
      shift
      ;;
    '-16c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_16c[$isCpu]=$2
      shift
      ;;
    '-17c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_17c[$isCpu]=$2
      shift
      ;;
    '-18c')
      RequireNumber $1 $2 || return 1
      doMaxTurboRatio=1
      tbt_ratio_limit_enable[$isCpu]="true"
      tbt_ratio_limit_18c[$isCpu]=$2
      shift
      ;;
    '--fscale-all')
      doFreqScaleAll=1
      shift
      ;;
    '--fscale')
      doFreqScale=1
      RequireLogicalCpu $1 $2 || return 1
      RequireGovernorString $1 ${3,,} || return 1
      fs_enabled_g[$isCpu]="true"
      fs_per_thread[$isCpu]="true"
      fs_enabled[$2]="true"
      fs_governor_ac[$2]=${3,,}
      [ "${fs_governor_ac[$2]}" = "userspace" ] && {
        RequireNumber $1 $4 || return 1
        fs_freq_ac[$2]=$4
        shift
      } || {
        RequireNumber $1 $4 || return 1
        RequireNumber $1 $5 || return 1
        fs_min_freq_ac[$2]=$4
        fs_max_freq_ac[$2]=$5
        shift
        shift
      }
      shift
      shift
      ;;
    '--fs-enable')
      doFastStringsEnable=1
      fs_enable_enable[$isCpu]="true"
      fs_enable[$isCpu]="true"
      shift
      ;;
    '--fs-disable')
      doFastStringsEnable=1
      fs_enable_enable[$isCpu]="true"
      fs_enable[$isCpu]="false"
      shift
      ;;
    '--hwp-disable')
      doHwPrefetcherDisable=1
      hwp_disable_enable[$isCpu]="true"
      hwp_disable[$isCpu]="true"
      shift
      ;;
    '--hwp-enable')
      doHwPrefetcherDisable=1
      hwp_disable_enable[$isCpu]="true"
      hwp_disable[$isCpu]="false"
      shift
      ;;
    '--ferr-enable')
      doFerrMpxEnable=1
      ferr_enable_enable[$isCpu]="true"
      ferr_enable[$isCpu]="true"
      shift
      ;;
    '--ferr-disable')
      doFerrMpxEnable=1
      ferr_enable_enable[$isCpu]="true"
      ferr_enable[$isCpu]="false"
      shift
      ;;
    '--fsm-enable')
      doMonitorFsmEnable=1
      fsm_enable_enable[$isCpu]="true"
      fsm_enable[$isCpu]="true"
      shift
      ;;
    '--fsm-disable')
      doMonitorFsmEnable=1
      fsm_enable_enable[$isCpu]="true"
      fsm_enable[$isCpu]="false"
      shift
      ;;
    '--adj-clp-disable')
      doAdjCacheLinePrefetchDisable=1
      adj_disable_enable[$isCpu]="true"
      adj_disable[$isCpu]="true"
      shift
      ;;
    '--adj-clp-enable')
      doAdjCacheLinePrefetchDisable=1
      adj_disable_enable[$isCpu]="true"
      adj_disable[$isCpu]="false"
      shift
      ;;
    '--cpuid-max-enable')
      doCpuidMaxvalEnable=1
      cpuid_maxval_enable[$isCpu]="true"
      cpuid_maxval[$isCpu]="true"
      shift
      ;;
    '--cpuid-max-disable')
      doCpuidMaxvalEnable=1
      cpuid_maxval_enable[$isCpu]="true"
      cpuid_maxval[$isCpu]="false"
      shift
      ;;
    '--xtpr-msg-disable')
      doXtprMessageDisable=1
      xtpr_disable_enable[$isCpu]="true"
      xtpr_disable[$isCpu]="true"
      shift
      ;;
    '--xtpr-msg-enable')
      doXtprMessageDisable=1
      xtpr_disable_enable[$isCpu]="true"
      xtpr_disable[$isCpu]="false"
      shift
      ;;
    '--xd-bit-disable')
      doXdBitDisable=1
      xd_disable_enable[$isCpu]="true"
      xd_disable[$isCpu]="true"
      shift
      ;;
    '--xd-bit-enable')
      doXdBitDisable=1
      xd_disable_enable[$isCpu]="true"
      xd_disable[$isCpu]="false"
      shift
      ;;
    '--dcup-disable')
      doDcuPrefetcherDisable=1
      dcu_disable_enable[$isCpu]="true"
      dcu_disable[$isCpu]="true"
      shift
      ;;
    '--dcup-enable')
      doDcuPrefetcherDisable=1
      dcu_disable_enable[$isCpu]="true"
      dcu_disable[$isCpu]="false"
      shift
      ;;
    '--ipp-disable')
      doIpPrefetcherDisable=1
      ipp_disable_enable[$isCpu]="true"
      ipp_disbale[$isCpu]="true"
      shift
      ;;
    '--ipp-enable')
      doIpPrefetcherDisable=1
      ipp_disable_enable[$isCpu]="true"
      ipp_disbale[$isCpu]="false"
      shift
      ;;
    *)
      >&2 echo -e "Unknown option: '$1'\n"
      return 1
      ;;
    esac
    shift
  done
  return 0
}

# Move the processor configuration specified on the commandline to another processor id.
# $1 == the new processor number
CmdlineSwitchProcessor() {
  [ $1 -eq $isCpu ] && return
  # copy configuration to the new processor
  #
  # FIVR
  allow_overvolting[$1]=${allow_overvolting[$isCpu]}
  voltage_offset_plane_0_enable[$1]=${voltage_offset_plane_0_enable[$isCpu]}
  voltage_offset_plane_1_enable[$1]=${voltage_offset_plane_1_enable[$isCpu]}
  voltage_offset_plane_2_enable[$1]=${voltage_offset_plane_2_enable[$isCpu]}
  voltage_offset_plane_3_enable[$1]=${voltage_offset_plane_3_enable[$isCpu]}
  voltage_offset_plane_4_enable[$1]=${voltage_offset_plane_4_enable[$isCpu]}
  voltage_offset_plane_5_enable[$1]=${voltage_offset_plane_5_enable[$isCpu]}
  voltage_offset_plane_0[$1]=${voltage_offset_plane_0[$isCpu]}
  voltage_offset_plane_1[$1]=${voltage_offset_plane_1[$isCpu]}
  voltage_offset_plane_2[$1]=${voltage_offset_plane_2[$isCpu]}
  voltage_offset_plane_3[$1]=${voltage_offset_plane_3[$isCpu]}
  voltage_offset_plane_4[$1]=${voltage_offset_plane_4[$isCpu]}
  voltage_offset_plane_5[$1]=${voltage_offset_plane_5[$isCpu]}
  # Target Temperature
  target_temperature_enable[$1]=${target_temperature_enable[$isCpu]}
  target_temperature_battery_enable[$1]=${target_temperature_battery_enable[$isCpu]}
  target_temperature[$1]=${target_temperature[$isCpu]}
  target_temperature_battery[$1]=${target_temperature_battery[$isCpu]}
  # MSR_THERM2_CTL
  tm_select_enable[$1]=${tm_select_enable[$isCpu]}
  tm_select[$1]=${tm_select[$isCpu]}
  # IA32_MISC_ENABLE
  tm2_enable_enable[$1]=${tm2_enable_enable[$isCpu]}
  tm2_enable[$1]=${tm2_enable[$isCpu]}
  eist_enable_enable[$1]=${eist_enable_enable[$isCpu]}
  eist_enable[$1]=${eist_enable[$isCpu]}
  eist_lock_enable[$1]=${eist_lock_enable[$isCpu]}
  eist_lock[$1]=${eist_lock[$isCpu]}
  tbt_disable_enable[$1]=${tbt_disable_enable[$isCpu]}
  tbt_disable[$1]=${tbt_disable[$isCpu]}
  fs_enable_enable[$1]=${fs_enable_enable[$isCpu]}
  fs_enable[$1]=${fs_enable[$isCpu]}
  hwp_disable_enable[$1]=${hwp_disable_enable[$isCpu]}
  hwp_disable[$1]=${hwp_disable[$isCpu]}
  ferr_enable_enable[$1]=${ferr_enable_enable[$isCpu]}
  ferr_enable[$1]=${ferr_enable[$isCpu]}
  fsm_enable_enable[$1]=${fsm_enable_enable[$isCpu]}
  fsm_enable[$1]=${fsm_enable[$isCpu]}
  adj_disable_enable[$1]=${adj_disable_enable[$isCpu]}
  adj_disable[$1]=${adj_disable[$isCpu]}
  cpuid_maxval_enable[$1]=${cpuid_maxval_enable[$isCpu]}
  cpuid_maxval[$1]=${cpuid_maxval[$isCpu]}
  xtpr_disable_enable[$1]=${xtpr_disable_enable[$isCpu]}
  xtpr_disable[$1]=${xtpr_disable[$isCpu]}
  xd_disable_enable[$1]=${xd_disable_enable[$isCpu]}
  xd_disable[$1]=${xd_disable[$isCpu]}
  dcu_disable_enable[$1]=${dcu_disable_enable[$isCpu]}
  dcu_disable[$1]=${dcu_disable[$isCpu]}
  ipp_disable_enable[$1]=${ipp_disable_enable[$isCpu]}
  ipp_disbale[$1]=${ipp_disbale[$isCpu]}
  # MSR_TURBO_ACTIVATION_RATIO
  tbt_activation_ratio_enable[$1]=${tbt_activation_ratio_enable[$isCpu]}
  tbt_activation_ratio[$1]=${tbt_activation_ratio[$isCpu]}
  tbt_activation_ratio_lock_enable[$1]=${tbt_activation_ratio_lock_enable[$isCpu]}
  tbt_activation_ratio_lock[$1]=${tbt_activation_ratio_lock[$isCpu]}
  # MSR_TURBO_RATIO_LIMIT
  tbt_ratio_limit_enable[$1]=${tbt_ratio_limit_enable[$isCpu]}
  tbt_ratio_limit_1c[$1]=${tbt_ratio_limit_1c[$isCpu]}
  tbt_ratio_limit_2c[$1]=${tbt_ratio_limit_2c[$isCpu]}
  tbt_ratio_limit_3c[$1]=${tbt_ratio_limit_3c[$isCpu]}
  tbt_ratio_limit_4c[$1]=${tbt_ratio_limit_4c[$isCpu]}
  tbt_ratio_limit_5c[$1]=${tbt_ratio_limit_5c[$isCpu]}
  tbt_ratio_limit_6c[$1]=${tbt_ratio_limit_6c[$isCpu]}
  tbt_ratio_limit_7c[$1]=${tbt_ratio_limit_7c[$isCpu]}
  tbt_ratio_limit_8c[$1]=${tbt_ratio_limit_8c[$isCpu]}
  tbt_ratio_limit_9c[$1]=${tbt_ratio_limit_9c[$isCpu]}
  tbt_ratio_limit_10c[$1]=${tbt_ratio_limit_10c[$isCpu]}
  tbt_ratio_limit_11c[$1]=${tbt_ratio_limit_11c[$isCpu]}
  tbt_ratio_limit_12c[$1]=${tbt_ratio_limit_12c[$isCpu]}
  tbt_ratio_limit_13c[$1]=${tbt_ratio_limit_13c[$isCpu]}
  tbt_ratio_limit_14c[$1]=${tbt_ratio_limit_14c[$isCpu]}
  tbt_ratio_limit_15c[$1]=${tbt_ratio_limit_15c[$isCpu]}
  tbt_ratio_limit_16c[$1]=${tbt_ratio_limit_16c[$isCpu]}
  tbt_ratio_limit_17c[$1]=${tbt_ratio_limit_17c[$isCpu]}
  tbt_ratio_limit_18c[$1]=${tbt_ratio_limit_18c[$isCpu]}
  # SMP/HTT
  htt_disable_enable[$1]=${htt_disable_enable[$isCpu]}
  htt_disable[$1]=${htt_disable[$isCpu]}
  #
  # unset the configuration of the old processor
  #
  # FIVR
  unset allow_overvolting[$isCpu]
  unset voltage_offset_plane_0_enable[$isCpu]
  unset voltage_offset_plane_1_enable[$isCpu]
  unset voltage_offset_plane_2_enable[$isCpu]
  unset voltage_offset_plane_3_enable[$isCpu]
  unset voltage_offset_plane_4_enable[$isCpu]
  unset voltage_offset_plane_5_enable[$isCpu]
  unset voltage_offset_plane_0[$isCpu]
  unset voltage_offset_plane_1[$isCpu]
  unset voltage_offset_plane_2[$isCpu]
  unset voltage_offset_plane_3[$isCpu]
  unset voltage_offset_plane_4[$isCpu]
  unset voltage_offset_plane_5[$isCpu]
  # Target Temperature
  unset target_temperature_enable[$isCpu]
  unset target_temperature_battery_enable[$isCpu]
  unset target_temperature[$isCpu]
  unset target_temperature_battery[$isCpu]
  # MSR_THERM2_CTL
  unset tm_select_enable[$isCpu]
  unset tm_select[$isCpu]
  # IA32_MISC_ENABLE
  unset tm2_enable_enable[$isCpu]
  unset tm2_enable[$isCpu]
  unset eist_enable_enable[$isCpu]
  unset eist_enable[$isCpu]
  unset eist_lock_enable[$isCpu]
  unset eist_lock[$isCpu]
  unset tbt_disable_enable[$isCpu]
  unset tbt_disable[$isCpu]
  unset fs_enable_enable[$isCpu]
  unset fs_enable[$isCpu]
  unset hwp_disable_enable[$isCpu]
  unset hwp_disable[$isCpu]
  unset ferr_enable_enable[$isCpu]
  unset ferr_enable[$isCpu]
  unset fsm_enable_enable[$isCpu]
  unset fsm_enable[$isCpu]
  unset adj_disable_enable[$isCpu]
  unset adj_disable[$isCpu]
  unset cpuid_maxval_enable[$isCpu]
  unset cpuid_maxval[$isCpu]
  unset xtpr_disable_enable[$isCpu]
  unset xtpr_disable[$isCpu]
  unset xd_disable_enable[$isCpu]
  unset xd_disable[$isCpu]
  unset dcu_disable_enable[$isCpu]
  unset dcu_disable[$isCpu]
  unset ipp_disable_enable[$isCpu]
  unset ipp_disbale[$isCpu]
  # MSR_TURBO_ACTIVATION_RATIO
  unset tbt_activation_ratio_enable[$isCpu]
  unset tbt_activation_ratio[$isCpu]
  unset tbt_activation_ratio_lock_enable[$isCpu]
  unset tbt_activation_ratio_lock[$isCpu]
  # MSR_TURBO_RATIO_LIMIT
  unset tbt_ratio_limit_enable[$isCpu]
  unset tbt_ratio_limit_1c[$isCpu]
  unset tbt_ratio_limit_2c[$isCpu]
  unset tbt_ratio_limit_3c[$isCpu]
  unset tbt_ratio_limit_4c[$isCpu]
  unset tbt_ratio_limit_5c[$isCpu]
  unset tbt_ratio_limit_6c[$isCpu]
  unset tbt_ratio_limit_7c[$isCpu]
  unset tbt_ratio_limit_8c[$isCpu]
  unset tbt_ratio_limit_9c[$isCpu]
  unset tbt_ratio_limit_10c[$isCpu]
  unset tbt_ratio_limit_11c[$isCpu]
  unset tbt_ratio_limit_12c[$isCpu]
  unset tbt_ratio_limit_13c[$isCpu]
  unset tbt_ratio_limit_14c[$isCpu]
  unset tbt_ratio_limit_15c[$isCpu]
  unset tbt_ratio_limit_16c[$isCpu]
  unset tbt_ratio_limit_17c[$isCpu]
  unset tbt_ratio_limit_18c[$isCpu]
  # SMP/HTT
  unset htt_disable_enable[$isCpu]
  unset htt_disable[$isCpu]
  #
  # Set the new processor nr to use
  isCpu=$1
}

