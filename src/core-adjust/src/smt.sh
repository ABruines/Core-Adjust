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
# Expand a number sequence like '0-3,5,7-9'
# into an array like '0 1 2 3 5 7 8 9'
# $1 == the input sequence.
# $2 == reference to the array receiving the output.
# returns: nothing
ExpandSequence() {
  local -n RESULT=$2
  # Do not operate on the RESULT array directly as it may be declared as
  # an integer type and we initialy have strings.
  local -a a
  local i
  unset RESULT
  [ "x$1" != "x" ] && {
    # Split the input at each ',' and store
    # each 'word' in-between into an array.
    IFS=',' read -a a <<< "$1"
    # Expand every sequence in the array
    for i in ${!a[@]}; do
      [[ ${a[$i]} =~ ([[:digit:]]+)\-([[:digit:]]+) ]] && {
        a[$i]=`eval echo {${BASH_REMATCH[1]}..${BASH_REMATCH[2]}}`
      }
    done
    # Expand the array into the output array
    RESULT=(${a[@]})
  }
}

# Detect the number of cores and logical cpus a processor has.
#
# $1 == The physical processor id
# $2 == Reference to the variable that receivces the number of cores detected.
# $3 == Reference to the variable that receivces the number of logical cpus detected.
#
# Returns 0 if the output is valid.
#
# This function works as follows:
#
#  First test CPUID.01H.EDX[28] to determine if CPUID.01H.EBX[23:16]
#  is valid.
#
#  Next we retrieve the 'maximum number of addressable IDs for
#  logical processors in this package' from CPUID.01H.EBX[23:16]
#  and assume this equals the number of cores in this processor.
#
#  This should be enough for older processors (that are not supported
#  by this script).
#  More recent processors have gaps in their (x2)APIC ID space,
#  meaning that the number of cores we retrieved from CPUID.01H.EBX[23:16]
#  needs to be adjusted in order to get the correct core count.
#  All these newer processors however provide access to CPUID
#  leaf 0x0B and/or 0x1F (assumption).
#
#  CPUID leaf 0x0B and 0x1F contain information about the topology
#  of the processor. Both leafs report the width of the gaps in the
#  x2APIC ID space in the EAX register (bits 4:0).
#  The EBX register (bits 15:0) reports the (factory set) number of
#  threads this processor has.
#
#  Intel states that when CPUID leaf 0x1F exists, it is preffered
#  over leaf 0x0B. (My current cpu does not have leaf 1F so I cannot
#  test this.)
#
GetHyperThreadingCoreCount() {
  local -n CORE_COUNT=$2
  local -n THREAD_COUNT=$3
  local -a regs
  local -a topology
  local div
  # Get the 1st logical cpu nr of this processor
  local l
  GetLogicalCpu $1 0 l
  # Read cpuid.01H output registers to array regs, the
  # resulting indices are: 0==EAX, 1==EBX, 2==ECX, 3==EDX.
  # The array values are hex strings, we must prepend '0x' before
  # doing any arithmatic with them.
  regs=(${cpuid_eax01_ecx00[$l]})
  # Test CPUID.01H.EDX[28]
  [ $((0x${regs[3]} & (1 << 28))) != 0 ] && {
    # Get the number of cores from CPUID.01H.EBX[23:16]
    CORE_COUNT=$(((0x${regs[1]} >> 16) & 255))
    THREAD_COUNT=${cpuid_siblings[$1]}
    # If CPUID.1FH or CPUID.0BH exist then adjust CORE_COUNT
    # for the gaps in the APIC ID space and read THREAD_COUNT
    # from that CPUID leaf instead.
    #
    # Test if CPUID leaf 1F sub-leaf 1 exists and its contents is valid.
    regs=(${cpuid_eax00_ecx00[$l]})
    topology=(${cpuid_eax1F_ecx01[$l]})
    [[ $((0x${regs[0]} >= 0x1F)) -ne 0 && $(((0x${topology[2]} >> 8) & 255)) -ne 0 ]] && {
      # Leaf 0x1F exists and is valid,
      # divide the number of cores by the gap size in the x2APIC ID space.
      div=$((0x${topology[0]} & 15))
      [ $div -gt 1 ] && CORE_COUNT=$((CORE_COUNT / div))
      # Get the number of logical cpus
      THREAD_COUNT=$((0x${topology[1]} & 0xffff))
    } || {
      # Leaf 0x1F does not exist or is invalid.
      # Does CPUID leaf 0x0B sub-leaf 1 exist and its contents valid?
      topology=(${cpuid_eax0B_ecx01[$l]})
      [[ $((0x${regs[0]} >= 0x0B)) -ne 0 && $(((0x${topology[2]} >> 8) & 255)) -ne 0 ]] && {
        # Leaf 0x0B exists and is valid,
        # divide the number of cores by the gap size in the x2APIC ID space.
        div=$((0x${topology[0]} & 15))
        [ $div -gt 1 ] && CORE_COUNT=$((CORE_COUNT / div))
        # Get the number of logical cpus
        THREAD_COUNT=$((0x${topology[1]} & 0xffff))
      }
    }
    return 0
  }
  return 1
}

