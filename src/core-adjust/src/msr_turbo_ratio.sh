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
# Program MSR_TURBO_ACTIVATION_RATIO.MAX_NON_TURBO_RATIO for a single processor.
#
# $1 == physical cpu number
# returns 0 on success
#
Prg_TBT_activation_ratio() {
  local org
  local dest
  local -i logical
  [ "${tbt_activation_ratio_enable[$1]}" = "false" ] && {
    [ $isVerbose -gt 0 ] && {
      echo "  - Adjusting MAX_NON_TURBO_RATIO is disabled"
    }
  } || {
    [ $isVerbose -gt 0 ] && {
      echo "  - MAX_NON_TURBO_RATIO (${tbt_activation_ratio[$1]})"
    }
    GetLogicalCpu $1 0 logical || return 1
    org=`rdmsr -p $logical -u 1612`
    [ $? -ne 0 ] && {
      >&2 echo "Failed to read MSR_TURBO_ACTIVATION_RATIO."
      return 1
    }
    dest=$(((org & ~255) | (tbt_activation_ratio[$1] & 255)))
    [ $org -ne $dest ] && {
      [ $isDryRun -ne 0 ] && {
        >&2 printf "  dry-run: wrmsr -p %d 1612 0x%X\n" $logical $dest
      } || {
        wrmsr -p $logical 1612 `printf "0x%X" $dest`
        [ $? -ne 0 ] && {
          >&2 echo "Failed to write MSR_TURBO_ACTIVATION_RATIO."
          return 1
        }
      }
    }
  }
  return 0
}

# Program MSR_TURBO_ACTIVATION_RATIO.TURBO_ACTIVATION_RATIO_Lock for a single processor.
#
# $1 == physical cpu number
# returns 0 on success
#
Prg_TBT_activation_ratio_lock() {
  local org
  local dest
  local logical
  [ "${tbt_activation_ratio_lock_enable[$1]}" = "false" ] && {
    [ $isVerbose -gt 0 ] && {
      echo "  - Adjusting TURBO_ACTIVATION_RATIO_Lock is disabled"
    }
  } || {
    [ $isVerbose -gt 0 ] && {
      echo "  - TURBO_ACTIVATION_RATIO_Lock (${tbt_activation_ratio_lock[$1]})"
    }
    GetLogicalCpu $1 0 logical || return 1
    org=`rdmsr -p $logical -u 1612`
    [ $? -ne 0 ] && {
      >&2 echo "Failed to read MSR_TURBO_ACTIVATION_RATIO."
      return 1
    }
    [ "${tbt_activation_ratio_lock[$1]}" = "true" ] && {
      dest=$((org | (2**31)))
    }
    [ $org -ne $dest ] && {
      [ $isDryRun -ne 0 ] && {
        >&2 printf "  dry-run: wrmsr -p %d 1612 0x%X\n" $logical $dest
      } || {
        wrmsr -p $logical 1612 `printf "0x%X" $dest`
        [ $? -ne 0 ] && {
          >&2 echo "Failed to write MSR_TURBO_ACTIVATION_RATIO."
          return 1
        }
      }
    }
  }
  return 0
}

