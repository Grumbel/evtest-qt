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

set(WARNINGS 0 CACHE BOOL "Switch on extra warnings")

if(WARNINGS)
  set(WARNINGS_CXX_FLAGS
    -pedantic -Wall -Wextra -Wno-c++0x-compat -Wnon-virtual-dtor -Weffc++
    -Wconversion -Werror -Wshadow -Wcast-qual -Winit-self -Wno-unused-parameter)
  # -Winline -Wfloat-equal -Wunreachable-code
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(Qt5Widgets)

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
target_link_libraries(jslib Qt5::Widgets)
target_compile_options(jslib PRIVATE ${WARNINGS_CXX_FLAGS})

file(GLOB EVTEST_QT_SOURCES src/main.cpp)
add_executable(evtest-qt ${EVTEST_QT_SOURCES})
target_link_libraries(evtest-qt Qt5::Widgets jslib)
target_compile_options(evtest-qt PRIVATE ${WARNINGS_CXX_FLAGS})

add_executable(evdev-test src/evdev_test.cpp)
target_link_libraries(evdev-test jslib)
target_compile_options(evdev-test PRIVATE ${WARNINGS_CXX_FLAGS})

# EOF #