# --htt-detect
# $1 == physical cpu nr
# returns: nothing
PrintHyperThreadingSupport() {
  local -i ncores=0
  local -i nthreads=0
  HaveSMT
  [ $? -eq 0 ] && {
    echo "Global SMT control        : yes" 
    local -i vb=$isVerbose
    isVerbose=0
    TestSMT
    [ $? -eq 0 ] \
      && echo "Global SMT state          : disabled" \
      || echo "Global SMT state          : enabled"
    isVerbose=$vb
  } || {
    echo "Global SMT control        : no"
    echo "Global SMT state          : disabled"
  }
  GetHyperThreadingCoreCount $1 ncores nthreads
  [ $? -eq 0 ] && {
    [ $ncores -ne $nthreads ] \
      && echo "Processor #$1 supports HTT : yes" \
      || echo "Processor #$1 supports HTT : no"
    [[ $nthreads -eq ${cpuid_siblings[$1]} && $nthreads -gt $ncores ]] \
      && echo "Processor #$1 HTT state    : enabled" \
      || echo "Processor #$1 HTT state    : disabled"
    echo "Processor #$1 cores        : $ncores"
    echo "Processor #$1 threads      : $nthreads"
  } || {
    echo "Processor #$1 supports HTT : no"
    echo "Processor #$1 HTT state    : disabled"
    echo "Processor #$1 cores        : ${cpuid_num_cores[$1]}"
    echo "Processor #$1 threads      : ${cpuid_siblings[$1]}"
  }
}

# Sysfs has SMT constrol?
# args: none
# returns: 0 on success
HaveSMT() {
  [ -d /sys/devices/system/cpu/smt ] && {
    return 0
  }
  return 1
}

# Test if SMT is enabled
# args: none
# returns: 1 if SMT is enabled, 0 if it is not.
TestSMT() {
  local active
  [ -f /sys/devices/system/cpu/smt/active ] && {
    read active < /sys/devices/system/cpu/smt/control
    [ "$active" = "off" ] && {
      # SMT is disabled
      [ $isVerbose -gt 0 ] && {
        echo -n "SMT is currently disabled."
      }
      return 0
    }
  }
  # SMT is enabled
  [ $isVerbose -gt 0 ] && {
    echo -n "SMT is currently enabled."
  }
  return 1
}

# Enable SMT and HTT all processors
# args: none
# returns: 0 on success
EnableSMT() {
  local t
  local x
  # SMT supported?
  HaveSMT || {
    >&2 echo "Cannot enable SMT, /sys/devices/system/cpu/smt does not exist!"
    return 1
  }
  # SMT enabled?
  TestSMT && {
    [ $isVerbose -gt 0 ] && {
      echo " Enabling SMT..."
    }
    [ -f /sys/devices/system/cpu/smt/control ] && {
      # First enable SMT globally
      [ $isDryRun -ne 0 ] && {
        # dry-run
        >&2 echo "dry-run: echo on > /sys/devices/system/cpu/smt/control"
      } || {
        # not a dry-run
        echo on > /sys/devices/system/cpu/smt/control
      }
      [ -e /usr/sbin/irqbalance ] && {
        [ $isVerbose -gt 0 ] && echo "Rebalance IRQs..."
        [ $isDryRun -ne 0 ] && {
          # dry-run
          >&2 echo "dry-run: /usr/sbin/irqbalance --oneshot"
        } || {
          # not a dry-run
          /usr/sbin/irqbalance --oneshot
        }
      }
      GetProcessorsInformation
      # Also enable HTT per cpu
      for t in ${!cpuid_logical_cpus[@]}; do
        [ $((${cpuid_num_cores[$t]} * 2)) -ne ${cpuid_siblings[$t]} ] && {
          EnableLogicalCores $t
        }
      done
      return 0
    }
    return 1
  } || {
    # Terminate the line TestSMT has printed
    [ $isVerbose -gt 0 ] && {
      echo ""
    }
    # Allready enabled globally, also enable HTT per cpu
    for t in ${!cpuid_logical_cpus[@]}; do
      [ $((${cpuid_num_cores[$t]} * 2)) -ne ${cpuid_siblings[$t]} ] && {
        EnableLogicalCores $t
      }
    done
  }
  #
  return 0
}

