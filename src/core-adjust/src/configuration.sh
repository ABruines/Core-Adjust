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
# Set the default values for a single cpu
# no return value
_DefaultCpuConfiguration() {
  # FIVR
  allow_overvolting[$1]="false"
  voltage_offset_plane_0_enable[$1]="false"
  voltage_offset_plane_1_enable[$1]="false"
  voltage_offset_plane_2_enable[$1]="false"
  voltage_offset_plane_3_enable[$1]="false"
  voltage_offset_plane_4_enable[$1]="false"
  voltage_offset_plane_5_enable[$1]="false"
  voltage_offset_plane_0[$1]="NaN"
  voltage_offset_plane_1[$1]="NaN"
  voltage_offset_plane_2[$1]="NaN"
  voltage_offset_plane_3[$1]="NaN"
  voltage_offset_plane_4[$1]="NaN"
  voltage_offset_plane_5[$1]="NaN"
  # Target Temperature
  target_temperature_enable[$1]="false"
  target_temperature_battery_enable[$1]="false"
  target_temperature[$1]="NaN"
  target_temperature_battery[$1]="NaN"
  # MSR_THERM2_CTL
  tm_select_enable[$1]="false"
  tm_select[$1]="false"
  # IA32_MISC_ENABLE
  tm2_enable_enable[$1]="false"
  tm2_enable[$1]="false"
  eist_enable_enable[$1]="false"
  eist_enable[$1]="false"
  eist_lock_enable[$1]="false"
  eist_lock[$1]="false"
  tbt_disable_enable[$1]="false"
  tbt_disable[$1]="false"

  fs_enable_enable[$1]="false"
  fs_enable[$1]="false"
  hwp_disable_enable[$1]="false"
  hwp_disable[$1]="false"
  ferr_enable_enable[$1]="false"
  ferr_enable[$1]="false"
  fsm_enable_enable[$1]="false"
  fsm_enable[$1]="false"
  adj_disable_enable[$1]="false"
  adj_disable[$1]="false"
  cpuid_maxval_enable[$1]="false"
  cpuid_maxval[$1]="false"
  xtpr_disable_enable[$1]="false"
  xtpr_disable[$1]="false"
  xd_disable_enable[$1]="false"
  xd_disable[$1]="false"
  dcu_disable_enable[$1]="false"
  dcu_disable[$1]="false"
  ipp_disable_enable[$1]="false"
  ipp_disbale[$1]="false"

  # MSR_TURBO_ACTIVATION_RATIO
  tbt_activation_ratio_enable[$1]="false"
  tbt_activation_ratio[$1]=0
  tbt_activation_ratio_lock_enable[$1]="false"
  tbt_activation_ratio_lock[$1]="false"
  # MSR_TURBO_RATIO_LIMIT
  tbt_ratio_limit_enable[$1]="false"
  tbt_ratio_limit_1c[$1]=0
  tbt_ratio_limit_2c[$1]=0
  tbt_ratio_limit_3c[$1]=0
  tbt_ratio_limit_4c[$1]=0
  tbt_ratio_limit_5c[$1]=0
  tbt_ratio_limit_6c[$1]=0
  tbt_ratio_limit_7c[$1]=0
  tbt_ratio_limit_8c[$1]=0
  tbt_ratio_limit_9c[$1]=0
  tbt_ratio_limit_10c[$1]=0
  tbt_ratio_limit_11c[$1]=0
  tbt_ratio_limit_12c[$1]=0
  tbt_ratio_limit_13c[$1]=0
  tbt_ratio_limit_14c[$1]=0
  tbt_ratio_limit_15c[$1]=0
  tbt_ratio_limit_16c[$1]=0
  tbt_ratio_limit_17c[$1]=0
  tbt_ratio_limit_18c[$1]=0
  # HTT
  htt_disable_enable[$1]="false"
  htt_disable[$1]="false"
}

_DefaultFsConfiguration() {
  local i
  # Frequency Scaling
  fs_enabled_g[$1]="false"
  fs_batt_g[$1]="false"
  fs_per_thread[$1]="false"
  # Frequency Scaling
  for i in ${cpuid_logical_cpus[$1]}; do
    fs_enabled[$i]="false"
    fs_governor_ac[$i]="conservative"
    fs_min_freq_ac[$i]=0
    fs_max_freq_ac[$i]=0
    fs_freq_ac[$i]=0
    fs_batt[$i]="false"
    fs_governor_dc[$i]="conservative"
    fs_min_freq_dc[$i]=0
    fs_max_freq_dc[$i]=0
    fs_freq_dc[$i]=0
  done
}

# Set the default values for all cpus
# no return value
DefaultCpuConfiguration() {
  local i
  apply_on_boot_and_resume="false"
  apply_on_acpi_power_event="false"
  smp_disable_enable="false"
  smp_disable="false"
  per_cpu_htt_enable="false"
  for i in ${!cpuid_model[@]}; do
    _DefaultCpuConfiguration $i
    _DefaultFsConfiguration $i
  done
}

