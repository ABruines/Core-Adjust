#
# CMakeLists.txt - Build/install the Core Adjust script for acpid
#

# Create the script
configure_file(core-adjust.in core-adjust @ONLY)

# Install the script
# Note: destination is a system directory, ie. use a hardcoded absolute path
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/core-adjust"
  DESTINATION /etc/acpid)

# Post-Install: restart the acpid service
install(CODE "\
  set(do_cmd \"${ENABLE_SERVICE}\")
  if(do_cmd)
    message(STATUS \"Restarting the acpid service...\")
    execute_process(COMMAND \"systemctl\" \"restart\" \"acpid\")
  endif()")