# Disable SMT for all processors
# args: none
# returns: 0 on success
DisableSMT() {
  HaveSMT || {
    >&2 echo "Cannot disable SMT, /sys/devices/system/cpu/smt does not exist!"
    return 1
  }
  TestSMT || {
    [ $isVerbose -gt 0 ] && {
      echo " Disabling SMT..."
    }
    [ -f /sys/devices/system/cpu/smt/control ] && {
      [ $isDryRun -ne 0 ] && {
        >&2 echo "dry-run: echo off > /sys/devices/system/cpu/smt/control"
      } || {
        echo off > /sys/devices/system/cpu/smt/control
      }
      [ -e /usr/sbin/irqbalance ] && {
        [ $isVerbose -gt 0 ] && echo "Rebalance IRQs..."
        [ $isDryRun -ne 0 ] && {
          >&2 echo "dry-run: /usr/sbin/irqbalance --oneshot"
        } || {
          /usr/sbin/irqbalance --oneshot
        }
      }
      GetProcessorsInformation
      return 0
    }
    return 1
  }
  [ $isVerbose -gt 0 ] && {
    echo ""
  }
  return 0
}

# Enable HTT for a given processor
# $1 == The psysical processor id (0...)
# Returns 0 on success, 1 on failure.
EnableLogicalCores() {
  local -i -a logical
  local -i balance=0
  local -i ncores
  local -i nthreads
  local -i i
  local -i j
  local +i s
  local -i online=0
  #
  # Get the number of cores and the number of factory set threads
  # for the first processor.
  GetHyperThreadingCoreCount 0 ncores nthreads || {
    >&2 echo "Could not get processor core count."
    return 1
  }
  # The cpu must support hyperthreading
  [ $ncores -eq $nthreads ] && {
    >&2 echo "Processor #0 does not support HyperThreading."
    return 1
  }
  #
  # SMT must be enabled if available
  HaveSMT && {
    TestSMT >/dev/null 2>&1 && {
      >&2 echo "Cannot enable HTT because SMT is disabled, enable it with '--smt on'"
      return 1
    }
  }
  #
  # Get the number of logical cpus listed by sysfs
  local -a present
  [ -f /sys/devices/system/cpu/present ] || {
    >&2 echo "Error, file not found: /sys/devices/system/cpu/present"
    return 1
  }
  ExpandSequence "$(</sys/devices/system/cpu/present)" present
  #
  # System has more than one processor?
  [ $num_physical_cpus -eq 0 ] && {
    #
    # Only one physical procesor.
    # Loop over all 'present' cpus and bring them online
    for i in ${present[@]}; do
      # (un)even numbered logical cpu?
      #[ $((i & 1)) -ne 0 ] && {
      #  # even
      #} || {
      #  # uneven
      #}
      [ -f /sys/devices/system/cpu/cpu$i/topology/core_id ] || {
        [ $isVerbose -gt 0 ] && echo "Bringing cpu #$i online."
        [ $isDryRun -ne 0 ] && {
          >&2 echo "dry-run: echo 1 > /sys/devices/system/cpu/cpu$i/online"
        } || {
          echo 1 > /sys/devices/system/cpu/cpu$i/online
        }
        # signal to rebalance irqs
        balance=1
        online=$((online + 1))
      }
    done
  } || {
    #
    # More then one physical procesor.
    # We must assume that they are identical, ie. they have the same
    # CPUID model (and thus the same number of cores/threads).
    # (The reason for this is that all cpus except cpu0 may be taken
    # offline. In that case the only CPUID information available is
    # that for cpu0 and we should not query the others by bringing
    # them online first.)
    #
    # 2D array with an entry for each processor containing all
    # possible logical cpus for that processor
    local -a all_logical
    #
    # Assuming that each processor has the same number of threads
    # generate the logical cpu numbers for all processors.
    for i in ${!cpuid_model[@]}; do
      s=""
      for j in `seq 1 $nthreads`; do
        [ -z "$s" ] && {
          s="$((((i * nthreads) + j - 1)))"
        } || {
          s="$s $((((i * nthreads) + j - 1)))"
        }
      done
      all_logical[$i]=$s
    done
    #
    # The generated list of logical cpus should match
    # the cpus listed in /sys/devices/system/cpu/present 
    [ "${all_logical[*]}" = "${present[*]}" ] || {
      >&2 echo "Internal error, failed to enumerate total number of threads"
      return 1
    }
    #
    # Now enable HTT for the specified processor:
    #
    # Get the logical cpus for the selected processor
    # and loop over and enable all of them.
    logical=(${all_logical[$1]})
    for i in ${logical[@]}; do
      # Is the cpu online?
      [ -f /sys/devices/system/cpu/cpu$i/topology/core_id ] || {
        # not online
        [ $isVerbose -gt 0 ] && echo "Bringing cpu #$i online."
        [ $isDryRun -ne 0 ] && {
          >&2 echo "dry-run: echo 1 > /sys/devices/system/cpu/cpu$i/online"
        } || {
          echo 1 > /sys/devices/system/cpu/cpu$i/online
        }
        # signal to rebalance irqs
        balance=1
        online=$((online + 1))
      }
    done
  }
  [ $online -eq 0 ] && {
    [ $isVerbose -gt 0 ] && echo "All cpus for processor #$1 are allready enabled."
  } || {
    [ $isVerbose -gt 0 ] && echo "$online cpus were brought online."
  }
  #
  # Rebalance IRQs
  [ $balance -ne 0 ] && {
    [ -e /usr/sbin/irqbalance ] && {
      [ $isVerbose -gt 0 ] && echo "Rebalance IRQs..."
      [ $isDryRun -ne 0 ] && {
        >&2 echo "dry-run: /usr/sbin/irqbalance --oneshot"
      } || {
        /usr/sbin/irqbalance --oneshot
      }
    }
    #
    # Reread /proc/cpuinfo
    GetProcessorsInformation
  }
  return 0
}