# load the configuration file into variables
# $1 == path to ini file
# returns 0 on success
ParseConfigurationFile() {
  local current_section
  local gui_save
  local i
  local p
  local l
  #
  # The nr of the current 'Processor' section
  local cpu
  local logical
  #
  [ -f $1 ] || {
    >&2 echo -e "No such file: $1\n"
    return 1
  }
  #
  apply_on_boot_and_resume="false"
  apply_on_acpi_power_event="false"
  #
  while read line; do
    # match/capture any section
    [[ $line =~ ^\[(.*)\] ]] && {
      # matched a section
      current_section=${BASH_REMATCH[1]}
      # processor section? then extract the cpu number and initialize its configuration
      [[ $current_section =~ Processor([[:digit:]]+) ]] && {
        current_section="Processor"
        cpu=${BASH_REMATCH[1]}
        _DefaultCpuConfiguration $cpu
      }
      [[ $current_section =~ CpuFreqUtils([[:digit:]]+) ]] && {
        current_section="CpuFreqUtils"
        cpu=${BASH_REMATCH[1]}
        # !!! This works because section CpuFreqUtils0 comes before the
        # !!! CpuFreqUtils_Thread0 sections in the INI file...
        # !!! ie. "_" is 'larger' then "0...9"
        _DefaultFsConfiguration $cpu
      }
      [[ $current_section =~ CpuFreqUtils_Thread([[:digit:]]+) ]] && {
        current_section="CpuFreqUtils_Thread"
        logical=${BASH_REMATCH[1]}
      }
    } || {
      # did not match a section,
      # match/capture any name+value and store
      [[ $line =~ ^(.+)=(.*) ]] && {
        # store the value
        case "$current_section" in
        "Common")
          case "${BASH_REMATCH[1]}" in
          "Apply_On_Boot_And_Resume") apply_on_boot_and_resume=${BASH_REMATCH[2]};;
          "Apply_On_Acpi_Power_Event") apply_on_acpi_power_event=${BASH_REMATCH[2]};;
          "GUI_Save_On_Exit") gui_save=${BASH_REMATCH[2]};;
          # SMP
          "SMT_Disable_Enabled") smp_disable_enable=${BASH_REMATCH[2]};;
          "SMT_Disable") smp_disable=${BASH_REMATCH[2]};;
          # HTT
          "CPU_HTT_Enable") per_cpu_htt_enable=${BASH_REMATCH[2]};;
          *) >&2 echo "Warning: Ignoring unknown variable Common::${BASH_REMATCH[1]}";;
          esac
          ;;
        "Processor")
          case ${BASH_REMATCH[1]} in
          # FIVR
          "Allow_Overvolting") allow_overvolting[$cpu]=${BASH_REMATCH[2]};;
          "Voltage_Offset_Plane_0_Enabled") voltage_offset_plane_0_enable[$cpu]=${BASH_REMATCH[2]};;
          "Voltage_Offset_Plane_1_Enabled") voltage_offset_plane_1_enable[$cpu]=${BASH_REMATCH[2]};;
          "Voltage_Offset_Plane_2_Enabled") voltage_offset_plane_2_enable[$cpu]=${BASH_REMATCH[2]};;
          "Voltage_Offset_Plane_3_Enabled") voltage_offset_plane_3_enable[$cpu]=${BASH_REMATCH[2]};;
          "Voltage_Offset_Plane_4_Enabled") voltage_offset_plane_4_enable[$cpu]=${BASH_REMATCH[2]};;
          "Voltage_Offset_Plane_5_Enabled") voltage_offset_plane_5_enable[$cpu]=${BASH_REMATCH[2]};;
          "Voltage_Offset_Plane_0") voltage_offset_plane_0[$cpu]=${BASH_REMATCH[2]};;
          "Voltage_Offset_Plane_1") voltage_offset_plane_1[$cpu]=${BASH_REMATCH[2]};;
          "Voltage_Offset_Plane_2") voltage_offset_plane_2[$cpu]=${BASH_REMATCH[2]};;
          "Voltage_Offset_Plane_3") voltage_offset_plane_3[$cpu]=${BASH_REMATCH[2]};;
          "Voltage_Offset_Plane_4") voltage_offset_plane_4[$cpu]=${BASH_REMATCH[2]};;
          "Voltage_Offset_Plane_5") voltage_offset_plane_5[$cpu]=${BASH_REMATCH[2]};;
          # Target Temperature
          "Target_Temperature_Enabled") target_temperature_enable[$cpu]=${BASH_REMATCH[2]};;
          "Target_Temperature") target_temperature[$cpu]=${BASH_REMATCH[2]};;
          "Target_Temperature_Battery_Enabled") target_temperature_battery_enable[$cpu]=${BASH_REMATCH[2]};;
          "Target_Temperature_Battery") target_temperature_battery[$cpu]=${BASH_REMATCH[2]};;
          # MSR_THERM2_CTL
          "TM_Select_Enabled") tm_select_enable[$cpu]=${BASH_REMATCH[2]};;
          "TM_Select") tm_select[$cpu]=${BASH_REMATCH[2]};;
          # IA32_MISC_ENABLE
          "TM2_Enable_Enabled") tm2_enable_enable[$cpu]=${BASH_REMATCH[2]};;
          "TM2_Enable") tm2_enable[$cpu]=${BASH_REMATCH[2]};;
          "EIST_Enable_Enabled") eist_enable_enable[$cpu]=${BASH_REMATCH[2]};;
          "EIST_Enable") eist_enable[$cpu]=${BASH_REMATCH[2]};;
          "EIST_Lock_Enabled") eist_lock_enable[$cpu]=${BASH_REMATCH[2]};;
          "EIST_Lock") eist_lock[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Disable_Enabled") tbt_disable_enable[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Disable") tbt_disable[$cpu]=${BASH_REMATCH[2]};;
          "Fast_Strings_Enable_Enabled") fs_enable_enable[$cpu]=${BASH_REMATCH[2]};;
          "Fast_Strings_Enable") fs_enable[$cpu]=${BASH_REMATCH[2]};;
          "Hardware_Prefetcher_Disable_Enabled") hwp_disable_enable[$cpu]=${BASH_REMATCH[2]};;
          "Hardware_Prefetcher_Disable") hwp_disable[$cpu]=${BASH_REMATCH[2]};;
          "FERR_Multiplexing_Enable_Enabled") ferr_enable_enable[$cpu]=${BASH_REMATCH[2]};;
          "FERR_Multiplexing_Enable") ferr_enable[$cpu]=${BASH_REMATCH[2]};;
          "ENABLE_MONITOR_FSM_Enabled") fsm_enable_enable[$cpu]=${BASH_REMATCH[2]};;
          "ENABLE_MONITOR_FSM") fsm_enable[$cpu]=${BASH_REMATCH[2]};;
          "Adjacent_Cache_Line_Prefetch_Disable_Enabled") adj_disable_enable[$cpu]=${BASH_REMATCH[2]};;
          "Adjacent_Cache_Line_Prefetch_Disable") adj_disable[$cpu]=${BASH_REMATCH[2]};;
          "Limit_CPUID_Maxval_Enabled") cpuid_maxval_enable[$cpu]=${BASH_REMATCH[2]};;
          "Limit_CPUID_Maxval") cpuid_maxval[$cpu]=${BASH_REMATCH[2]};;
          "xTPR_Message_Disable_Enabled") xtpr_disable_enable[$cpu]=${BASH_REMATCH[2]};;
          "xTPR_Message_Disable") xtpr_disable[$cpu]=${BASH_REMATCH[2]};;
          "XD_Bit_Disable_Enabled") xd_disable_enable[$cpu]=${BASH_REMATCH[2]};;
          "XD_Bit_Disable") xd_disable[$cpu]=${BASH_REMATCH[2]};;
          "DCU_Prefetcher_Disable_Enabled") dcu_disable_enable[$cpu]=${BASH_REMATCH[2]};;
          "DCU_Prefetcher_Disable") dcu_disable[$cpu]=${BASH_REMATCH[2]};;
          "IP_Prefetcher_Disable_Enabled") ipp_disable_enable[$cpu]=${BASH_REMATCH[2]};;
          "IP_Prefetcher_Disable") ipp_disbale[$cpu]=${BASH_REMATCH[2]};;
          # MSR_TURBO_ACTIVATION_RATIO
          "TBT_Activation_Ratio_Enabled") tbt_activation_ratio_enable[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Activation_Ratio") tbt_activation_ratio[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Activation_Ratio_Lock_Enabled") tbt_activation_ratio_lock_enable[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Activation_Ratio_Lock") tbt_activation_ratio_lock[$cpu]=${BASH_REMATCH[2]};;
          # MSR_TURBO_RATIO_LIMIT
          "TBT_Ratio_Limit_Enabled") tbt_ratio_limit_enable[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_1C") tbt_ratio_limit_1c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_2C") tbt_ratio_limit_2c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_3C") tbt_ratio_limit_3c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_4C") tbt_ratio_limit_4c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_5C") tbt_ratio_limit_5c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_6C") tbt_ratio_limit_6c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_7C") tbt_ratio_limit_7c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_8C") tbt_ratio_limit_8c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_9C") tbt_ratio_limit_9c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_10C") tbt_ratio_limit_10c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_11C") tbt_ratio_limit_11c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_12C") tbt_ratio_limit_12c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_13C") tbt_ratio_limit_13c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_14C") tbt_ratio_limit_14c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_15C") tbt_ratio_limit_15c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_16C") tbt_ratio_limit_16c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_17C") tbt_ratio_limit_17c[$cpu]=${BASH_REMATCH[2]};;
          "TBT_Ratio_Limit_18C") tbt_ratio_limit_18c[$cpu]=${BASH_REMATCH[2]};;
          # HTT
          "HTT_Disable_Enabled") htt_disable_enable[$cpu]=${BASH_REMATCH[2]};;
          "HTT_Disable") htt_disable[$cpu]=${BASH_REMATCH[2]};;
          *) >&2 echo "Warning: Ignoring unknown variable Processor${cpu}::${BASH_REMATCH[1]}";;
          esac
          ;;
        "CpuFreqUtils")
          case "${BASH_REMATCH[1]}" in
          "Enabled") fs_enabled_g[$cpu]=${BASH_REMATCH[2]};;
          "Battery_Enabled") fs_batt_g[$cpu]=${BASH_REMATCH[2]};;
          "PerCpu_Enabled") fs_per_thread[$cpu]=${BASH_REMATCH[2]};;
          *) >&2 echo "Warning: Ignoring unknown variable CpuFreqUtils${cpu}::${BASH_REMATCH[1]}";;
          esac
          ;;
        "CpuFreqUtils_Thread")
          case "${BASH_REMATCH[1]}" in
          "Enabled") fs_enabled[$logical]=${BASH_REMATCH[2]};;
          "Battery_Enabled") fs_batt[$logical]=${BASH_REMATCH[2]};;
          "Governor_AC") fs_governor_ac[$logical]=${BASH_REMATCH[2]};;
          "Min_Frequency_AC") fs_min_freq_ac[$logical]=${BASH_REMATCH[2]};;
          "Max_Frequency_AC") fs_max_freq_ac[$logical]=${BASH_REMATCH[2]};;
          "Userspace_Frequency_AC") fs_freq_ac[$logical]=${BASH_REMATCH[2]};;
          "Governor_DC") fs_governor_dc[$logical]=${BASH_REMATCH[2]};;
          "Min_Frequency_DC") fs_min_freq_dc[$logical]=${BASH_REMATCH[2]};;
          "Max_Frequency_DC") fs_max_freq_dc[$logical]=${BASH_REMATCH[2]};;
          "Userspace_Frequency_DC") fs_freq_dc[$logical]=${BASH_REMATCH[2]};;
          *) >&2 echo "Warning: Ignoring unknown variable CpuFreqUtils_Thread${logical}::${BASH_REMATCH[1]}";;
          esac
          ;;
        *) >&2 echo "Warning: Ignoring unknown section '$current_section'";;
        esac
      }
    }
  done < $1
  #
  [ $isVerbose -gt 1 ] && {
    echo "Read \"Common\" section from the configuration file:"
    echo "  apply_on_boot_and_resume = ${apply_on_boot_and_resume}"
    echo "  apply_on_acpi_power_event = ${apply_on_acpi_power_event}"
    echo "  gui_save_on_exit = ${gui_save}"
    echo "  smp_disable_enable = $smp_disable_enable"
    echo "  smp_disable = $smp_disable"
    echo "  per_cpu_htt_enable = $per_cpu_htt_enable"
    for i in ${!allow_overvolting[@]}; do
      echo "Read \"Processor$i\" section from the configuration file:"
      echo "  allow_overvolting = ${allow_overvolting[$i]}"
      echo "  voltage_offset_plane_0_enable = ${voltage_offset_plane_0_enable[$i]}"
      echo "  voltage_offset_plane_1_enable = ${voltage_offset_plane_1_enable[$i]}"
      echo "  voltage_offset_plane_2_enable = ${voltage_offset_plane_2_enable[$i]}"
      echo "  voltage_offset_plane_3_enable = ${voltage_offset_plane_3_enable[$i]}"
      echo "  voltage_offset_plane_4_enable = ${voltage_offset_plane_4_enable[$i]}"
      echo "  voltage_offset_plane_5_enable = ${voltage_offset_plane_5_enable[$i]}"
      echo "  voltage_offset_plane_0 = ${voltage_offset_plane_0[$i]}"
      echo "  voltage_offset_plane_1 = ${voltage_offset_plane_1[$i]}"
      echo "  voltage_offset_plane_2 = ${voltage_offset_plane_2[$i]}"
      echo "  voltage_offset_plane_3 = ${voltage_offset_plane_3[$i]}"
      echo "  voltage_offset_plane_4 = ${voltage_offset_plane_4[$i]}"
      echo "  voltage_offset_plane_5 = ${voltage_offset_plane_5[$i]}"
      echo "  target_temperature_enable = ${target_temperature_enable[$i]}"
      echo "  target_temperature = ${target_temperature[$i]}"
      echo "  target_temperature_battery_enable = ${target_temperature_battery_enable[$i]}"
      echo "  target_temperature_battery = ${target_temperature_battery[$i]}"
      echo "  tm_select_enable = ${tm_select_enable[$i]}"
      echo "  tm_select = ${tm_select[$i]}"
      echo "  tm2_enable_enable = ${tm2_enable_enable[$i]}"
      echo "  tm2_enable = ${tm2_enable[$i]}"
      echo "  eist_enable_enable = ${eist_enable_enable[$i]}"
      echo "  eist_enable = ${eist_enable[$i]}"
      echo "  eist_lock_enable = ${eist_lock_enable[$i]}"
      echo "  eist_lock = ${eist_lock[$i]}"
      echo "  tbt_disable_enable = ${tbt_disable_enable[$i]}"
      echo "  tbt_disable = ${tbt_disable[$i]}"
      echo "  fs_enable_enable = ${fs_enable_enable[$i]}"
      echo "  fs_enable = ${fs_enable[$i]}"
      echo "  hwp_disable_enable = ${hwp_disable_enable[$i]}"
      echo "  hwp_disable = ${hwp_disable[$i]}"
      echo "  ferr_enable_enable = ${ferr_enable_enable[$i]}"
      echo "  ferr_enable = ${ferr_enable[$i]}"
      echo "  fsm_enable_enable = ${fsm_enable_enable[$i]}"
      echo "  fsm_enable = ${fsm_enable[$i]}"
      echo "  adj_disable_enable = ${adj_disable_enable[$i]}"
      echo "  adj_disable = ${adj_disable[$i]}"
      echo "  cpuid_maxval_enable = ${cpuid_maxval_enable[$i]}"
      echo "  cpuid_maxval = ${cpuid_maxval[$i]}"
      echo "  xtpr_disable_enable = ${xtpr_disable_enable[$i]}"
      echo "  xtpr_disable = ${xtpr_disable[$i]}"
      echo "  xd_disable_enable = ${xd_disable_enable[$i]}"
      echo "  xd_disable = ${xd_disable[$i]}"
      echo "  dcu_disable_enable = ${dcu_disable_enable[$i]}"
      echo "  dcu_disable = ${dcu_disable[$i]}"
      echo "  ipp_disable_enable = ${ipp_disable_enable[$i]}"
      echo "  ipp_disbale = ${ipp_disbale[$i]}"
      echo "  tbt_activation_ratio_enable = ${tbt_activation_ratio_enable[$i]}"
      echo "  tbt_activation_ratio = ${tbt_activation_ratio[$i]}"
      echo "  tbt_activation_ratio_lock_enable = ${tbt_activation_ratio_lock_enable[$i]}"
      echo "  tbt_activation_ratio_lock = ${tbt_activation_ratio_lock[$i]}"
      echo "  tbt_ratio_limit_enable = ${tbt_ratio_limit_enable[$i]}"
      echo "  tbt_ratio_limit_1c = ${tbt_ratio_limit_1c[$i]}"
      echo "  tbt_ratio_limit_2c = ${tbt_ratio_limit_2c[$i]}"
      echo "  tbt_ratio_limit_3c = ${tbt_ratio_limit_3c[$i]}"
      echo "  tbt_ratio_limit_4c = ${tbt_ratio_limit_4c[$i]}"
      echo "  tbt_ratio_limit_5c = ${tbt_ratio_limit_5c[$i]}"
      echo "  tbt_ratio_limit_6c = ${tbt_ratio_limit_6c[$i]}"
      echo "  tbt_ratio_limit_7c = ${tbt_ratio_limit_7c[$i]}"
      echo "  tbt_ratio_limit_8c = ${tbt_ratio_limit_8c[$i]}"
      echo "  tbt_ratio_limit_9c = ${tbt_ratio_limit_9c[$i]}"
      echo "  tbt_ratio_limit_10c = ${tbt_ratio_limit_10c[$i]}"
      echo "  tbt_ratio_limit_11c = ${tbt_ratio_limit_11c[$i]}"
      echo "  tbt_ratio_limit_12c = ${tbt_ratio_limit_12c[$i]}"
      echo "  tbt_ratio_limit_13c = ${tbt_ratio_limit_13c[$i]}"
      echo "  tbt_ratio_limit_14c = ${tbt_ratio_limit_14c[$i]}"
      echo "  tbt_ratio_limit_15c = ${tbt_ratio_limit_15c[$i]}"
      echo "  tbt_ratio_limit_16c = ${tbt_ratio_limit_16c[$i]}"
      echo "  tbt_ratio_limit_17c = ${tbt_ratio_limit_17c[$i]}"
      echo "  tbt_ratio_limit_18c = ${tbt_ratio_limit_18c[$i]}"
      echo "  htt_disable_enable = ${htt_disable_enable[$i]}"
      echo "  htt_disable = ${htt_disable[$i]}"
      echo "Read \"CpuFreqUtils$i\" section from the configuration file:"
      echo "  fs_enabled_g = ${fs_enabled_g[$i]}"
      echo "  fs_batt_g = ${fs_batt_g[$i]}"
      echo "  fs_per_thread = ${fs_per_thread[$i]}"
    done
    for p in ${!cpuid_logical_cpus[@]}; do
      for l in ${cpuid_logical_cpus[$p]}; do
        echo "Read \"CpuFreqUtils_Thread$l\" section from the configuration file:"
        echo "  fs_enabled = ${fs_enabled[$l]}"
        echo "  fs_governor_ac = ${fs_governor_ac[$l]}"
        echo "  fs_min_freq_ac = ${fs_min_freq_ac[$l]}"
        echo "  fs_max_freq_ac = ${fs_max_freq_ac[$l]}"
        echo "  fs_freq_ac = ${fs_freq_ac[$l]}"
        echo "  fs_batt = ${fs_batt[$l]}"
        echo "  fs_governor_dc = ${fs_governor_dc[$l]}"
        echo "  fs_min_freq_dc = ${fs_min_freq_dc[$l]}"
        echo "  fs_max_freq_dc = ${fs_max_freq_dc[$l]}"
        echo "  fs_freq_dc = ${fs_freq_dc[$l]}"
      done
    done
  }
  return 0
}

