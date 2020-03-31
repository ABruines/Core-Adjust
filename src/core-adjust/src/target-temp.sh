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
# READ MSR_TEMPERATURE_TARGET
#
##############################################################################
#
# Return the target temperature (not including the offset) for the given cpu
# $1 == reference to the output variable
# $2 == the (physical) cpu nr to read
# returns 0 on success
TargetTemperature() {
  local -n TTEMP=$1
  local -i logical
  GetLogicalCpu $2 0 logical
  TTEMP=`rdmsr -p $logical -u -f23:16 0x1a2`
  [ $? -ne 0 ] && return 1
  return 0
}

# Return the target temperature offset range for the given cpu
# (assume unknown fam6h is newer than the latest known 'big core' cpu)
# $1 == reference to the output variable
# $2 == the cpu nr to read
# returns 0 on success
TargetTemperatureRange() {
  local -n RANGE=$1
  case "${synth_microarchitecture[$2]}" in
  "Nehalem" | "Westmere" | "Sandybridge")
    RANGE=0;;
  "Ivybridge" | "Haswell" | "Broadwell" | "Skylake" | "Kabylake" | "Cannonlake" | \
  "Icelake" | "Tigerlake" | "Cometlake" | "Unknown Fam6")
    RANGE=15;;
  "Silvermont" | "Airmont" | "Goldmont" | "Tremont" | "Knights Landing" | "Knights Mill")
    RANGE=63;;
  *)
    RANGE="n.a.";;
  esac
  return 0
}

# Return the target temperature minus the offset for a given voltage plane
# (assume unknown fam6h is newer than the latest known 'big core' cpu)
# $1 == reference to the target_temperature variable to read
# $2 == the (physical) cpu nr to read
# returns 0 on success
ReadTargetTemperature() {
  local -n RESULT=$1
  local -i logical
  local -i ttemp
  local -i toffset
  GetLogicalCpu $2 0 logical
  case "${synth_microarchitecture[$2]}" in
  "Nehalem" | "Westmere" | "Sandybridge")
    # No offset available, just the target temperature
    RESULT=`rdmsr -p $logical -u -f23:16 0x1a2`
    [ $? -ne 0 ] && return 1
    ;;
  "Ivybridge" | "Haswell" | "Broadwell" | "Skylake" | "Kabylake" | "Cannonlake" | \
  "Icelake" | "Tigerlake" | "Cometlake" | "Unknown Fam6")
    # Offset in bits 27:24. Write is only permitted if bit 30 of msr 0xCE is set, read is ok.
    ttemp=`rdmsr -p $logical -u -f23:16 0x1a2`
    [ $? -ne 0 ] && return 1
    toffset=`rdmsr -p $logical -u -f27:24 0x1a2`
    [ $? -ne 0 ] && return 1
    RESULT=$((ttemp - toffset))
    ;;
  "Silvermont" | "Airmont" | "Goldmont" | "Tremont" | "Knights Landing" | "Knights Mill")
    # Offset in bits 29:24.
    ttemp=`rdmsr -p $logical -u -f23:16 0x1a2`
    [ $? -ne 0 ] && return 1
    toffset=`rdmsr -p $logical -u -f29:24 0x1a2`
    [ $? -ne 0 ] && return 1
    RESULT=$((ttemp - toffset))
    ;;
  *)
    RESULT="n.a.";;
  esac
  return 0
}

#
# WRITE MSR_TEMPERATURE_TARGET
#

# Write target temperature (bits 27:24: 0...15 degrees)
# $1 == The temperature to program
# $2 == the physical cpu nr
# returns 0 on success
WriteTargetTemp15() {
  local -i logical
  GetLogicalCpu $2 0 logical
  [ `rdmsr -p $logical -u -f30:30 0xce` -ne 1 ] && {
    >&2 echo -e "Not allowed to program a target temperature for CPU #$2 because MSR_PLATFORM_INFO bit 30 is unset!"
    return 1
  }
  local -i val=`rdmsr -p $logical -u 0x1a2`
  [ $? -ne 0 ] && return 1
  local -i msk=$((val & 0xFFFFFFFFF0FFFFFF))
  local -i ttemp=$(((val >> 16) & 255))
  local -i tnew=$((ttemp - $1))
  [ $tnew -gt 15 ] && {
    >&2 echo -e "CPU #$2 Target Temperature must be in-between $((ttemp - 15))°C and $ttemp°C. ($1°C)\n"
    return 1
  }
  val=$((((tnew & 15) << 24) | msk))
  [ $isVerbose -gt 0 ] && {
    echo "  - Target temperature offset: $ttemp°C - $tnew°C = $1°C"
  }
  [ $isDryRun -ne 0 ] && {
    >&2 printf "  dry-run: wrmsr -p %d 0x1a2 0x%X\n" $logical $val
  } || {
    logger -t core-adjust.service "CPU #$2 target temperature offset -> $ttemp°C - $tnew°C = $1°C"
    wrmsr -p $logical 0x1a2 `printf "0x%X" $val` || return 1
  }
  return 0
}

