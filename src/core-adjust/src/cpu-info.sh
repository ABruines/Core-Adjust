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
# See kernel sources: 'arch/x86/include/asm/intel-family.h'
# Up to date with kernel: 5.4-rc5
#
# $1 == output variable (reference)
# $2 == cpuid family (6)
# $3 == cpuid model
# $4 == id of the physical processor
# no return value
#
# Note:
#   To add a new microarchitecture, add its name and model number(s) to:
#    - GetMicroarchitecture
#   Then to actually implement support, add its name to the case switch in:
#    - ValidateCpuConfiguration
#    - ReadVoltageOffset
#    - WriteVoltageOffset
#    - TargetTemperatureRange
#    - ReadTargetTemperature
#    - WriteTargetTemperature
#
GetMicroarchitecture() {
  local -n march=$1
  # Convert the CPUID family/model integers into hex strings because 'case'
  # uses character pattern matching.
  local family=`printf "0x%02X" $2`
  local model=`printf "0x%02X" $3`
  case $family in
  0x06)
    case $model in
    # "Big Core" Processors (Branded as Core, Xeon, etc...)
    0x0E)                     march="Core";;
    0x0F|0x16|0x17|0x1D)      march="Core2";;
    0x1E|0x1F|0x1A|0x2E)      march="Nehalem";;
    0x25|0x2C|0x2F)           march="Westmere";;
    0x2A|0x2D)                march="Sandybridge";;
    0x3A|0x3E)                march="Ivybridge";;
    0x3C|0x3F|0x45|0x46)      march="Haswell";;
    0x3D|0x47|0x4F|0x56)      march="Broadwell";;
    0x4E|0x5E|0x55)           march="Skylake";;
    0x8E|0x9E)                march="Kabylake";;
    0x66)                     march="Cannonlake";;
    0x6A|0x6C|0x7D|0x7E|0x9D) march="Icelake";;
    0x8C|0x8D)                march="Tigerlake";;
    0xA5|0xA6)                march="Cometlake";;
    # "Small Core" Processors (Atom)
    0x1C|0x26)                march="Bonnell";;
    0x36|0x27|0x35)           march="Saltwell";;
    0x37|0x4D|0x4A)           march="Silvermont";;
    0x4C|0x5A)                march="Airmont";;
    0x5C|0x5D|0x5F|0x7A)      march="Goldmont";;
    0x86|0x96)                march="Tremont";;
    # Xeon Phi
    0x57)                     march="Knights Landing";;
    0x85)                     march="Knights Mill";;
    # unknown family 6 member
    *)                        march="Unknown Fam6";;
    esac;;
  *)
    march="Unknown";;
  esac
}