# Validates boolean values
# $1 == value under test
# $2 == configuration-file section name
# $3 == configuration-file item name
# returns 0 on success
ValidateBooleanValue() {
  [[ $1 =~ ^(true|false)$ ]] || {
    >&2 echo -e "The value of item '$2::$3' must be 'true' or 'false'.\n"
    return 1
  }
  return 0
}

# Validates integer values
# $1 == value under test
# $2 == configuration-file section name
# $3 == configuration-file item name
# returns 0 on success
ValidateIntegerValue() {
  [[ "$1" =~ ^[-]?[[:digit:]]+$ ]] || {
    >&2 echo -e "The value of item '$2::$3' must contain an integer value.\n"
    return 1
  }
  return 0
}

# Validate the configuration for a single cpu
# $1 == ordinal nr of the cpu under test
# returns 0 on success
ValidateCpuConfiguration() {
  local ttarget
  local tmax
  local l
  #
  # Common
  #
  ValidateBooleanValue "$apply_on_boot_and_resume" Common Apply_On_Boot_And_Resume || return 1
  ValidateBooleanValue "$apply_on_acpi_power_event" Common Apply_On_Acpi_Power_Event || return 1
  ValidateBooleanValue "${smp_disable_enable}" Common SMT_Disable_Enabled || return 1
  ValidateBooleanValue "${smp_disable}" Common SMT_Disable || return 1
  ValidateBooleanValue "${per_cpu_htt_enable}" Common CPU_HTT_Enable || return 1
  #
  # FIVR
  #
  ValidateBooleanValue "${allow_overvolting[$1]}" Processor$1 Allow_Overvolting || return 1
  ValidateBooleanValue "${voltage_offset_plane_0_enable[$1]}" Processor$1 Voltage_Offset_Plane_0_Enable || return 1
  ValidateBooleanValue "${voltage_offset_plane_1_enable[$1]}" Processor$1 Voltage_Offset_Plane_1_Enable || return 1
  ValidateBooleanValue "${voltage_offset_plane_2_enable[$1]}" Processor$1 Voltage_Offset_Plane_2_Enable || return 1
  ValidateBooleanValue "${voltage_offset_plane_3_enable[$1]}" Processor$1 Voltage_Offset_Plane_3_Enable || return 1
  ValidateBooleanValue "${voltage_offset_plane_4_enable[$1]}" Processor$1 Voltage_Offset_Plane_4_Enable || return 1
  ValidateBooleanValue "${voltage_offset_plane_5_enable[$1]}" Processor$1 Voltage_Offset_Plane_5_Enable || return 1
  #
  # only test if voltage offsets are witin range if they are enabled
  [ "x${allow_overvolting[$1]}" != "xtrue" ] && {
    [ "x${voltage_offset_plane_0_enable[$1]}" == "xtrue" ] && {
      IsPositiveVoltageOffset "${voltage_offset_plane_0[$1]}" "CPU #$1 FIVR plane #0" && return 1
    }
    [ "x${voltage_offset_plane_1_enable[$1]}" == "xtrue" ] && {
      IsPositiveVoltageOffset "${voltage_offset_plane_1[$1]}" "CPU #$1 FIVR plane #1" && return 1
    }
    [ "x${voltage_offset_plane_2_enable[$1]}" == "xtrue" ] && {
      IsPositiveVoltageOffset "${voltage_offset_plane_2[$1]}" "CPU #$1 FIVR plane #2" && return 1
    }
    [ "x${voltage_offset_plane_3_enable[$1]}" == "xtrue" ] && {
      IsPositiveVoltageOffset "${voltage_offset_plane_3[$1]}" "CPU #$1 FIVR plane #3" && return 1
    }
    [ "x${voltage_offset_plane_4_enable[$1]}" == "xtrue" ] && {
      IsPositiveVoltageOffset "${voltage_offset_plane_4[$1]}" "CPU #$1 FIVR plane #4" && return 1
    }
    [ "x${voltage_offset_plane_5_enable[$1]}" == "xtrue" ] && {
      IsPositiveVoltageOffset "${voltage_offset_plane_5[$1]}" "CPU #$1 FIVR plane #5" && return 1
    }
  } || {
    [ "x${voltage_offset_plane_0_enable[$1]}" == "xtrue" ] && {
      IsValidVoltageOffset "${voltage_offset_plane_0[$1]}" "CPU #$1 FIVR plane #0" || return 1
    }
    [ "x${voltage_offset_plane_1_enable[$1]}" == "xtrue" ] && {
      IsValidVoltageOffset "${voltage_offset_plane_1[$1]}" "CPU #$1 FIVR plane #1" || return 1
    }
    [ "x${voltage_offset_plane_2_enable[$1]}" == "xtrue" ] && {
      IsValidVoltageOffset "${voltage_offset_plane_2[$1]}" "CPU #$1 FIVR plane #2" || return 1
    }
    [ "x${voltage_offset_plane_3_enable[$1]}" == "xtrue" ] && {
      IsValidVoltageOffset "${voltage_offset_plane_3[$1]}" "CPU #$1 FIVR plane #3" || return 1
    }
    [ "x${voltage_offset_plane_4_enable[$1]}" == "xtrue" ] && {
      IsValidVoltageOffset "${voltage_offset_plane_4[$1]}" "CPU #$1 FIVR plane #4" || return 1
    }
    [ "x${voltage_offset_plane_5_enable[$1]}" == "xtrue" ] && {
      IsValidVoltageOffset "${voltage_offset_plane_5[$1]}" "CPU #$1 FIVR plane #5" || return 1
    }
  }
  #
  # Target Temperature
  #
  ValidateBooleanValue "${target_temperature_enable[$1]}" Processor$1 Target_Temperature_Enable || return 1
  ValidateBooleanValue "${target_temperature_battery_enable[$1]}" Processor$1 Target_Temperature_Battery_Enable || return 1
  #
  # Test target temperature (if enabled)
  # (assume 'Unknown Fam6' is newer than the latest known 'big core' cpu)
  local logical
  GetLogicalCpu $1 0 logical
  [ "${target_temperature_enable[$1]}" = "true" ] && {
    case "${synth_microarchitecture[$1]}" in
    "Ivybridge" | "Haswell" | "Broadwell" | "Skylake" | "Kabylake" | "Cannonlake" | \
    "Icelake" | "Tigerlake" | "Cometlake" | "Unknown Fam6")
      ttarget=`rdmsr -p $logical -u -f23:16 0x1a2`
      [ $? -ne 0 ] && return 1
      [ $ttarget -eq 0 ] && ttarget=100
      tmax=$((ttarget - 15))
      ;;
    "Silvermont" | "Airmont" | "Goldmont" | "Tremont" | "Knights Landing" | "Knights Mill")
      ttarget=`rdmsr -p $logical -u -f23:16 0x1a2`
      [ $? -ne 0 ] && return 1
      [ $ttarget -eq 0 ] && ttarget=100
      tmax=$((ttarget - 63))
      ;;
    *)
      >&2 echo -e "CPU #$1 does not support modifying its target temperature. (${synth_microarchitecture[$1]})\n"
      return 1
      ;;
    esac
    target_temperature[$1]=`printf "%.0f" ${target_temperature[$1]}`
    [ `bc -l <<<"rv=0; if (${target_temperature[$1]} < $tmax) rv=1; if (${target_temperature[$1]} > $ttarget) rv=1; print rv;"` -eq 1 ] && {
      >&2 echo -e "CPU #$1 Target Temperature out of range (${target_temperature[$1]}), it must be in-between $tmax and $ttarget °C.\n"
      return 1
    }
    [ "${target_temperature_battery_enable[$1]}" = "true" ] && {
      target_temperature_battery[$1]=`printf "%.0f" ${target_temperature_battery[$1]}`
      [ `bc -l <<<"rv=0; if (${target_temperature_battery[$1]} < $tmax) rv=1; if (${target_temperature_battery[$1]} > $ttarget) rv=1; print rv;"` -eq 1 ] && {
        >&2 echo -e "CPU #$1 Target Temperature (Battery) out of range (${target_temperature_battery[$1]}), it must be in-between $tmax and $ttarget °C.\n"
        return 1
      }
    }
  }
  #
  # MSR_THERM2_CTL
  ValidateBooleanValue "${tm_select_enable[$1]}" Processor$1 TM_Select_Enable || return 1
  ValidateBooleanValue "${tm_select[$1]}" Processor$1 TM_Select || return 1
  #
  # IA32_MISC_ENABLE
  ValidateBooleanValue "${tm2_enable_enable[$1]}" Processor$1 TM2_Enable_Enable || return 1
  ValidateBooleanValue "${tm2_enable[$1]}" Processor$1 TM2_Enable || return 1
  ValidateBooleanValue "${eist_enable_enable[$1]}" Processor$1 EIST_Enable_Enable || return 1
  ValidateBooleanValue "${eist_enable[$1]}" Processor$1 EIST_Enable || return 1
  ValidateBooleanValue "${eist_lock_enable[$1]}" Processor$1 EIST_Lock_Enable || return 1
  ValidateBooleanValue "${eist_lock[$1]}" Processor$1 EIST_Lock || return 1
  ValidateBooleanValue "${tbt_disable_enable[$1]}" Processor$1 TBT_Disable_Enable || return 1
  ValidateBooleanValue "${tbt_disable[$1]}" Processor$1 TBT_Disable || return 1
  ValidateBooleanValue "${fs_enable_enable[$1]}" Processor$1 Fast_Strings_Enable_Enabled || return 1
  ValidateBooleanValue "${fs_enable[$1]}" Processor$1 Fast_Strings_Enable || return 1
  ValidateBooleanValue "${hwp_disable_enable[$1]}" Processor$1 Hardware_Prefetcher_Disable_Enabled || return 1
  ValidateBooleanValue "${hwp_disable[$1]}" Processor$1 Hardware_Prefetcher_Disable || return 1
  ValidateBooleanValue "${ferr_enable_enable[$1]}" Processor$1 FERR_Multiplexing_Enable_Enabled || return 1
  ValidateBooleanValue "${ferr_enable[$1]}" Processor$1 FERR_Multiplexing_Enable || return 1
  ValidateBooleanValue "${fsm_enable_enable[$1]}" Processor$1 ENABLE_MONITOR_FSM_Enabled || return 1
  ValidateBooleanValue "${fsm_enable[$1]}" Processor$1 ENABLE_MONITOR_FSM || return 1
  ValidateBooleanValue "${adj_disable_enable[$1]}" Processor$1 Adjacent_Cache_Line_Prefetch_Disable_Enabled || return 1
  ValidateBooleanValue "${adj_disable[$1]}" Processor$1 Adjacent_Cache_Line_Prefetch_Disable || return 1
  ValidateBooleanValue "${cpuid_maxval_enable[$1]}" Processor$1 Limit_CPUID_Maxval_Enabled || return 1
  ValidateBooleanValue "${cpuid_maxval[$1]}" Processor$1 Limit_CPUID_Maxval || return 1
  ValidateBooleanValue "${xtpr_disable_enable[$1]}" Processor$1 xTPR_Message_Disable_Enabled || return 1
  ValidateBooleanValue "${xtpr_disable[$1]}" Processor$1 xTPR_Message_Disable || return 1
  ValidateBooleanValue "${xd_disable_enable[$1]}" Processor$1 XD_Bit_Disable_Enabled || return 1
  ValidateBooleanValue "${xd_disable[$1]}" Processor$1 XD_Bit_Disable || return 1
  ValidateBooleanValue "${dcu_disable_enable[$1]}" Processor$1 DCU_Prefetcher_Disable_Enabled || return 1
  ValidateBooleanValue "${dcu_disable[$1]}" Processor$1 DCU_Prefetcher_Disable || return 1
  ValidateBooleanValue "${ipp_disable_enable[$1]}" Processor$1 IP_Prefetcher_Disable_Enabled || return 1
  ValidateBooleanValue "${ipp_disbale[$1]}" Processor$1 IP_Prefetcher_Disable || return 1
  #
  # MSR_TURBO_ACTIVATION_RATIO
  ValidateBooleanValue "${tbt_activation_ratio_enable[$1]}" Processor$1 TBT_Activation_Ratio_Enable || return 1
  [ "${tbt_activation_ratio_enable[$1]}" = "true" ] && {
    ValidateIntegerValue "${tbt_activation_ratio[$1]}" Processor$1 TBT_Activation_Ratio || return 1
  }
  ValidateBooleanValue "${tbt_activation_ratio_lock_enable[$1]}" Processor$1 TBT_Activation_Ratio_Lock_Enable || return 1
  ValidateBooleanValue "${tbt_activation_ratio_lock[$1]}" Processor$1 TBT_Activation_Ratio_Lock || return 1
  #
  # MSR_TURBO_RATIO_LIMIT
  ValidateBooleanValue "${tbt_ratio_limit_enable[$1]}" Processor$1 TBT_Ratio_Limit_Enable || return 1
  [ "${tbt_ratio_limit_enable[$1]}" = "true" ] && {
    ValidateIntegerValue "${tbt_ratio_limit_1c[$1]}" Processor$1 TBT_Ratio_Limit_1C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_2c[$1]}" Processor$1 TBT_Ratio_Limit_2C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_3c[$1]}" Processor$1 TBT_Ratio_Limit_3C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_4c[$1]}" Processor$1 TBT_Ratio_Limit_4C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_5c[$1]}" Processor$1 TBT_Ratio_Limit_5C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_6c[$1]}" Processor$1 TBT_Ratio_Limit_6C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_7c[$1]}" Processor$1 TBT_Ratio_Limit_7C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_8c[$1]}" Processor$1 TBT_Ratio_Limit_8C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_9c[$1]}" Processor$1 TBT_Ratio_Limit_9C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_10c[$1]}" Processor$1 TBT_Ratio_Limit_10C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_11c[$1]}" Processor$1 TBT_Ratio_Limit_11C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_12c[$1]}" Processor$1 TBT_Ratio_Limit_12C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_13c[$1]}" Processor$1 TBT_Ratio_Limit_13C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_14c[$1]}" Processor$1 TBT_Ratio_Limit_14C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_15c[$1]}" Processor$1 TBT_Ratio_Limit_15C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_16c[$1]}" Processor$1 TBT_Ratio_Limit_16C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_17c[$1]}" Processor$1 TBT_Ratio_Limit_17C || return 1
    ValidateIntegerValue "${tbt_ratio_limit_18c[$1]}" Processor$1 TBT_Ratio_Limit_18C || return 1
  }
  #
  # SMT/HTT
  ValidateBooleanValue "${htt_disable_enable[$1]}" Processor$1 HTT_Disable_Enabled || return 1
  ValidateBooleanValue "${htt_disable[$1]}" Processor$1 HTT_Disable || return 1
  #
  # Frequency Scaling
  ValidateBooleanValue "${fs_enabled_g[$1]}" CpuFreqUtils$1 Enabled || return 1
  [ "${fs_enabled_g[$1]}" = "true" ] && {
    ValidateBooleanValue "${fs_batt_g[$1]}" CpuFreqUtils$1 Battery_Enabled || return 1
    ValidateBooleanValue "${fs_per_thread[$1]}" CpuFreqUtils$1 PerCpu_Enabled || return 1
    for l in ${cpuid_logical_cpus[$1]}; do
      ValidateBooleanValue "${fs_enabled[$l]}" CpuFreqUtils_Thread$l Enabled || return 1
      [ "${fs_enabled[$l]}" = "true" ] && {
        ValidateIntegerValue "${fs_min_freq_ac[$l]}" CpuFreqUtils_Thread$l Min_Frequency_AC || return 1
        ValidateIntegerValue "${fs_max_freq_ac[$l]}" CpuFreqUtils_Thread$l Max_Frequency_AC || return 1
        ValidateIntegerValue "${fs_freq_ac[$l]}" CpuFreqUtils_Thread$l Userspace_Frequency_AC || return 1
        ValidateBooleanValue "${fs_batt[$l]}" CpuFreqUtils_Thread$l Battery_Enabled || return 1
        [ "${fs_batt[$l]}" = "true" ] && {
          ValidateIntegerValue "${fs_min_freq_dc[$l]}" CpuFreqUtils_Thread$l Min_Frequency_DC || return 1
          ValidateIntegerValue "${fs_max_freq_dc[$l]}" CpuFreqUtils_Thread$l Max_Frequency_DC || return 1
          ValidateIntegerValue "${fs_freq_dc[$l]}" CpuFreqUtils_Thread$l Userspace_Frequency_DC || return 11
        }
      }
    done
  }
  return 0
}

