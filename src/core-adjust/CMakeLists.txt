#
# CMakeLists.txt - Build/install the Core Adjust shell script
#

# Name of the output script
set(CORE_ADJUST_SCRIPT core-adjust)

# Arguments to the bash command that generates the script.
# ie.: monolith.sh <source-script> <working-directory>
set(MyArgs
  ${CMAKE_CURRENT_SOURCE_DIR}/monolith.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/core-adjust.sh
  ${CMAKE_CURRENT_SOURCE_DIR})

# The file that receives the output of the above shell command
set(MyOutput
  ${CMAKE_CURRENT_BINARY_DIR}/${CORE_ADJUST_SCRIPT})

# All the files that the generated shell script depends on
set(MyDepends
  ${CMAKE_CURRENT_SOURCE_DIR}/core-adjust.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/debug.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/actions.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cpu-info.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cmdline.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/target-temp.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/fivr-voffset.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/smt.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/msr_turbo_ratio.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ia32_misc_enable.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/msr_therm2_ctl.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/freq-scale.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/src/help.sh)

# The command to generate the Core Adjust shell script
add_custom_command(
  OUTPUT ${CORE_ADJUST_SCRIPT}
  COMMAND bash ${MyArgs} > ${MyOutput} && chmod +x ${MyOutput}
  COMMAND sed -i \"s|\\\@CONFIG_FILE\\\@|${CONFIG_FILE}|g\" ${MyOutput}
  COMMAND sed -i \"s|\\\@PROJECT_VERSION\\\@|${PROJECT_VERSION}|g\" ${MyOutput}
  DEPENDS ${MyDepends})

# Add the above command to the ALL target
add_custom_target(core-adjust-script ALL DEPENDS ${CORE_ADJUST_SCRIPT})

# Install the shell script
install(
  PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${CORE_ADJUST_SCRIPT}
  DESTINATION ${CMAKE_INSTALL_BINDIR})