# This code assumes that /proc/cpuinfo lists all logical processors
# in order, ie. first listing the cpus of processor 0 in numerical order,
# then those for processor 1 etc.
#
# no return value
ParseProcCpuinfo() {
  # Temporary arrays to store information about each logical cpu
  local -a vendor_id
  local -a family
  local -a model
  local -a model_name
  local -a stepping
  local -a physical_id
  local -a core_id
  local -a cpu_cores
  local -a cpu_siblings
  #
  # Empty the current values so we dont accidentaly append
  # data to the arrays
  unset cpuid_vendor_id
  unset cpuid_family
  unset cpuid_model
  unset cpuid_model_name
  unset cpuid_stepping
  unset cpuid_num_cores
  unset cpuid_siblings
  unset synth_microarchitecture
  unset cpuid_logical_cpus
  #
  # After the while/read loop each indice of 'logical_cpu' contains the
  # logical cpus belonging to that physical cpu.
  local -a logical_cpu
  #
  # Used by the while/read loop to remember the current logical cpu.
  local current_logical
  #
  # The (0-based) number of logical and physical cpus.
  num_logical_cpus=-1
  num_physical_cpus=0
  #
  # If we have root permission then ensure that Linux has a current list
  # of configured CPUs by rescanning them
  [ `id -u` -eq 0 ] && {
    [ -f /sys/devices/system/cpu/rescan ] && {
      echo 1 > /sys/devices/system/cpu/rescan
    }
  }
  #
  # Parse /proc/cpuinfo line by line and use a regex to match + capture,
  # counting logical/physical processors as we go and storing the information we want:
  while read line; do
    [[ $line =~ ^([A-Za-z_ ]+)[[:space:]]*:[[:space:]]*(.*) ]] && {
      case "${BASH_REMATCH[1]}" in
      "processor")  num_logical_cpus=$(($num_logical_cpus + 1));
                    current_logical=${BASH_REMATCH[2]};;
      "vendor_id")  vendor_id[$num_logical_cpus]=${BASH_REMATCH[2]};;
      "cpu family") family[$num_logical_cpus]=${BASH_REMATCH[2]};;
      "model")      model[$num_logical_cpus]=${BASH_REMATCH[2]};;
      "model name") model_name[$num_logical_cpus]=${BASH_REMATCH[2]};;
      "stepping")   stepping[$num_logical_cpus]=${BASH_REMATCH[2]};;
      "core id")    core_id[$num_logical_cpus]=${BASH_REMATCH[2]};;
      "cpu cores")  cpu_cores[$num_logical_cpus]=${BASH_REMATCH[2]};;
      "siblings")   cpu_siblings[$num_logical_cpus]=${BASH_REMATCH[2]};;
      "physical id")
        physical_id[$num_logical_cpus]=${BASH_REMATCH[2]}
        [ ${BASH_REMATCH[2]} -gt $num_physical_cpus ] && {
          num_physical_cpus=${BASH_REMATCH[2]}
        }
        logical_cpu[$num_physical_cpus]="${logical_cpu[$num_physical_cpus]} ${current_logical}"
        ;;
      esac
    }
  done < /proc/cpuinfo
  #
  # Translate the information about the logical cpus to
  # information about the physical cpus:
  local X
  for p in `seq 0 $num_physical_cpus`; do
    for l in `seq 0 $num_logical_cpus`; do
      # is the current logical cpu the first core of the current physical cpu
      [[ ${physical_id[$l]} -eq $p ]] && [[ ${core_id[$l]} -eq 0 ]] && {
        cpuid_vendor_id[$p]=${vendor_id[$l]}
        cpuid_family[$p]=${family[$l]}
        cpuid_model[$p]=${model[$l]}
        cpuid_model_name[$p]=${model_name[$l]}
        cpuid_stepping[$p]=${stepping[$l]}
        cpuid_num_cores[$p]=${cpu_cores[$l]}
        cpuid_siblings[$p]=${cpu_siblings[$l]}
        cpuid_logical_cpus[$p]=${logical_cpu[$p]}
        # ! cannot use array member as reference to var so use a temporary (X)
        GetMicroarchitecture X ${family[$l]} ${model[$l]} $p
        synth_microarchitecture[$p]=$X
        break
      }
    done
  done
}

