# evtest-qt - A graphical joystick tester
# Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.7)
project(evtest-qt)

include(mk/cmake/TinyCMMC.cmake)

find_package(Qt6 COMPONENTS Core Widgets)
if(Qt6_FOUND)
  set(QT_LIBRARIES Qt6::Core Qt6::Widgets)
  message(STATUS "Using Qt version: Qt6")
else()
  find_package(Qt5 COMPONENTS Core Widgets)
  if(Qt5_FOUND)
    set(QT_LIBRARIES Qt5::Core Qt5::Widgets)
    message(STATUS "Using Qt version: Qt5")
  else()
    find_package(Qt4 REQUIRED)
    set(QT_LIBRARIES Qt4::QtGui)
    message(STATUS "Using Qt version: Qt4")
  endif()
endif()

if(Qt6_FOUND)
  qt_standard_project_setup()
else()
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)
  set(CMAKE_AUTOUIC ON)
endif()

if (BUILD_TESTS)
  enable_testing()
endif(BUILD_TESTS)

file(GLOB EVTEST_QT_SOURCES
  src/axis_widget.cpp
  src/button_widget.cpp
  src/evdev_device.cpp
  src/evdev_enum.cpp
  src/evdev_info.cpp
  src/evdev_list.cpp
  src/evdev_state.cpp
  src/evdev_widget.cpp
  src/evtest_app.cpp
  src/multitouch_widget.cpp
  src/rel_widget.cpp
  src/stick_widget.cpp
  )
add_library(evtest-qt STATIC ${EVTEST_QT_SOURCES})
target_link_libraries(evtest-qt PUBLIC ${QT_LIBRARIES})
target_compile_options(evtest-qt PRIVATE ${TINYCMMC_WARNINGS_CXX_FLAGS})

file(GLOB EVTEST_QT_MAIN_SOURCES
  src/main.cpp
  )
add_executable(evtest-qt_exe ${EVTEST_QT_MAIN_SOURCES})
target_link_libraries(evtest-qt_exe evtest-qt)
set_target_properties(evtest-qt_exe PROPERTIES
  OUTPUT_NAME "evtest-qt")
target_compile_options(evtest-qt_exe PRIVATE ${TINYCMMC_WARNINGS_CXX_FLAGS})
target_compile_definitions(evtest-qt_exe PRIVATE -DEVTEST_QT_VERSION="${PROJECT_VERSION}")

add_executable(evdev-test src/evdev_test.cpp)
target_link_libraries(evdev-test evtest-qt)
target_compile_options(evdev-test PRIVATE ${TINYCMMC_WARNINGS_CXX_FLAGS})

install(TARGETS evtest-qt_exe
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES
  evtest-qt.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

install(FILES
  evtest-qt.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

configure_file(evtest-qt.appdata.xml.in ${CMAKE_CURRENT_BINARY_DIR}/evtest-qt.appdata.xml)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/evtest-qt.appdata.xml
  DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

if (BUILD_TESTS)
  add_test(NAME evtest-qt.appdata.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND appstream-util validate-relax ${CMAKE_CURRENT_BINARY_DIR}/evtest-qt.appdata.xml)
endif(BUILD_TESTS)

# EOF #
