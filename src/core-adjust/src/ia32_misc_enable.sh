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
# Program a single IA32_MISC_ENABLE bit
# $1 == physical cpu number
# $2 == enabled in cfg (true/false)
# $3 == value for the bit (true/false)
# $4 == the bit position
# $5 == the name of the bit
# returns 0 on success
Prg_IA32_MISC_ENABLE() {
  local -i l
  local -i value
  local -i org
  # Load the logical cpus for this processor into an array
  local -i -a logical=(${cpuid_logical_cpus[$1]})
  # Adjust enabled in cfg?
  [ "$2" = "false" ] && {
    [ $isVerbose -gt 0 ] && {
      echo "  - Adjusting $5 is disabled"
    }
  } || {
    [ $isVerbose -gt 0 ] && {
      echo "  - $5 ($3)"
    }
    [ ${#logical[@]} -eq 0 ] && {
      # no logical cpus!?
      return 1
    }
    # Read the current IA32_MISC_ENABLE value.
    value=`rdmsr -p ${logical[0]} -u 416`
    [ $? -ne 0 ] && {
      >&2 echo "Failed to read IA32_MISC_ENABLE."
      return 1
    }
    org=$value
    # (Re)set the bit
    [ "$3" = "true" ] && {
      value=$(($value | (2**$4)))
    } || {
      value=$(($value & ~(2**$4)))
    }
    # Write the IA32_MISC_ENABLE value back to all logical cpus
    [ $value -ne $org ] && {
      for l in ${logical[@]}; do
        [ $isDryRun -ne 0 ] && {
          >&2 printf "  dry-run: wrmsr -p %d 416 0x%X\n" $l $value
        } || {
          wrmsr -p $l 416 `printf "0x%X" $value`
          [ $? -ne 0 ] && {
            >&2 echo "Failed to write IA32_MISC_ENABLE."
            return 1
          }
        }
      done
    }
  }
  return 0
}

Prg_Fast_Strings_Enable() {
  Prg_IA32_MISC_ENABLE \
      $1 "${fs_enable_enable[$1]}" "${fs_enable[$1]}" 0 "Fast_Strings_Enable"
  return $?
}

Prg_Hardware_Prefetcher_Disable() {
  Prg_IA32_MISC_ENABLE \
      $1 "${hwp_disable_enable[$1]}" "${hwp_disable[$1]}" 9 "Hardware_Prefetcher_Disable"
  return $?
}

Prg_FERR_Multiplexing_Enable() {
  Prg_IA32_MISC_ENABLE \
      $1 "${ferr_enable_enable[$1]}" "${ferr_enable[$1]}" 10 "FERR_Multiplexing_Enable"
  return $?
}

Prg_TM2_SELECT() {
  Prg_IA32_MISC_ENABLE \
      $1 "${tm2_enable_enable[$1]}" "${tm2_enable[$1]}" 13 "TM2_SELECT"
  return $?
}

Prg_EIST_Enable() {
  Prg_IA32_MISC_ENABLE \
      $1 "${eist_enable_enable[$1]}" "${eist_enable[$1]}" 16 "EIST_Enable"
  return $?
}

Prg_ENABLE_MONITOR_FSM() {
  Prg_IA32_MISC_ENABLE \
      $1 "${fsm_enable_enable[$1]}" "${fsm_enable[$1]}" 18 "ENABLE_MONITOR_FSM"
  return $?
}

Prg_Adjacent_Cache_Line_Prefetch_Disable() {
  Prg_IA32_MISC_ENABLE \
      $1 "${adj_disable_enable[$1]}" "${adj_disable[$1]}" 19 "Prg_Adjacent_Cache_Line_Prefetch_Disable"
  return $?
}

Prg_EIST_Lock() {
  Prg_IA32_MISC_ENABLE \
      $1 "${eist_lock_enable[$1]}" "${eist_lock[$1]}" 20 "EIST_Select_Lock"
  return $?
}

Prg_Limit_CPUID_Maxval() {
  Prg_IA32_MISC_ENABLE \
      $1 "${cpuid_maxval_enable[$1]}" "${cpuid_maxval[$1]}" 22 "Limit_CPUID_Maxval"
  return $?
}

Prg_xTPR_Message_Disable() {
  Prg_IA32_MISC_ENABLE \
      $1 "${xtpr_disable_enable[$1]}" "${xtpr_disable[$1]}" 23 "xTPR_Message_Disable"
  return $?
}

Prg_XD_Bit_Disable() {
  Prg_IA32_MISC_ENABLE \
      $1 "${xd_disable_enable[$1]}" "${xd_disable[$1]}" 34 "XD_Bit_Disable"
  return $?
}

Prg_DCU_Prefetcher_Disable() {
  Prg_IA32_MISC_ENABLE \
      $1 "${dcu_disable_enable[$1]}" "${dcu_disable[$1]}" 37 "DCU_Prefetcher_Disable"
  return $?
}

Prg_TBT_Disable() {
  Prg_IA32_MISC_ENABLE \
    $1 "${tbt_disable_enable[$1]}" "${tbt_disable[$1]}" 38 "IDA_Disable"
  return $?
}

Prg_IP_Prefetcher_Disable() {
  Prg_IA32_MISC_ENABLE \
    $1 "${ipp_disable_enable[$1]}" "${ipp_disable[$1]}" 39 "IP_Prefetcher_Disable"
  return $?
}