# Write target temperature (bits 29:24: 0...63 degrees)
# $1 == The temperature to program
# $2 == the physical cpu nr
# returns 0 on success
WriteTargetTemp63() {
  local -i logical
  GetLogicalCpu $2 0 logical
  local -i val=`rdmsr -p $logical -u 0x1a2`
  [ $? -ne 0 ] && return 1
  local -i msk=$((val & 0xFFFFFFFFC0FFFFFF))
  local -i ttemp=$(((val >> 16) & 255))
  local -i tnew=$((ttemp - $1))
  [ $tnew -gt 63 ] && {
    >&2 echo -e "CPU #$2 Target Temperature must be in-between $((ttemp - 63))°C and $ttemp°C. ($1°C)\n"
    return 1
  }
  val=$((((tnew & 63) << 24) | msk))
  [ $isVerbose -gt 0 ] && {
    echo "  - Target temperature offset: $ttemp°C - $tnew°C = $1°C"
  }
  [ $isDryRun -ne 0 ] && {
    >&2 printf "  dry-run: wrmsr -p %d 0x1a2 0x%X\n" $logical $val
  } || {
    logger -t core-adjust.service "CPU #$2 target temperature offset -> $ttemp°C - $tnew°C = $1°C"
    wrmsr -p $logical 0x1a2 `printf '0x%016x' $val` || return 1
  }
  return 0
}

# Write target temperature
# (assume unknown fam6h is newer than the latest known 'big core' cpu)
# $1 == The temperature to program
# $2 == the physical cpu nr
# returns 0 on success
WriteTargetTemperature() {
  case "${synth_microarchitecture[$2]}" in
  "Ivybridge" | "Haswell" | "Broadwell" | "Skylake" | "Kabylake" | "Cannonlake" | \
  "Icelake" | "Tigerlake" | "Cometlake" | "Unknown Fam6")
    WriteTargetTemp15 $1 $2 || return 1
    ;;
  "Silvermont" | "Airmont" | "Goldmont" | "Tremont" | "Knights Landing" | "Knights Mill")
    WriteTargetTemp63 $1 $2 || return 1
    ;;
  *)
    >&2 echo -e "Setting a target temperature offset is not supported by CPU #$2 (${synth_microarchitecture[$2]})\n"
    return 1
    ;;
  esac
  return 0
}

# Write all target temperatures
# returns 0 on success
WriteEnabledTargetTemperatures() {
  # loop over all physical cpus (using the indices in target_temperature_enable)
  for i in ${!target_temperature_enable[@]}; do
    [ "${target_temperature_enable[$i]}" = "true" ] && {
      [ "${target_temperature_battery_enable[$i]}" = "true" ] && {
        /usr/bin/on_ac_power
        [ $? -eq 1 ] && {
          # on battery power
          [ $isVerbose -gt 0 ] && {
            echo "  - Selecting 'on battery power' target."
          }
          WriteTargetTemperature ${target_temperature_battery[$i]} $i || return 1
        } || {
          # on ac power (or unknown)
          [ $isVerbose -gt 0 ] && {
            echo "  - Selecting 'on AC power' target."
          }
          WriteTargetTemperature ${target_temperature[$i]} $i || return 1
        }
      } || {
        WriteTargetTemperature ${target_temperature[$i]} $i || return 1
      }
    } || {
      [[ $isVerbose -gt 0 && ($doBootOrResume -ne 0 || $doPower -ne 0) ]] && {
        echo "  - Adjusting the Target temperature is disabled"
      }
      continue
    }
  done
  return 0
}

