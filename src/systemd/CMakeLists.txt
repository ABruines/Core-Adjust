#
# CMakeLists.txt - Install the systemd service for Core Adjust
#

if(NOT DEFINED SYSTEMD_DIR)
  set(SYSTEMD_DIR "/lib/systemd/system")
endif()

set(SYSTEMD_DIR "${SYSTEMD_DIR}" CACHE STRING
  "The systemd service directory." FORCE)

# Only install/enable the service when ENABLE_SERVICE is true,
# ie. when not creating a debian package.
# When a debian package is built the postinst script takes care that.
if(ENABLE_SERVICE)
  configure_file(core-adjust.service.in core-adjust.service @ONLY)

  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/core-adjust.service"
    DESTINATION ${SYSTEMD_DIR})

  # Post-Install: enable the Core Adjust service
  install(CODE "\
    set(do_cmd \"${ENABLE_SERVICE}\")
    if(do_cmd)
      message(STATUS \"Enabling the Core Adjust service...\")
      execute_process(COMMAND \"systemctl\" \"enable\" \"core-adjust\")
    endif()")
endif()

