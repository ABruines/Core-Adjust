#
# Toplevel CMakeLists.txt for Core Adjust
#

cmake_minimum_required(VERSION 3.13)

#
# Prevent in-source builds
#

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR
    "\nIn-source builds not allowed.\n"
    "Please make a new directory (called a build directory) and run CMake from there.\n"
    "You may need to remove CMakeCache.txt.\n")
endif()

#
# setup the project info
#

project(core-adjust VERSION "1.0" LANGUAGES CXX)

# Set a local module path.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Locate the pthreads library
set(THREADS_PREFER_PTHREAD_FLAG ON CACHE BOOL "" FORCE)
find_package(Threads QUIET REQUIRED)

# Locate the Qt5 libraries
find_package(Qt5 QUIET COMPONENTS Widgets REQUIRED)

#
# Set the default installation prefixes
#

# We must set the default installation prefix to /usr
# so that CPack will create a correct package when CMake defaults are used.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "" FORCE)
endif()

include(GNUInstallDirs)

if(NOT DEFINED CORE_ADJUST_DIR)
  set(CORE_ADJUST_DIR core-adjust)
endif()
set(CORE_ADJUST_DIR ${CORE_ADJUST_DIR} CACHE STRING
  "The sub-directory to append to CMAKE_INSTALL_DATADIR and CMAKE_INSTALL_SYSCONFDIR."
  FORCE)

#
# specify the C++ standard and compiler flags
#

set(CMAKE_CXX_STANDARD 17 CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "" FORCE)

set(CMAKE_CXX_FLAGS
  "-frtti -fPIC -Wall -Wformat-security -fstack-protector-strong -D_FORTIFY_SOURCE=2"
  CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g3 -DNDEBUG" CACHE STRING "" FORCE)

#
# Setup linker flags
#

if(CMAKE_BUILD_TYPE MATCHES [Dd]?ebug OR
    CMAKE_BUILD_TYPE MATCHES [Rr]?el[Ww]?ith[Dd]?eb[Ii]?nfo )
  # Debug | RelWithDebInfo
  set(CMAKE_EXE_LINKER_FLAGS
    "-Wl,-call_shared,-z,relro,-z,now -fPIC -pie"
    CACHE STRING "" FORCE)
else()
  # Release | MinSizeRel
  set(CMAKE_EXE_LINKER_FLAGS
    "-Wl,-call_shared,-z,relro,-z,now -fPIC -pie -Xlinker --strip-all"
    CACHE STRING "" FORCE)
endif()

#
# Set the build type if it was not not set manually and
# create a nice dropdown menu to select it in cmake-gui.
#

set(default_build_type MinSizeRel)

set(CMAKE_CONFIGURATION_TYPES
  "Debug" "Release" "MinSizeRel" "RelWithDebInfo" CACHE STRING
  "The available configurations for this project." FORCE)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
  ${CMAKE_CONFIGURATION_TYPES})

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS
    "Setting CMAKE_BUILD_TYPE to '${default_build_type}' "
    "because none was specified.")
  set(CMAKE_BUILD_TYPE ${default_build_type})
endif()

set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING
  "Choose the type of build." FORCE)

#
# Set variables for and write config.h
#

if(CMAKE_BUILD_TYPE MATCHES [Dd]?ebug)
  set(DEBUG 1)
endif()

if(NOT DEFINED CONFIG_FILE)
  set(CONFIG_FILE ${CMAKE_INSTALL_FULL_SYSCONFDIR}/${CORE_ADJUST_DIR}/core-adjust.ini)
endif()
set(CONFIG_FILE ${CONFIG_FILE} CACHE STRING
  "Runtime location of the Core Adjust configuration file." FORCE)

if(NOT DEFINED SCRIPT_EXEC)
  set(SCRIPT_EXEC ${CMAKE_INSTALL_PREFIX}/bin/core-adjust)
endif()
set(SCRIPT_EXEC ${SCRIPT_EXEC} CACHE STRING
  "Runtime location of the Core Adjust shell script." FORCE)

if(NOT DEFINED GRUB_CFG_FILE)
  set(GRUB_CFG_FILE /etc/default/grub)
endif()
set(GRUB_CFG_FILE ${GRUB_CFG_FILE} CACHE STRING
  "Location of the GRUB configuration." FORCE)

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexander Bruines <alexander.bruines@gmail.com>")

configure_file(config.h.in config.h @ONLY)

