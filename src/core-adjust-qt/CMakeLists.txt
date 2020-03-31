#
# CMakeLists.txt - Build/install the Core Adjust Qt5 application
#

add_executable(core-adjust-qt
  CoreAdjust.cpp CoreAdjust.hpp
  CpuId.cpp CpuId.hpp
  CpuInfo.cpp CpuInfo.hpp
  CpuFreqUtils.cpp CpuFreqUtils.hpp
  CpuNumber.hpp
  Dbg.hpp
  Gauge.cpp Gauge.hpp
  Grub.hpp Grub.cpp
  MainWindow.cpp MainWindow.hpp
  MiscEnable.cpp MiscEnable.hpp
  Monitor.cpp Monitor.hpp
  Msr.cpp Msr.hpp
  MsrReadout.cpp MsrReadout.hpp
  SaferCombo.cpp SaferCombo.hpp
  SaferSlider.cpp SaferSlider.hpp
  ShellCommand.cpp ShellCommand.hpp
  SmtControl.cpp SmtControl.hpp
  SpeedControl.cpp SpeedControl.hpp
  TabMember.cpp TabMember.hpp
  TabMemberBase.cpp TabMemberBase.hpp
  ThermalControl.cpp ThermalControl.hpp
  ThermalStatus.cpp ThermalStatus.hpp
  VoltageOffsets.cpp VoltageOffsets.hpp
  VoltageOffsetSlider.cpp VoltageOffsetSlider.hpp
  main.cpp)

target_link_libraries (core-adjust-qt
  common
  Threads::Threads
  Qt5::Widgets)

set_target_properties(core-adjust-qt PROPERTIES
  AUTOMOC ON
  AUTOUIC ON)

target_include_directories(core-adjust-qt PUBLIC
  "${PROJECT_BINARY_DIR}"
  "${CMAKE_SOURCE_DIR}/src/libcommon")

install(
  PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/core-adjust-qt"
  DESTINATION ${CMAKE_INSTALL_BINDIR})

