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
# READ FIVR VOLTAGE OFFSETS
#
##############################################################################
#
# MSR 0x150 bitfields:
#
# bit 20:0   reserved,                 always 0
# bit 31:21  voltage offset,           11bit signed value
# bit 32     0:read 1:write
# bit 35:33  reserved,                 always 0
# bit 36     reserved,                 always 1
# bit 39:37  reserved,                 always 0
# bit 42:40  voltage plane index       0-5
# bit 62:43  reserved,                 always 0
# bit 63     reserved,                 always 1
#
##############################################################################
#
# Convert a value read from MSR 150h into a voltage offset (in millivolts):
#
# The command `rdmsr -p 0 -u -f31:21 0x150` reads an 11 bit value from
# MSR 150h that represents a voltage offset in units of 1/1024 volt.
#
# The above rdmsr command will mask the other bits read from the
# register and right-shifts the result to bit 0.
# It is this value that must be fed into this function...
#
# $1 == reference to the variable holding the 11bit value read from MSR 150h
#
VoltageOffsetFromMsrValue() {
  # DO NOT REMOVE THE DOLLAR SIGN FROM $VALUE INSIDE $((...)) !
  local -n VALUE=$1
  [ $VALUE -ne 0 ] && {
    # Is the sign-bit set?
    [ $(($VALUE & 2**10)) -ne 0 ] && {
      # negative voltage offset:
      # first extend the sign-bit from bit 11 to bit 63 (by 2x twos-complement),
      # then multiply by 1/1024 to get the number of millivolts.
      VALUE=`bc -l <<<"scale=7; $((((($VALUE ^ 0x7FF) + 1) ^ -1) + 1)) * 0.9765625"`
    } || {
      # positive voltage offset:
      # just multiply by 1/1024 to get the number of millivolts
      VALUE=`bc -l <<<"scale=7; $VALUE * 0.9765625"`
    }
    # bc does not print the leading 0 for fractions,
    # this inserts the leading 0 when needed.
    [[ $VALUE =~ ^([-]?)([[:digit:]]*)([.]?.*) ]] && {
      VALUE=${BASH_REMATCH[1]}${BASH_REMATCH[2]:-0}${BASH_REMATCH[3]}
    }
  }
}

# read the voltage offset for a given voltage plane
# (assume unknown fam6h is newer than the latest known 'big core' cpu)
# $1 == reference to the voltage plane variable to read
# $2 == the (physical) cpu nr to read
# returns 0 on success
ReadVoltageOffset() {
  local -n RESULT=$1
  #
  # Get the first logical cpu for this processor
  local logical
  GetLogicalCpu $2 0 logical || return 1
  #
  case "${synth_microarchitecture[$2]}" in
  "Haswell" | "Broadwell" | "Skylake" | "Kabylake" | "Cannonlake" | \
  "Icelake" | "Tigerlake" | "Cometlake" | "Unknown Fam6")
    ;;
  *)
    RESULT="n.a."
    return 0
    ;;
  esac
  #
  [[ $1 =~ voltage_offset_plane_([012345]{1}) ]] || {
    >&2 echo -e "Internal Error, invalid argument to function ReadVoltageOffset()."
    return 1
  } && {
    case ${BASH_REMATCH[1]} in
      # Let the MSR know that we want the offset for voltage plane X by
      # writing these values:
      0) wrmsr -p $logical 0x150 0x8000001000000000 || return 1;;
      1) wrmsr -p $logical 0x150 0x8000011000000000 || return 1;;
      2) wrmsr -p $logical 0x150 0x8000021000000000 || return 1;;
      3) wrmsr -p $logical 0x150 0x8000031000000000 || return 1;;
      4) wrmsr -p $logical 0x150 0x8000041000000000 || return 1;;
      5) wrmsr -p $logical 0x150 0x8000051000000000 || return 1;;
    esac
  }
  #
  #  The command `rdmsr -p 0 -u -f31:21 0x150` reads the 11 bit value
  #  from the MSR while purposly ignoring the sign bit.
  #  ('rdmsr -d' does not extend the sign bit correctly in this case!)
  RESULT=`rdmsr -p $logical -u -f31:21 0x150`
  [ $? -ne 0 ] && return 1
  #
  # convert the raw result into millivolts
  VoltageOffsetFromMsrValue RESULT
  #
  return 0
}

