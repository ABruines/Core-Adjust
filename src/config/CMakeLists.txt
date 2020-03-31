#
# CMakeLists.txt - Install the Core Adjust configuration file.
#

# Note: Use the absolute path to the sysconfdir so CPack functions correctly...
install(
  FILES "core-adjust.ini"
  DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/${CORE_ADJUST_DIR})