#
# Enable the Core Adjust service when running the install target?
#

if(NOT DEFINED ENABLE_SERVICE)
  set(ENABLE_SERVICE No)
endif()

set(ENABLE_SERVICE ${ENABLE_SERVICE}
  CACHE BOOL "Install target installs the Core Adjust service?" FORCE)

message(STATUS
  "Install target installs the Core Adjust service? (${ENABLE_SERVICE})")

#
# XDG install the desktop file and icon?
#

if(NOT DEFINED ENABLE_XDG)
  set(ENABLE_XDG No)
endif()

set(ENABLE_XDG ${ENABLE_XDG} CACHE BOOL
  "Install target registers icons and desktop files?" FORCE)

message(STATUS
  "Install target registers icons and desktop files? (${ENABLE_XDG})")

#
# add the (next) subdirectory
#

add_subdirectory(src)

#
# CPack configuration
#

# Common CPack variables
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "")

# Multiple lines do not work here :-(
set(CPACK_PACKAGE_DESCRIPTION "Adjust various settings of Intel Processors.")
#Core Adjust is a software tool that is used to adjust various settings of")
#4th generation (Haswell) and later Intel(tm) processors on Debian/GNU and
#other Linux systems.")

set(CPACK_PACKAGE_HOMEPAGE_URL "https://core-adjust.sourceforge.io")
set(CPACK_PACKAGE_CHECKSUM SHA1)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)
set(CPACK_STRIP_FILES ON)
set(CPACK_VERBATIM_VARIABLES ON)
set(CPACK_SET_DESTDIR ON)

# source-package-generators CPack variables
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
set(CPACK_SOURCE_GENERATOR TBZ2)
set(CPACK_SOURCE_IGNORE_FILES ${CMAKE_BINARY_DIR} .git .gitignore)
set(CPACK_INSTALLED_DIRECTORIES "")

# CPack Debian (DEB) generator variables
set(CPACK_DEB_COMPONENT_INSTALL OFF)
set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
set(CPACK_DEBIAN_FILE_NAME ${PROJECT_NAME}_${PROJECT_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb)
set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_DEBIAN_PACKAGE_RELEASE "1")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "acpi-support, bash, bc, bsdutils, coreutils, cpufrequtils, cpuid, irqbalance, kmod, msr-tools, powermgmt-base, systemd, policykit-1, xdg-utils")
unset(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS)
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_DEBIAN_PACKAGE_SECTION "utilities")
set(CPACK_DEBIAN_ARCHIVE_TYPE "gnutar")
set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${CPACK_PACKAGE_HOMEPAGE_URL})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
unset(CPACK_DEBIAN_PACKAGE_PREDEPENDS)
set(CPACK_DEBIAN_PACKAGE_ENHANCES "cpufrequtils, grub-common")
unset(CPACK_DEBIAN_PACKAGE_BREAKS)
unset(CPACK_DEBIAN_PACKAGE_CONFLICTS)
unset(CPACK_DEBIAN_PACKAGE_PROVIDES)
unset(CPACK_DEBIAN_PACKAGE_REPLACES)
unset(CPACK_DEBIAN_PACKAGE_RECOMMENDS)
set(CPACK_DEBIAN_PACKAGE_SUGGESTS "grub-common")
unset(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS_POLICY "=")
unset(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)
unset(CPACK_DEBIAN_PACKAGE_SOURCE)

#
# CPackDebHelper configuration
#

#set(CPACK_DEBHELPER_VERBOSE ON)

string(TOLOWER ${CPACK_PACKAGE_NAME} DebPkgName)

set(CPACK_DEBHELPER_INPUT_DIR "${CMAKE_SOURCE_DIR}/debian")

# Files to process with debhelper
set(CPACK_DEBHELPER_INPUT
  service.in
  postinst.in
  prerm.in
  copyright.in
  changelog.in
  docs.in
  control.in
)

# Extra files to copy into the debian directory before running debhelper
set(CPACK_DEBHELPER_INPUT_EXTRA
  ${CMAKE_SOURCE_DIR}/README
)

include(CPackDebHelper)
include(CPack)

if(CPACK_DEBHELPER_FATALITY)
  unset(CPACK_GENERATOR)
else()
  # Add a 'deb' alias for the package target
  add_custom_target(deb COMMAND +make SOURCE_DATE_EPOCH="1585612680" package)
endif()

# Add a 'dist' alias for the package_source target
add_custom_target(dist COMMAND +make package_source)

