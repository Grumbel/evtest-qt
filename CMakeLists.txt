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

cmake_minimum_required(VERSION 3.0)
project(evtest-qt)

include(GNUInstallDirs)

option(WARNINGS "Switch on extra warnings" OFF)
option(WERROR "Turn warnings into errors" OFF)
option(BUILD_TESTS "Build test cases" OFF)

find_package(Git REQUIRED)

execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always HEAD
  OUTPUT_VARIABLE GIT_REPO_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=%cd --date=format:%Y-%m-%d ${GIT_REPO_VERSION}
  OUTPUT_VARIABLE GIT_REPO_DATE
  OUTPUT_STRIP_TRAILING_WHITESPACE)

add_definitions(-DEVTEST_QT_VERSION="${GIT_REPO_VERSION}")

find_package(Qt5 COMPONENTS Core Widgets)

if (Qt5_FOUND)
  set(QT_LIBRARIES Qt5::Core Qt5::Widgets)
  message(STATUS "Using Qt version: Qt5")
else()
  find_package(Qt4 REQUIRED)
  set(QT_LIBRARIES Qt4::QtGui)
  message(STATUS "Using Qt version: Qt4")
endif()

if(WARNINGS)
  set(WARNINGS_CXX_FLAGS ${WARNINGS_CXX_FLAGS}
    -pedantic -Wall -Wextra -Wno-c++0x-compat -Wnon-virtual-dtor -Weffc++
    -Wconversion -Wshadow -Wcast-qual -Winit-self -Wno-unused-parameter)
  # -Winline -Wfloat-equal -Wunreachable-code
endif()

if(WERROR)
  set(WARNINGS_CXX_FLAGS ${WARNINGS_CXX_FLAGS}
    -Werror)
endif()

if (BUILD_TESTS)
  enable_testing()
endif(BUILD_TESTS)

add_definitions(${WARNINGS_CXX_FLAGS})

# remove 'override' keyword from source
add_definitions(-Doverride=)

include_directories(SYSTEM
  ${QT_INCLUDE_DIR}
  ${QT_QTGUI_INCLUDE_DIR}
  ${QT_QTCORE_INCLUDE_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_library(jslib STATIC src/
  src/axis_widget.cpp
  src/rel_widget.cpp
  src/button_widget.cpp
  src/evdev_device.cpp
  src/evdev_info.cpp
  src/evdev_enum.cpp
  src/evdev_state.cpp
  src/evdev_list.cpp
  src/evdev_widget.cpp
  src/evtest_app.cpp
  src/multitouch_widget.cpp
  src/stick_widget.cpp)
target_link_libraries(jslib ${QT_LIBRARIES})

file(GLOB EVTEST_QT_SOURCES src/main.cpp)
add_executable(evtest-qt ${EVTEST_QT_SOURCES})
target_link_libraries(evtest-qt jslib)

add_executable(evdev-test src/evdev_test.cpp)
target_link_libraries(evdev-test jslib)

install(TARGETS evtest-qt
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