# Program MSR_TURBO_RATIO_LIMIT for a single processor.
#
# $1 == physical cpu number
# returns 0 on success
#
Prg_TBT_ratio_limit() {
  local -i logical
  local -i core
  local -i semaphore
  local limit
  local limit1
  local limit2
  local limit3
  local org
  local org1
  local org2
  local org3
  #
  # Adjust turbo ratio limits?
  [ "${tbt_ratio_limit_enable[$1]}" = "false" ] && {
    [ $isVerbose -gt 0 ] && {
      echo "  - Adjusting MSR_TURBO_RATIO_LIMIT is disabled"
    }
  } || {
    [ $isVerbose -gt 0 ] && {
      echo "  - MSR_TURBO_RATIO_LIMIT"
    }
    #
    # Get the first logical cpu for this processor
    GetLogicalCpu $1 0 logical || return 1
    #
    # The default is no semaphore required for activation
    semaphore=0
    #
    # read MSR_TURBO_RATIO_LIMIT
    limit=`rdmsr -p $logical -u 429 2>&1`
    [ $? -ne 0 ] && {
      >&2 echo "Failed to read MSR_TURBO_RATIO_LIMIT"
      return 1
    }
    #
    # more then 8 cores?
    [ ${cpuid_num_cores[$1]} -gt 8 ] && {
      # yes, read MSR_TURBO_RATIO_LIMIT1
      limit1=`rdmsr -p $logical -u 430 2>&1`
      [ $? -ne 0 ] && {
        >&2 echo "Failed to read MSR_TURBO_RATIO_LIMIT1"
        return 1
      }
    }
    #
    # Try to read MSR_TURBO_RATIO_LIMIT2.
    limit2=`rdmsr -p $logical -u 431 2>&1`
    [ $? -eq 0 ] && {
      # If we can read then use its semaphore bit.
      semaphore=2
    } || {
      # Reset limit2, it contains the error message due to 2>&1
      limit2=0
      # Not being able to read is an error
      # when the processor has more then 16 cores.
      [ ${cpuid_num_cores[$1]} -gt 16 ] && {
        >&2 echo "Failed to read MSR_TURBO_RATIO_LIMIT2"
        return 1
      }
    }
    #
    # Fam6h models 56h and 4Fh have MSR_TURBO_RATIO_LIMIT3 (?)
    #[[ ${cpuid_model[$1]} -eq 0x4F || ${cpuid_model[$1]} -eq 0x56 ]] && {
      limit3=`rdmsr -p $logical -u 428 2>&1`
      [ $? = 0 ] && {
        # If we can read then use its semaphore bit.
        semaphore=3
      } || {
        # Reset limit3, it contains the error message due to 2>&1
        limit3=0
      }
    #}
    #
    # Preserve the values read from the MSRs
    org=$limit
    org1=$limit1
    org2=$limit2
    org3=$limit3
    #
    # Set the new ratio limits using our configuration data
    for core in `seq 1 ${cpuid_num_cores[$1]}`; do
      case $core in
      1) [ ${tbt_ratio_limit_1c[$1]} -gt 0 ] && \
           limit=$(( (limit & ~255) | (tbt_ratio_limit_1c[$1] & 255) ));;
      2) [ ${tbt_ratio_limit_2c[$1]} -gt 0 ] && \
           limit=$(( (limit & ~(255 << 8)) | ((tbt_ratio_limit_2c[$1] & 255) << 8) ));;
      3) [ ${tbt_ratio_limit_3c[$1]} -gt 0 ] && \
           limit=$(( (limit & ~(255 << 16)) | ((tbt_ratio_limit_3c[$1] & 255) << 16) ));;
      4) [ ${tbt_ratio_limit_4c[$1]} -gt 0 ] && \
           limit=$(( (limit & ~(255 << 24)) | ((tbt_ratio_limit_4c[$1] & 255) << 24) ));;
      5) [ ${tbt_ratio_limit_5c[$1]} -gt 0 ] && \
           limit=$(( (limit & ~(255 << 32)) | ((tbt_ratio_limit_5c[$1] & 255) << 32) ));;
      6) [ ${tbt_ratio_limit_6c[$1]} -gt 0 ] && \
           limit=$(( (limit & ~(255 << 40)) | ((tbt_ratio_limit_6c[$1] & 255) << 40) ));;
      7) [ ${tbt_ratio_limit_7c[$1]} -gt 0 ] && \
           limit=$(( (limit & ~(255 << 48)) | ((tbt_ratio_limit_7c[$1] & 255) << 48) ));;
      8) [ ${tbt_ratio_limit_8c[$1]} -gt 0 ] && \
           limit=$(( (limit & ~(255 << 56)) | ((tbt_ratio_limit_8c[$1] & 255) << 56) ));;
      9) [ ${tbt_ratio_limit_9c[$1]} -gt 0 ] && \
           limit1=$(( (limit1 & ~255) | (tbt_ratio_limit_9c[$1] & 255) ));;
      10) [ ${tbt_ratio_limit_10c[$1]} -gt 0 ] && \
            limit1=$(( (limit1 & ~(255 << 8)) | ((tbt_ratio_limit_10c[$1] & 255) << 8) ));;
      11) [ ${tbt_ratio_limit_11c[$1]} -gt 0 ] && \
            limit1=$(( (limit1 & ~(255 << 16)) | ((tbt_ratio_limit_11c[$1] & 255) << 16) ));;
      12) [ ${tbt_ratio_limit_12c[$1]} -gt 0 ] && \
            limit1=$(( (limit1 & ~(255 << 24)) | ((tbt_ratio_limit_12c[$1] & 255) << 24) ));;
      13) [ ${tbt_ratio_limit_13c[$1]} -gt 0 ] && \
            limit1=$(( (limit1 & ~(255 << 32)) | ((tbt_ratio_limit_13c[$1] & 255) << 32) ));;
      14) [ ${tbt_ratio_limit_14c[$1]} -gt 0 ] && \
            limit1=$(( (limit1 & ~(255 << 40)) | ((tbt_ratio_limit_14c[$1] & 255) << 40) ));;
      15) [ ${tbt_ratio_limit_15c[$1]} -gt 0 ] && \
            limit1=$(( (limit1 & ~(255 << 48)) | ((tbt_ratio_limit_15c[$1] & 255) << 48) ));;
      16) [ ${tbt_ratio_limit_16c[$1]} -gt 0 ] && \
            limit1=$(( (limit1 & ~(255 << 56)) | ((tbt_ratio_limit_16c[$1] & 255) << 56) ));;
      17) [ ${tbt_ratio_limit_17c[$1]} -gt 0 ] && \
            limit2=$(( (limit2 & ~255) | (tbt_ratio_limit_17c[$1] & 255) ));;
      18) [ ${tbt_ratio_limit_18c[$1]} -gt 0 ] && \
            limit2=$(( (limit2 & ~(255 << 8)) | ((tbt_ratio_limit_18c[$1] & 255) << 8) ));;
      esac
    done
    #
    # Set the correct semaphore bit if required.
    [ $semaphore -eq 2 ] && {
      limit2=$((limit2 | (2**63)))
    }
    [ $semaphore -eq 3 ] && {
      limit3=$((limit3 | (2**63)))
    }
    #
    # Write the MSRs if the value differs from the original.
    [ $limit -ne $org ] && {
      # write MSR_TURBO_RATIO_LIMIT:
      [ $isDryRun -ne 0 ] && {
        >&2 printf "  dry-run: wrmsr -p %d 429 0x%X\n" $logical $limit
      } || {
        wrmsr -p $logical 429 `printf "0x%X" $limit`
        [ $? -ne 0 ] && {
          >&2 echo "Failed to write MSR_TURBO_RATIO_LIMIT"
          return 1
        }
      }
    }
    # more then 8 cores?
    [ ${cpuid_num_cores[$1]} -gt 8 ] && {
      # yes, write MSR_TURBO_RATIO_LIMIT1
      [ $limit1 -ne $org1 ] && {
        [ $isDryRun -ne 0 ] && {
          >&2 printf "  dry-run: wrmsr -p %d 430 0x%X\n" $logical $limit1
        } || {
          wrmsr -p $logical 430 `printf "0x%X" $limit1`
          [ $? -ne 0 ] && {
            >&2 echo "Failed to write MSR_TURBO_RATIO_LIMIT1"
            return 1
          }
        }
      }
    }
    # more then 16 cores or semaphore==2
    [[ ${cpuid_num_cores[$1]} -gt 16 || $semaphore -eq 2 ]] && {
      # yes, write MSR_TURBO_RATIO_LIMIT2
      [ $limit2 -ne $org2 ] && {
        [ $isDryRun -ne 0 ] && {
          >&2 printf "  dry-run: wrmsr -p %d 431 0x%X\n" $logical $limit2
        } || {
          wrmsr -p $logical 431 `printf "0x%X" $limit2`
          [ $? -ne 0 ] && {
            >&2 echo "Failed to write MSR_TURBO_RATIO_LIMIT2"
            return 1
          }
        }
      }
    }
    # semaphore==3 ?
    [[ $semaphore -eq 3 && $limit3 -ne $org3 ]] && {
      # yes, write MSR_TURBO_RATIO_LIMIT3
      [ $isDryRun -ne 0 ] && {
        >&2 printf "  dry-run: wrmsr -p %d 428 0x%X\n" $logical $limit3
      } || {
        wrmsr -p $logical 428 `printf "0x%X" $limit3`
        [ $? -ne 0 ] && {
          >&2 echo "Failed to write MSR_TURBO_RATIO_LIMIT3"
          return 1
        }
      }
    }
  }
  return 0
}

