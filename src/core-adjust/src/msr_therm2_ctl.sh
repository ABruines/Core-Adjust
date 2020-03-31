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
# Program MSR_THERM2_CTL for a single processor.
#
# $1 == physical cpu number
# returns 0 on success
#
Prg_TM_SELECT() {
  local -i org
  local -i dest
  local -i logical
  [ "${tm_select_enable[$1]}" = "false" ] && {
    [ $isVerbose -gt 0 ] && {
      echo "  - Adjusting TM_SELECT is disabled"
    }
  } || {
    [ $isVerbose -gt 0 ] && {
      echo "  - TM_SELECT (${tm_select[$1]})"
    }
    GetLogicalCpu $1 0 logical || return 1
    org=`rdmsr -p $logical -u 413`
    [ $? -ne 0 ] && {
      >&2 echo "Failed to read MSR_THERM2_CTL."
      return 1
    }
    [ "${tm_select[$1]}" = "true" ] && {
      # set TM_SELECT
      dest=$(($org | (2**16)))
    } || {
      # reset TM_SELECT
      dest=$(($org & ~(2**16)))
    }
    [ $org -ne $dest ] && {
      [ $isDryRun -ne 0 ] && {
        >&2 printf "  dry-run: wrmsr -p %d 413 0x%X\n" $logical $dest
      } || {
        wrmsr -p $logical 413 `printf "0x%X" $dest`
        [ $? -ne 0 ] && {
          >&2 echo "Failed to write MSR_THERM2_CTL."
          return 1
        }
      }
    }
  }
  return 0
}

