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

cmake_minimum_required(VERSION 2.8)
project(evtest-qt)

include(GNUInstallDirs)

option(WARNINGS "Switch on extra warnings" OFF)
option(WERROR "Turn warnings into errors" OFF)

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

add_definitions(${WARNINGS_CXX_FLAGS})
include_directories(${Qt4Widgets_INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Qt4 REQUIRED)

add_definitions(-fPIC)

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
target_link_libraries(jslib PUBLIC Qt4::QtGui)

file(GLOB EVTEST_QT_SOURCES src/main.cpp)
add_executable(evtest-qt ${EVTEST_QT_SOURCES})
target_link_libraries(evtest-qt jslib)

add_executable(evdev-test src/evdev_test.cpp)
target_link_libraries(evdev-test jslib)

install(TARGETS evtest-qt
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# install(FILES
#   ${CMAKE_CURRENT_SOURCE_DIR}/doc/man/pingus.6
#   DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)

# EOF #
