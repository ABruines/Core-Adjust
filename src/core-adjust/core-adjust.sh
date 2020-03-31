#!/bin/bash

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

. src/debug.sh
. src/main.sh
. src/cpu-info.sh
. src/actions.sh
. src/cmdline.sh
. src/configuration.sh
. src/target-temp.sh
. src/fivr-voffset.sh
. src/smt.sh
. src/msr_turbo_ratio.sh
. src/ia32_misc_enable.sh
. src/msr_therm2_ctl.sh
. src/freq-scale.sh
. src/help.sh
MinimalBashVersion() {
  local -i major=`echo $BASH_VERSION | cut -d '.' -f 1`
  local -i minor=`echo $BASH_VERSION | cut -d '.' -f 2`
  [[ $major -lt $1 || ($major -eq $1 && $minor -lt $2) ]] && {
    echo -e "This script requires bash $1.$2 or later ($BASH_VERSION).\n"
    return 1
  }
  return 0
}

MinimalBashVersion 4 3 || exit 1
Main "$@"