# Disable HTT for a given processor
# $1 == The psysical processor id (0...)
# Returns 0 on success, 1 on failure.
DisableLogicalCores() {
  local -i -a logical
  local -i balance=0
  local -i ncores
  local -i nthreads
  local -i i
  local -i j
  local +i s
  local -i offline=0
  local -i online=0
  #
  # Get the number of cores and the number of factory set threads
  # for the first processor.
  GetHyperThreadingCoreCount 0 ncores nthreads || {
    >&2 echo "Could not get processor core count."
    return 1
  }
  # The cpu must support hyperthreading
  [ $ncores -eq $nthreads ] && {
    >&2 echo "Processor #0 does not support HyperThreading."
    return 1
  }
  #
  # SMT must be enabled if available
  HaveSMT && {
    TestSMT >/dev/null 2>&1 && {
      >&2 echo "Cannot disable HTT because SMT is disabled, enable it with '--smt on'"
      return 1
    }
  }
  #
  # Get the number of logical cpus listed by sysfs
  local -a present
  [ -f /sys/devices/system/cpu/present ] || {
    >&2 echo "Error, file not found: /sys/devices/system/cpu/present"
    return 1
  }
  ExpandSequence "$(</sys/devices/system/cpu/present)" present
  #
  # System has more than one processor?
  [ $num_physical_cpus -eq 0 ] && {
    #
    # Only one physical procesor.
    # Loop over all 'present' cpus
    for i in ${present[@]}; do
      # (un)even numbered logical cpu?
      [ $((i & 1)) -eq 0 ] && {
        # even cpus must be brought online
        [ -f /sys/devices/system/cpu/cpu$i/topology/core_id ] || {
          [ $isVerbose -gt 0 ] && echo "Bringing cpu #$i online."
          [ $isDryRun -ne 0 ] && {
            >&2 echo "dry-run: echo 1 > /sys/devices/system/cpu/cpu$i/online"
          } || {
            echo 1 > /sys/devices/system/cpu/cpu$i/online
          }
          # signal to rebalance irqs
          balance=1
          online=$((online + 1))
        }
      } || {
        # uneven cpus must be taken offline
        [ -f /sys/devices/system/cpu/cpu${i}/topology/core_id ] && {
          [ $isVerbose -gt 0 ] && echo "Taking cpu #$i offline..."
          [ $isDryRun -ne 0 ] && {
            >&2 echo "dry-run: echo 0 > /sys/devices/system/cpu/cpu${i}/online"
          } || {
            echo 0 > /sys/devices/system/cpu/cpu${i}/online
          }
          balance=1
          offline=$((offline + 1))
        }
      }
    done
  } || {
    #
    # More then one physical procesor.
    # We must assume that they are identical, ie. they have the same
    # CPUID model (and thus the same number of cores/threads).
    # (The reason for this is that all cpus except cpu0 may be taken
    # offline. In that case the only CPUID information available is
    # that for cpu0 and we should not query the others by bringing
    # them online first.)
    #
    # 2D array with an entry for each processor containing all
    # possible logical cpus for that processor
    local -a all_logical
    #
    # Assuming that each processor has the same number of threads
    # generate the logical cpu numbers for all processors.
    for i in ${!cpuid_model[@]}; do
      s=""
      for j in `seq 1 $nthreads`; do
        [ -z "$s" ] && {
          s="$((((i * nthreads) + j - 1)))"
        } || {
          s="$s $((((i * nthreads) + j - 1)))"
        }
      done
      all_logical[$i]=$s
    done
    #
    # The generated list of logical cpus should match
    # the cpus listed in /sys/devices/system/cpu/present 
    [ "${all_logical[*]}" = "${present[*]}" ] || {
      >&2 echo "Internal error, failed to enumerate total number of threads"
      return 1
    }
    #
    # Now disable HTT for the specified processor:
    #
    # Get the logical cpus for the selected processor
    # and loop over them enabling the even numbered ones and
    # disabling the uneven numbered cpus.
    logical=(${all_logical[$1]})
    for i in ${logical[@]}; do
      # (un)even numbered logical cpu?
      [ $((i & 1)) -eq 0 ] && {
        # even cpus must be brought online
        [ -f /sys/devices/system/cpu/cpu$i/topology/core_id ] || {
          [ $isVerbose -gt 0 ] && echo "Bringing cpu #$i online."
          [ $isDryRun -ne 0 ] && {
            >&2 echo "dry-run: echo 1 > /sys/devices/system/cpu/cpu$i/online"
          } || {
            echo 1 > /sys/devices/system/cpu/cpu$i/online
          }
          # signal to rebalance irqs
          balance=1
          online=$((online + 1))
        }
      } || {
        # uneven cpus must be taken offline
        [ -f /sys/devices/system/cpu/cpu${i}/topology/core_id ] && {
          [ $isVerbose -gt 0 ] && echo "Taking cpu #$i offline..."
          [ $isDryRun -ne 0 ] && {
            >&2 echo "dry-run: echo 0 > /sys/devices/system/cpu/cpu${i}/online"
          } || {
            echo 0 > /sys/devices/system/cpu/cpu${i}/online
          }
          balance=1
          offline=$((offline + 1))
        }
      }
    done
  }
  [ $offline -eq 0 ] && {
    [ $isVerbose -gt 0 ] && echo "No cpus were taken offline."
  } || {
    [ $isVerbose -gt 0 ] && echo "$offline cpus were taken offline."
  }
  [ $online -eq 0 ] || {
    [ $isVerbose -gt 0 ] && echo "$online cpus were brought online."
  }
  #
  # Rebalance IRQs
  [ $balance -ne 0 ] && {
    [ -e /usr/sbin/irqbalance ] && {
      [ $isVerbose -gt 0 ] && echo "Rebalance IRQs..."
      [ $isDryRun -ne 0 ] && {
        >&2 echo "dry-run: /usr/sbin/irqbalance --oneshot"
      } || {
        /usr/sbin/irqbalance --oneshot
      }
    }
    #
    # Reread /proc/cpuinfo
    GetProcessorsInformation
  }
  return 0
}

