#
# CMakeLists.txt - Install the policykit policy for Core Adjust
#

configure_file(org.freedesktop.policykit.core-adjust-qt.policy.in
  org.freedesktop.policykit.core-adjust-qt.policy @ONLY)

# Install the script
# Note: destination is a system directory, ie. use a hardcoded absolute path
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.policykit.core-adjust-qt.policy"
  DESTINATION /usr/share/polkit-1/actions)