# Validate commandline arguments
# returns 0 on success
ValidateCmdlineConfiguration() {
  # fixup 'numargs' if --dry-run is used.
  [ $isDryRun -ne 0 ] && numargs=$((numargs - 1))
  #
  # fixup 'numargs' if --verbose is used.
  [ $isVerbose -ne 0 ] && numargs=$((numargs - isVerbose))
  #
  [[ $numargs -ne 1 && $doFreqScaleAll -ne 0 ]] && {
    >&2 echo -e "The --fscale-all option can only be combined with --verbose and --dry-run.\n"
    return 1
  }
  #
  # fixup 'numargs' if --march is used.
  [ ! -z "$force_march" ] && numargs=$((numargs - 2))
  #
  # Fixup 'numargs' if --force is used with --boot
  [ $doBootOrResume -ne 0 ] && numargs=$((numargs - isForce))
  #
  # Apply --force to the cpu (specified by --processor)
  [ $isForce -ne 0 ] && allow_overvolting[$isCpu]="true"
  #
  # --boot, --resume, --acpi and --read want to be the only task
  [ $numargs -ne 1 ] && {
    [ $doBootOrResume -ne 0 ] && {
      >&2 echo -e "The --boot and --resume options can only be combined with --dry-run and/or --verbose.\n"
      return 1
    }
    [ $doPower -ne 0 ] && {
      >&2 echo -e "The --acpi option can only be combined with --dry-run and/or --verbose.\n"
      return 1
    }
    [ $doRead -ne 0 ] && {
      # --read can be combined with --processor
      [ $doReadSingleCpu -ne 1 ] && {
        >&2 echo -e "The --read option can only be combined with --processor.\n"
        return 1
      }
      [ $numargs -ne 3 ] && {
        >&2 echo -e "The --read option can only be combined with --processor.\n"
        return 1
      }
    }
  }
  #
  # verify that the cpu is available
  [ $isCpu -gt $num_physical_cpus ] && {
    [ $isDryRun -ne 0 ] && {
      >&2 echo "Warning: The selected processor (CPU #$isCpu) does not exist!"
    } || {
      >&2 echo -e "The selected processor (CPU #$isCpu) does not exist!\n"
      return 1
    }
  }
  #
  # enable/disable SMT are mutually exclusive
  [[ $doEnableSmt -ne 0 && $doDisableSmt -ne 0 ]] && {
    >&2 echo -e "The --smt option may only be specified once.\n"
    return 1
  }
  #
  # enable/disable HTT are mutually exclusive
  [[ $doEnableSmtCpu -ne 0 && $doDisableSmtCpu -ne 0 ]] && {
    >&2 echo -e "The --htt option may only be specified once.\n"
    return 1
  }
  #
  # Make sure we have cpuid_ vars during a dry-run using an invalid cpu number
  [[ $isDryRun -ne 0 && $haveProcessor -ne 0 ]] && {
    [ -z ${cpuid_vendor_id[$isCpu]} ] && {
      cpuid_vendor_id[$isCpu]=0
      cpuid_family[$isCpu]=0
      cpuid_model[$isCpu]=0
      cpuid_model_name[$isCpu]=0
      cpuid_stepping[$isCpu]=0
      cpuid_num_cores[$isCpu]=0
      cpuid_siblings[$isCpu]=0
      synth_microarchitecture[$isCpu]=0
      cpuid_logical_cpus[$isCpu]=0
    }
  }
  return 0
}

