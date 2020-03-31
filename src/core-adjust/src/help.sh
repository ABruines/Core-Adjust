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
PrintHelp() {
cat <<EOF
Core Adjust - Adjust various settings of Intel Processors.
Copyright (C) 2020, Alexander Bruines <alexander.bruines@gmail.com>

This application is used to apply the processor settings found
in configuration file: '$configuration_file'

There should be no need to run this script directly or
to edit the configuration file by hand.
Please use the 'core-adjust-qt' application instead.

Synopsis:

 $(basename $0) <options>

Options:

 -h --help       - Display this help text and quit.
 --version       - Display version information and exit.

 --boot          - Apply settings, assuming the system is currently booting.
 --resume        - Apply settings, assuming the system is currently resuming.
 --acpi          - Apply settings, assuming an ACPI power event has occurred.

 The options --boot, --resume and --acpi are meant to be used by other
 scripts that are executed upon boot, resume and acpi power events.
 These options apply the processor settings found in the configuration file.

 This script also allows various processor settings to be modified using
 the following commandline options. Most of these are used by the graphical
 user interface (core-adjust-qt) to apply new settings but may also be used
 directly. Please note that settings applied using the commandline interface
 are not saved to the configuration file.

 -v --verbose    - Be increasingly more verbose (up to 2x).
 -d --dry-run    - Do not actualy write to the processor but print what would
                   have been written. Can be used in combination with -v.

 -l --list       - List all processors in the system.
 -c --cpuid      - Print the raw CPUID values for each processor.

 --smt <state>   - Turn Simultaneous multithreading (SMT) on or off.
                   Where <state> is 'on' or 'off'.
                   This disables or enables HyperThreading for all processors.

 -p #            - The ordinal number of the processor to read/write
 --processor #     Where # is the processor number (default = 0, see -l).
                   Applies to all options below this one.

                   You may override the microarchitecture of the selected
                   processor by specifying a microarchitecture name with:

 -m <microarchitecture name>
 --march <microarchitecture name>

                   Where the (case insensitive) microarchitecture name is
                   one of the following:

                     'Core', 'Core2', 'Nehalem', 'Westmere', 'Sandybridge',
                     'Ivybridge', 'Haswell', 'Broadwell', 'Skylake',
                     'Kabylake', 'Cannonlake', 'Icelake', 'Tigerlake',
                     'Cometlake', 'Bonnell', 'Saltwell', 'Silvermont',
                     'Airmont', 'Goldmont', 'Tremont', 'Knights Landing' or
                     'Knights Mill'.

                   The --march option only has effects when setting FIVR
                   voltage offsets and/or Target Temperature.
                   Names that contain spaces must be quoted.

 -r --read       - Read FIVR voltage offsets and Target Temperature
                   information from the selected processor.

 To manualy set a Voltage Offset for the various voltage planes
 reported by --read:

  -p0 <value>  or  --plane0  <value>
  -p1 <value>  or  --plane1  <value>
  -p2 <value>  or  --plane2  <value>
  -p3 <value>  or  --plane3  <value>
  -p4 <value>  or  --plane4  <value>
  -p5 <value>  or  --plane5  <value>

  Where <value> is a number of millivolts in-between -999.0234375 and
  +999.0234375. (The default value for all voltage planes is 0 mV.)
  The part of the processor that each voltage plane controls may differ
  between microarchitectures.

  Note that only Haswell and later microarchitectures support setting
  FIVR voltage offsets.

 -f --force      - Allow positive values when setting voltage offsets
                   using the commandline OR force applying settings
                   if used in combination with --boot.

 -t <value>      - Manualy set the Target Temperature for the processor
 --temp <value>    where <value> is a whole number (°C).
                   The minimum and maximum value depends on the
                   microarchitecture of the processor.

 --htt-detect    - Display hyperthreading support and state for the
                   selected processor.
 --htt <state>   - Turn Hyperthreading (HTT) on or off for the
                   selected processor. Where <state> is 'on' or 'off'.
                   (The --smt option is preferred over this option.)

 --tm1-select    - Reset bit 16 of MSR_THERM2_CTL selecting TM1.
 --tm2-select    - Set bit 16 of MSR_THERM2_CTL selecting TM2.

 --fs-enable     - Set bit 0 of IA32_MISC_ENABLE enabling Fast-Strings.
 --fs-disable    - Reset bit 0 of IA32_MISC_ENABLE disabling Fast-Strings.

 --hwp-disable   - Set bit 9 of IA32_MISC_ENABLE disabling the
                   Hardware Prefetcher.
 --hwp-enable    - Reset bit 9 of IA32_MISC_ENABLE enabling the
                   Hardware Prefetcher.

 --ferr-enable   - Set bit 10 of IA32_MISC_ENABLE enabling FERR# Multiplexing.
 --ferr-disable  - Reset bit 10 of IA32_MISC_ENABLE disabling FERR# Multiplexing

 --tm2-enable    - Set bit 13 of IA32_MISC_ENABLE enabling TM2.
 --tm2-disable   - Reset bit 13 of IA32_MISC_ENABLE disabling TM2.

 --eist-enable   - Set bit 16 of IA32_MISC_ENABLE.
                   Enables 'Enhanced Intel Speedstep Technology'.
 --eist-disable  - Reset bit 16 of IA32_MISC_ENABLE.
                   Disables 'Enhanced Intel Speedstep Technology'.

 --fsm-enable    - Set bit 18 of IA32_MISC_ENABLE.
                   Enables MONITOR FSM.
 --fsm-disable   - Reset bit 18 of IA32_MISC_ENABLE.
                   Disables MONITOR FSM.

 --adj-clp-disable - Set bit 19 of IA32_MISC_ENABLE.
                     Disables Adjacent Cache Line Prefetch.
 --adj-clp-enable  - Reset bit 19 of IA32_MISC_ENABLE.
                     Enables Adjacent Cache Line Prefetch.

 --eist-lock     - Lock bit 16 of IA32_MISC_ENABLE by setting bit 20.
                   Locks the current EIST state.

 --cpuid-max-enable  - Set bit 22 of IA32_MISC_ENABLE.
                       Limit the maximum CPUID value.
 --cpuid-max-disable - Reset bit 22 of IA32_MISC_ENABLE.
                       Do not limit the maximum CPUID value.

 --xtpr-msg-disable - Set bit 23 of IA32_MISC_ENABLE.
                      Disables xTPR Message.
 --xtpr-msg-enable  - Reset bit 23 of IA32_MISC_ENABLE.
                      Enables xTPR Message.

 --xd-bit-disable - Set bit 34 of IA32_MISC_ENABLE.
                    Disables the XD bit.
 --xd-bit-enable  - Reset bit 34 of IA32_MISC_ENABLE.
                    Enables the XD bit.

 --dcup-disable  - Set bit 37 of IA32_MISC_ENABLE.
                   Disables the DCU Prefetcher.
 --dcup-enable   - Reset bit 37 of IA32_MISC_ENABLE.
                   Enables the DCU Prefetcher.

 --tbt-disable   - Set bit 38 of IA32_MISC_ENABLE.
                   Disables 'Turbo Boost Technology'.
 --tbt-enable    - Reset bit 38 of IA32_MISC_ENABLE.
                   Enables 'Turbo Boost Technology'.

 --ipp-disable   - Set bit 39 of IA32_MISC_ENABLE.
                   Disables the IP Prefetcher.
 --ipp-enable    - Reset bit 39 of IA32_MISC_ENABLE.
                   Enables the IP Prefetcher.

 --tbt-activation-ratio <value>
                 - Program the value of MSR_TURBO_ACTIVATION_RATIO
                   where <value> is the turbo ratio to program.

 --tbt-activation-ratio-lock
                 - Lock the value of MSR_TURBO_ACTIVATION_RATIO

 -1c <value>     - Set the maximum turbo ratio for 1 active core.
 -2c <value>     - Set the maximum turbo ratio for 2 active cores.
 ...             - ...
 -18c <value>    - Set the maximum turbo ratio for 18 active cores.
                   Where <value> is the desired ratio.

 --fscale <logical cpu> <governor> <min-freq> <max-freq>
 --fscale <logical cpu> userspace <freq>
                 - This option sets the frequency scaling options for
                   the specified logical cpu.

 --fscale-all    - Applies the frequency scaling settings that are stored
                   in the configuration file.

License:

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

EOF
}