# $1 == The physical cpu nr (0...num_physical_cpus)
# $2 == the (ordinal) logical cpu nr for that physical cpu (0...cpuid_siblings[$1]-1).
# $3 == Reference to the variable that receives the result.
# returns 0 id the logical cpu was found, 1 if not
GetLogicalCpu() {
  local -n RESULT=$3
  local -i -a LOGICAL=(${cpuid_logical_cpus[$1]})
  # no or less logical cpus available then requested?
  [[ ${#LOGICAL[@]} -eq 0 || $((${#LOGICAL[@]} - 1)) -lt $2 ]] && {
    # always return a valid result if its a dry-run
    [ $isDryRun -ne 0 ] && {
      RESULT=0
      return 0
    }
    RESULT=-1
    return 1
  }
  RESULT=${LOGICAL[$2]}
  return 0
}

# Prints the processor information to stdout
# no return value
ListProcessors() {
  [ $isVerbose -gt 0 ] && {
    for i in ${!cpuid_model[@]}; do
      echo "Processor #$i"
      echo "  vendor id     : ${cpuid_vendor_id[$i]}"
      echo "  family        : ${cpuid_family[$i]}"
      echo "  model         : ${cpuid_model[$i]}"
      echo "  stepping      : ${cpuid_stepping[$i]}"
      echo "  cores         : ${cpuid_num_cores[$i]}"
      echo "  siblings      : ${cpuid_siblings[$i]}"
      echo "  model name    : ${cpuid_model_name[$i]}"
      echo "  micro-arch    : ${synth_microarchitecture[$i]}"
      echo "  logical cpus  :${cpuid_logical_cpus[$i]}"
    done
  } || {
    for i in ${!cpuid_model[@]}; do
      echo "#$i: ${cpuid_model_name[$i]} (${synth_microarchitecture[$i]})"
    done
  }
}

# Test if the physical processor(s) are supported by this application
# returns 0 on success
IsSupportedProcessor() {
  # Is this an Intel system
  for i in ${!cpuid_model[@]}; do
    [ "x${cpuid_vendor_id[$i]}" = "xGenuineIntel" ] || {
      >&2 echo "Unsupported CPU #$i: ${cpuid_model_name[$i]} (${synth_microarchitecture[$i]})"
      >&2 echo -e "This application requires an Intel processor.\n"
      return 1
    }
  done
  return 0;
}

_cpuid_extract() {
  local -n VAR=$1
  [[ $2 =~ eax=0x(.*)[[:space:]]*ebx=0x(.*)[[:space:]]*ecx=0x(.*)[[:space:]]*edx=0x(.*) ]] && {
    VAR="${BASH_REMATCH[1]}${BASH_REMATCH[2]}${BASH_REMATCH[3]}${BASH_REMATCH[4]}"
  }
}

# List CPUID output for each physical processor
#
# 33 lines of output per processor
#
# Note:
# Unable to verify leaf 1F (V2 Extended Topology Enumeration), my cpu does not have that leaf
# We are interested in sub-leaf 1 (core-level)...
#
ParseCPUID() {
  local i
  local line
  local p
  #
  # Cannot use an array member as reference to a variable
  # when calling a function so use a temporary variable instead
  # and assign the array member values after the function call.
  local X
  #
  # Load default values in-case an entry does not exist.
  unset cpuid_eax00_ecx00
  unset cpuid_eax01_ecx00
  unset cpuid_eax02_ecx00
  unset cpuid_eax03_ecx00
  unset cpuid_eax04_ecx00
  unset cpuid_eax04_ecx01
  unset cpuid_eax04_ecx02
  unset cpuid_eax04_ecx03
  unset cpuid_eax05_ecx00
  unset cpuid_eax06_ecx00
  unset cpuid_eax07_ecx00
  unset cpuid_eax08_ecx00
  unset cpuid_eax09_ecx00
  unset cpuid_eax0A_ecx00
  unset cpuid_eax0B_ecx00
  unset cpuid_eax0B_ecx01
  unset cpuid_eax0C_ecx00
  unset cpuid_eax0D_ecx00
  unset cpuid_eax0D_ecx01
  unset cpuid_eax0D_ecx02
  unset cpuid_eax1F_ecx01
  unset cpuid_eax80000000
  unset cpuid_eax80000001
  unset cpuid_eax80000002
  unset cpuid_eax80000003
  unset cpuid_eax80000004
  unset cpuid_eax80000005
  unset cpuid_eax80000006
  unset cpuid_eax80000007
  unset cpuid_eax80000008
  unset cpuid_eax80860000
  unset cpuid_eaxc0000000
  for i in `seq 0 $num_logical_cpus`; do
    cpuid_eax00_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax01_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax02_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax03_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax04_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax04_ecx01[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax04_ecx02[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax04_ecx03[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax05_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax06_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax07_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax08_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax09_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax0A_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax0B_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax0B_ecx01[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax0C_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax0D_ecx00[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax0D_ecx01[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax0D_ecx02[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax1F_ecx01[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax80000000[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax80000001[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax80000002[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax80000003[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax80000004[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax80000005[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax80000006[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax80000007[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax80000008[$i]="00000000 00000000 00000000 00000000"
    cpuid_eax80860000[$i]="00000000 00000000 00000000 00000000"
    cpuid_eaxc0000000[$i]="00000000 00000000 00000000 00000000"
  done
  #
  # parse the output of the command 'cpuid -r'
  # and fill the arrays with one entry for each logical cpu
  p=0
  while read line; do
    [[ $line =~ CPU[[:space:]]*([0-9]+)\: ]] && {
      p="${BASH_REMATCH[1]}"
      continue
    }
    [[ $line =~ (0x[0-9a-f]{8})[[:space:]]*(0x[0-9a-f]{2})\:[[:space:]]*(.*) ]] && {
      case "${BASH_REMATCH[1]}" in
        0x00000000) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax00_ecx00[$p]=$X;;
        0x00000001) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax01_ecx00[$p]=$X;;
        0x00000002) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax02_ecx00[$p]=$X;;
        0x00000003) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax03_ecx00[$p]=$X;;
        0x00000004)
          case "${BASH_REMATCH[2]}" in
            0x00) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax04_ecx00[$p]=$X;;
            0x01) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax04_ecx01[$p]=$X;;
            0x02) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax04_ecx02[$p]=$X;;
            0x03) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax04_ecx03[$p]=$X;;
          esac
          ;;
        0x00000005) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax05_ecx00[$p]=$X;;
        0x00000006) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax06_ecx00[$p]=$X;;
        0x00000007) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax07_ecx00[$p]=$X;;
        0x00000008) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax08_ecx00[$p]=$X;;
        0x00000009) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax09_ecx00[$p]=$X;;
        0x0000000a) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax0A_ecx00[$p]=$X;;
        0x0000000b)
          case "${BASH_REMATCH[2]}" in
            0x00) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax0B_ecx00[$p]=$X;;
            0x01) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax0B_ecx01[$p]=$X;;
          esac
          ;;
        0x0000000c) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax0C_ecx00[$p]=$X;;
        0x0000000d)
          case "${BASH_REMATCH[2]}" in
            0x00) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax0D_ecx00[$p]=$X;;
            0x01) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax0D_ecx01[$p]=$X;;
            0x02) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax0D_ecx02[$p]=$X;;
          esac
          ;;
        0x0000001f)
          case "${BASH_REMATCH[2]}" in
            0x01) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax1F_ecx01[$p]=$X;;
          esac
          ;;
        0x80000000) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax80000000[$p]=$X;;
        0x80000001) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax80000001[$p]=$X;;
        0x80000002) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax80000002[$p]=$X;;
        0x80000003) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax80000003[$p]=$X;;
        0x80000004) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax80000004[$p]=$X;;
        0x80000005) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax80000005[$p]=$X;;
        0x80000006) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax80000006[$p]=$X;;
        0x80000007) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax80000007[$p]=$X;;
        0x80000008) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax80000008[$p]=$X;;
        0x80860000) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eax80860000[$p]=$X;;
        0xc0000000) _cpuid_extract X "${BASH_REMATCH[3]}"; cpuid_eaxc0000000[$p]=$X;;
      esac
    }
  done <<< "`cpuid -r`"
}