#
# WRITE FIVR VOLTAGE OFFSETS
#
# write the voltage offset for a given voltage plane to msr 0x150
# $1 == reference to the voltage plane variable to write
# $2 == the (physical) cpu nr to write
# returns 0 on success
_WriteVoltageOffset() {
  local -n voltage_offset=$1
  local plane
  local -i write_mask
  local -i logical
  GetLogicalCpu $2 0 logical || return 1
  # determine the write mask to apply
  [[ $1 =~ voltage_offset_plane_([012345]{1}) ]] || {
    >&2 echo -e "Internal Error, invalid argument to function _WriteVoltageOffset()."
    return 1
  } && {
    plane=${BASH_REMATCH[1]}
    case $plane in
      0) write_mask=0x8000001100000000;;
      1) write_mask=0x8000011100000000;;
      2) write_mask=0x8000021100000000;;
      3) write_mask=0x8000031100000000;;
      4) write_mask=0x8000041100000000;;
      5) write_mask=0x8000051100000000;;
    esac
  }
  # Calculate the number of 1/1024 Volt steps, then shift the value to
  # bits 31:21 and apply the voltage-plane write bitmask.
  local value=`printf "%.0f" $(bc -l <<<"scale=7; $voltage_offset / 0.9765625")`
  value=$(($write_mask | (($value << 21) & 0xffe00000)))
  [ $isVerbose -gt 0 ] && {
    local o=$((($value >> 21) & 0x7FF))
    local v=$o
    [ $(($v & 2**10)) -ne 0 ] && v=$((((($v ^ 0x7FF) + 1) ^ -1) + 1))
    VoltageOffsetFromMsrValue o
    echo "  - Voltage plane #$plane offset: $voltage_offset mV -> $v/1024 = $o mV"
  }
  [ $isDryRun -ne 0 ] && {
    >&2 printf "  dry-run: wrmsr -p %d 0x150 0x%X\n" $logical $value
  } || {
    logger -t core-adjust.service "CPU #$2 FIVR $1 -> $voltage_offset mV"
    wrmsr -p $logical 0x150 `printf "0x%X" $value` || return 1
  }
  return 0
}

# write the voltage offset for a given voltage plane to msr 0x150
# (assume unknown fam6h is newer than the latest known 'big core' cpu)
# $1 == reference to the voltage plane variable to write
# $2 == the (physical) cpu nr to write
# returns 0 on success
WriteVoltageOffset() {
  case "${synth_microarchitecture[$2]}" in
  "Haswell" | "Broadwell" | "Skylake" | "Kabylake" | "Cannonlake" | \
  "Icelake" | "Tigerlake" | "Cometlake" | "Unknown Fam6")
    _WriteVoltageOffset $1 $2 || return 1
    ;;
  *)
    >&2 echo -e "Setting a FIVR voltage offset is not supported by CPU #$2 (${synth_microarchitecture[$2]}).\n"
    [ $isDryRun -eq 0 ] && return 1
    ;;
  esac
  return 0
}

# Writes all enabled voltage offsets to msr 0x150 for all processors
# returns 0 on success
WriteEnabledVoltageOffsets() {
  # loop over all physical cpus (using the indices in voltage_offset_plane_0_enable)
  for i in ${!voltage_offset_plane_0_enable[@]}; do
    [ "${voltage_offset_plane_0_enable[$i]}" = "true" ] && {
      WriteVoltageOffset voltage_offset_plane_0[$i] $i || return 1
    } || {
      [[ $isVerbose -gt 0 && $doBootOrResume -ne 0 ]] &&\
        echo "  - Voltage plane #0 offset: disabled"
    }
    [ "${voltage_offset_plane_1_enable[$i]}" = "true" ] && {
      WriteVoltageOffset voltage_offset_plane_1[$i] $i || return 1
    } || {
      [[ $isVerbose -gt 0 && $doBootOrResume -ne 0 ]] &&\
        echo "  - Voltage plane #1 offset: disabled"
    }
    [ "${voltage_offset_plane_2_enable[$i]}" = "true" ] && {
      WriteVoltageOffset voltage_offset_plane_2[$i] $i || return 1
    } || {
      [[ $isVerbose -gt 0 && $doBootOrResume -ne 0 ]] &&\
        echo "  - Voltage plane #2 offset: disabled"
    }
    [ "${voltage_offset_plane_3_enable[$i]}" = "true" ] && {
      WriteVoltageOffset voltage_offset_plane_3[$i] $i || return 1
    } || {
      [[ $isVerbose -gt 0 && $doBootOrResume -ne 0 ]] &&\
        echo "  - Voltage plane #3 offset: disabled"
    }
    [ "${voltage_offset_plane_4_enable[$i]}" = "true" ] && {
      WriteVoltageOffset voltage_offset_plane_4[$i] $i || return 1
    } || {
      [[ $isVerbose -gt 0 && $doBootOrResume -ne 0 ]] &&\
        echo "  - Voltage plane #4 offset: disabled"
    }
    [ "${voltage_offset_plane_5_enable[$i]}" = "true" ] && {
      WriteVoltageOffset voltage_offset_plane_5[$i] $i || return 1
    } || {
      [[ $isVerbose -gt 0 && $doBootOrResume -ne 0 ]] &&\
        echo "  - Voltage plane #5 offset: disabled"
    }
  done
  return 0
}

