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
# Apply the Frequency Scaling settings from the commandline or configuration file.
# Returns 0 on success.
FrequencyScalingApply() {
  local p
  local l
  local cpu
  #
  # Loop over all physical processors
  for p in ${!cpuid_logical_cpus[@]}; do
    #
    # Frequency Scaling enabled for this processor?
    [ "${fs_enabled_g[$p]}" = "true" ] && {
      [ $isVerbose -gt 0 ] && echo "Adjusting Frequency Scaling settings for processor #$p"
      #
      # Yes, Use seperate settings for each thread?
      [ "${fs_per_thread[$p]}" = "true" ] && {
        #
        # Yes, program each logical cpu with its own settings.
        # First get the AC/DC status.
        /usr/bin/on_ac_power
        [ $? -eq 1 ] && {
          #
          # On DC power, program DC settings if enabled, AC settings otherwise.
          # Loop over the logical cpus of this processor...
          for l in ${cpuid_logical_cpus[$p]}; do
            [ "${fs_enabled[$l]}" = "true" ] && {
              #
              # DC mode enabled?
              [ "${fs_batt[$l]}" = "true" ] && {
                #
                # Yes, program DC settings.
                [ "${fs_governor_dc[$l]}" = "userspace" ] && {
                  [ $isVerbose -gt 0 ] && {
                    echo "  - Setting governor for logical cpu$l to 'userspace' @ ${fs_freq_dc[$l]}MHz"
                  }
                  [ $isDryRun -ne 0 ] && {
                    >&2 echo "cpufreq-set --cpu $l --governor ${fs_governor_dc[$l]}"
                    >&2 echo "cpufreq-set --cpu $l --freq ${fs_freq_dc[$l]}MHz"
                  } || {
                    cpufreq-set --cpu $l --governor ${fs_governor_dc[$l]} || return 1
                    cpufreq-set --cpu $l --freq ${fs_freq_dc[$l]}MHz || return 1
                  }
                } || {
                  [ $isVerbose -gt 0 ] && {
                    echo "  - Setting governor for logical cpu$l to '${fs_governor_dc[$l]}' @ ${fs_min_freq_dc[$l]}MHz ... ${fs_max_freq_dc[$l]}MHz"
                  }
                  [ $isDryRun -ne 0 ] && {
                    >&2 echo "cpufreq-set --cpu $l --governor ${fs_governor_dc[$l]} --min ${fs_min_freq_dc[$l]}MHz --max ${fs_max_freq_dc[$l]}MHz"
                  } || {
                    cpufreq-set --cpu $l --governor ${fs_governor_dc[$l]} --min ${fs_min_freq_dc[$l]}MHz --max ${fs_max_freq_dc[$l]}MHz || return 1
                  }
                }
              } || {
                #
                # No, program AC settings.
                [ "${fs_governor_ac[$l]}" = "userspace" ] && {
                  [ $isVerbose -gt 0 ] && {
                    echo "  - Setting governor for logical cpu$l to 'userspace' @ ${fs_freq_ac[$l]}MHz"
                  }
                  [ $isDryRun -ne 0 ] && {
                    >&2 echo "cpufreq-set --cpu $l --governor ${fs_governor_ac[$l]}"
                    >&2 echo "cpufreq-set --cpu $l --freq ${fs_freq_ac[$l]}MHz"
                  } || {
                    cpufreq-set --cpu $l --governor ${fs_governor_ac[$l]} || return 1
                    cpufreq-set --cpu $l --freq ${fs_freq_ac[$l]}MHz || return 1
                  }
                } || {
                  [ $isVerbose -gt 0 ] && {
                    echo "  - Setting governor for logical cpu$l to '${fs_governor_ac[$l]}' @ ${fs_min_freq_ac[$l]}MHz ... ${fs_max_freq_ac[$l]}MHz"
                  }
                  [ $isDryRun -ne 0 ] && {
                    >&2 echo "cpufreq-set --cpu $l --governor ${fs_governor_ac[$l]} --min ${fs_min_freq_ac[$l]}MHz --max ${fs_max_freq_ac[$l]}MHz"
                  } || {
                    cpufreq-set --cpu $l --governor ${fs_governor_ac[$l]} --min ${fs_min_freq_ac[$l]}MHz --max ${fs_max_freq_ac[$l]}MHz || return 1
                  }
                }
              }
            }
          done
        } || {
          #
          # On AC power (or unknown), program AC settings.
          # Loop over the logical cpus of this processor...
          for l in ${cpuid_logical_cpus[$p]}; do
            [ "${fs_enabled[$l]}" = "true" ] && {
              [ "${fs_governor_ac[$l]}" = "userspace" ] && {
                [ $isVerbose -gt 0 ] && {
                  echo "  - Setting governor for logical cpu$l to 'userspace' @ ${fs_freq_ac[$l]}MHz"
                }
                [ $isDryRun -ne 0 ] && {
                  >&2 echo "cpufreq-set --cpu $l --governor ${fs_governor_ac[$l]}"
                  >&2 echo "cpufreq-set --cpu $l --freq ${fs_freq_ac[$l]}MHz"
                } || {
                  cpufreq-set --cpu $l --governor ${fs_governor_ac[$l]} || return 1
                  cpufreq-set --cpu $l --freq ${fs_freq_ac[$l]}MHz || return 1
                }
              } || {
                [ $isVerbose -gt 0 ] && {
                  echo "  - Setting governor for logical cpu$l to '${fs_governor_ac[$l]}' @ ${fs_min_freq_ac[$l]}MHz ... ${fs_max_freq_ac[$l]}MHz"
                }
                [ $isDryRun -ne 0 ] && {
                  >&2 echo "cpufreq-set --cpu $l --governor ${fs_governor_ac[$l]} --min ${fs_min_freq_ac[$l]}MHz --max ${fs_max_freq_ac[$l]}MHz"
                } || {
                  cpufreq-set --cpu $l --governor ${fs_governor_ac[$l]} --min ${fs_min_freq_ac[$l]}MHz --max ${fs_max_freq_ac[$l]}MHz || return 1
                }
              }
            }
          done
        }        
        #
        # Success (return here or fs_per_thread==false gets executed)
        return 0
      } || {
        #
        # No, program settings of the first logical cpu for all cpus for this processor
        GetLogicalCpu $p 0 cpu || return 1
        #
        # DC mode enabled?
        [ "${fs_batt_g[$p]}" = "true" ] && {
          #
          # Yes, DC mode enabled. Get the AC/DC status.
          /usr/bin/on_ac_power
          [ $? -eq 1 ] && {
            #
            # On DC power, program DC settings.
            for l in ${cpuid_logical_cpus[$p]}; do
              [ "${fs_governor_dc[$cpu]}" = "userspace" ] && {
                [ $isVerbose -gt 0 ] && {
                  echo "  - Setting governor for logical cpu$l to 'userspace' @ ${fs_freq_dc[$cpu]}MHz"
                }
                [ $isDryRun -ne 0 ] && {
                  >&2 echo "cpufreq-set --cpu $l --governor ${fs_governor_dc[$cpu]}"
                  >&2 echo "cpufreq-set --cpu $l --freq ${fs_freq_dc[$cpu]}MHz"
                } || {
                  cpufreq-set --cpu $l --governor ${fs_governor_dc[$cpu]} || return 1
                  cpufreq-set --cpu $l --freq ${fs_freq_dc[$cpu]}MHz || return 1
                }
              } || {
                [ $isVerbose -gt 0 ] && {
                  echo "  - Setting governor for logical cpu$l to '${fs_governor_dc[$cpu]}' @ ${fs_min_freq_dc[$cpu]}MHz ... ${fs_max_freq_dc[$cpu]}MHz"
                }
                [ $isDryRun -ne 0 ] && {
                  >&2 echo "cpufreq-set --cpu $l --governor ${fs_governor_dc[$cpu]} --min ${fs_min_freq_dc[$cpu]}MHz --max ${fs_max_freq_dc[$cpu]}MHz"
                } || {
                  cpufreq-set --cpu $l --governor ${fs_governor_dc[$cpu]} --min ${fs_min_freq_dc[$cpu]}MHz --max ${fs_max_freq_dc[$cpu]}MHz || return 1
                }
              }
            done
          } || {
            #
            # On AC power (or unknown), program AC settings.
            for l in ${cpuid_logical_cpus[$p]}; do
              [ "${fs_governor_ac[$cpu]}" = "userspace" ] && {
                [ $isVerbose -gt 0 ] && {
                  echo "  - Setting governor for logical cpu$l to 'userspace' @ ${fs_freq_ac[$cpu]}MHz"
                }
                [ $isDryRun -ne 0 ] && {
                  >&2 echo "cpufreq-set --cpu $l --governor ${fs_governor_ac[$cpu]}"
                  >&2 echo "cpufreq-set --cpu $l --freq ${fs_freq_ac[$cpu]}MHz"
                } || {
                  cpufreq-set --cpu $l --governor ${fs_governor_ac[$cpu]} || return 1
                  cpufreq-set --cpu $l --freq ${fs_freq_ac[$cpu]}MHz || return 1
                }
              } || {
                [ $isVerbose -gt 0 ] && {
                  echo "  - Setting governor for logical cpu$l to '${fs_governor_ac[$cpu]}' @ ${fs_min_freq_ac[$cpu]}MHz ... ${fs_max_freq_ac[$cpu]}MHz"
                }
                [ $isDryRun -ne 0 ] && {
                  >&2 echo "cpufreq-set --cpu $l --governor ${fs_governor_ac[$cpu]} --min ${fs_min_freq_ac[$cpu]}MHz --max ${fs_max_freq_ac[$cpu]}MHz"
                } || {
                  cpufreq-set --cpu $l --governor ${fs_governor_ac[$cpu]} --min ${fs_min_freq_ac[$cpu]}MHz --max ${fs_max_freq_ac[$cpu]}MHz || return 1
                }
              }
            done
          }
        } || {
          #
          # No DC mode, program AC settings.
          for l in ${cpuid_logical_cpus[$p]}; do
            [ "${fs_governor_ac[$cpu]}" = "userspace" ] && {
              [ $isVerbose -gt 0 ] && {
                echo "  - Setting governor for logical cpu$l to 'userspace' @ ${fs_freq_ac[$cpu]}MHz"
              }
              [ $isDryRun -ne 0 ] && {
                >&2 echo "cpufreq-set --cpu $l --governor ${fs_governor_ac[$cpu]}"
                >&2 echo "cpufreq-set --cpu $l --freq ${fs_freq_ac[$cpu]}MHz"
              } || {
                cpufreq-set --cpu $l --governor ${fs_governor_ac[$cpu]} || return 1
                cpufreq-set --cpu $l --freq ${fs_freq_ac[$cpu]}MHz || return 1
              }
            } || {
              [ $isVerbose -gt 0 ] && {
                echo "  - Setting governor for logical cpu$l to '${fs_governor_ac[$cpu]}' @ ${fs_min_freq_ac[$cpu]}MHz ... ${fs_max_freq_ac[$cpu]}MHz"
              }
              [ $isDryRun -ne 0 ] && {
                >&2 echo "cpufreq-set --cpu $l --governor ${fs_governor_ac[$cpu]} --min ${fs_min_freq_ac[$cpu]}MHz --max ${fs_max_freq_ac[$cpu]}MHz"
              } || {
                cpufreq-set --cpu $l --governor ${fs_governor_ac[$cpu]} --min ${fs_min_freq_ac[$cpu]}MHz --max ${fs_max_freq_ac[$cpu]}MHz || return 1
              }
            }
          done
        }
      }
    }
  done
  #
  # Success
  return 0
}

