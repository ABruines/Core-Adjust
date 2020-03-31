#
# CMakeLists.txt - Install the XDG componenents for Core Adjust
#

# Only register the xdg components when ENABLE_XDG is true,
# ie. when not creating a debian package.
# When a debian package is built the postinst script takes care that.

configure_file(core-adjust.desktop.in core-adjust.desktop @ONLY)

# Make sure this directory exists on the target filesystem.
# This prevents a bug in xdg-desktop-menu when installing on a system
# without the X-Windows system installed.
# Note: destination is a system directory, ie. use a hardcoded absolute path
install(DIRECTORY DESTINATION /usr/share/desktop-directories)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/core-adjust.desktop" "core-adjust.png"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${CORE_ADJUST_DIR})

# Post-Install: XDG install of icon and desktop file.
install(
  CODE "set(do_cmd \"${ENABLE_XDG}\")"
  CODE "if(do_cmd)\nmessage(STATUS \"Register the Core Adjust icon and desktop file...\")\nendif()"
  CODE "set(shell_cmd \"xdg-icon-resource\" \"install\" \"--size\" \"64\" \"${CMAKE_INSTALL_DATADIR}/${CORE_ADJUST_DIR}/core-adjust.png\")"
  CODE "if(do_cmd)\nexecute_process(COMMAND \${shell_cmd})\nendif()"
  CODE "set(shell_cmd \"xdg-desktop-menu\" \"install\" \"${CMAKE_INSTALL_DATADIR}/${CORE_ADJUST_DIR}/core-adjust.desktop\")"
  CODE "if(do_cmd)\nexecute_process(COMMAND \${shell_cmd})\nendif()")