# Output the CPUID values for each physical cpu.
PrintCPUID() {
  local i
  local l=0
  for i in ${!cpuid_model[@]}; do
    GetLogicalCpu $i 0 l
    echo "Package-$i:"
    echo "  00000000 00 ${cpuid_eax00_ecx00[$l]}"
    echo "  00000001 00 ${cpuid_eax01_ecx00[$l]}"
    echo "  00000002 00 ${cpuid_eax02_ecx00[$l]}"
    echo "  00000003 00 ${cpuid_eax03_ecx00[$l]}"
    echo "  00000004 00 ${cpuid_eax04_ecx00[$l]}"
    echo "  00000004 01 ${cpuid_eax04_ecx01[$l]}"
    echo "  00000004 02 ${cpuid_eax04_ecx02[$l]}"
    echo "  00000004 03 ${cpuid_eax04_ecx03[$l]}"
    echo "  00000005 00 ${cpuid_eax05_ecx00[$l]}"
    echo "  00000006 00 ${cpuid_eax06_ecx00[$l]}"
    echo "  00000007 00 ${cpuid_eax07_ecx00[$l]}"
    echo "  00000008 00 ${cpuid_eax08_ecx00[$l]}"
    echo "  00000009 00 ${cpuid_eax09_ecx00[$l]}"
    echo "  0000000a 00 ${cpuid_eax0A_ecx00[$l]}"
    echo "  0000000b 00 ${cpuid_eax0B_ecx00[$l]}"
    echo "  0000000b 01 ${cpuid_eax0B_ecx01[$l]}"
    echo "  0000000c 00 ${cpuid_eax0C_ecx00[$l]}"
    echo "  0000000d 00 ${cpuid_eax0D_ecx00[$l]}"
    echo "  0000000d 01 ${cpuid_eax0D_ecx01[$l]}"
    echo "  0000000d 02 ${cpuid_eax0D_ecx02[$l]}"
    echo "  0000001f 01 ${cpuid_eax1F_ecx01[$l]}"
    echo "  80000000 00 ${cpuid_eax80000000[$l]}"
    echo "  80000001 00 ${cpuid_eax80000001[$l]}"
    echo "  80000002 00 ${cpuid_eax80000002[$l]}"
    echo "  80000003 00 ${cpuid_eax80000003[$l]}"
    echo "  80000004 00 ${cpuid_eax80000004[$l]}"
    echo "  80000005 00 ${cpuid_eax80000005[$l]}"
    echo "  80000006 00 ${cpuid_eax80000006[$l]}"
    echo "  80000007 00 ${cpuid_eax80000007[$l]}"
    echo "  80000008 00 ${cpuid_eax80000008[$l]}"
    echo "  80860000 00 ${cpuid_eax80860000[$l]}"
    echo "  c0000000 00 ${cpuid_eaxc0000000[$l]}"
  done
}

GetProcessorsInformation() {
  ParseProcCpuinfo
  ParseCPUID
}

