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

# --- Project version (from VERSION file, git describe, or directory name) ---
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/mk/cmake")
include(GetProjectVersion)

# --- Install directories ---
include(GNUInstallDirs)

# --- Compiler warnings (replaces tinycmmc WARNINGS / WERROR) ---
option(WARNINGS "Enable extra compiler warnings" OFF)
option(WERROR "Treat warnings as errors" OFF)
option(BUILD_TESTS "Build tests" OFF)

set(EVTEST_QT_WARNINGS_CXX_FLAGS "")
if(WARNINGS)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    list(APPEND EVTEST_QT_WARNINGS_CXX_FLAGS
      -Wall
      -Wextra
      -Wpedantic
      -Wshadow
      -Wconversion
      -Wsign-conversion
      -Wdouble-promotion
      -Wformat=2
      -Wnull-dereference
      -Wold-style-cast
      -Woverloaded-virtual
      -Wnon-virtual-dtor
      -Wcast-align
      -Wno-unused
    )
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      list(APPEND EVTEST_QT_WARNINGS_CXX_FLAGS
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wuseless-cast
      )
    endif()
  elseif(MSVC)
    list(APPEND EVTEST_QT_WARNINGS_CXX_FLAGS /W4)
  endif()
endif()

if(WERROR)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    list(APPEND EVTEST_QT_WARNINGS_CXX_FLAGS -Werror)
  elseif(MSVC)
    list(APPEND EVTEST_QT_WARNINGS_CXX_FLAGS /WX)
  endif()
endif()

# --- Qt ---
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

if(BUILD_TESTS)
  enable_testing()
endif()

file(GLOB EVTEST_QT_SOURCES
  src/widgets/*.cpp
  src/evdev/*.cpp
  src/evtest_app.cpp
  )
add_library(evtest-qt STATIC ${EVTEST_QT_SOURCES})
target_include_directories(evtest-qt PUBLIC src/)
target_link_libraries(evtest-qt PUBLIC ${QT_LIBRARIES})
target_compile_options(evtest-qt PRIVATE ${EVTEST_QT_WARNINGS_CXX_FLAGS})

file(GLOB EVTEST_QT_MAIN_SOURCES
  src/main.cpp
  )
add_executable(evtest-qt_exe ${EVTEST_QT_MAIN_SOURCES})
target_link_libraries(evtest-qt_exe evtest-qt)
set_target_properties(evtest-qt_exe PROPERTIES
  OUTPUT_NAME "evtest-qt")
target_compile_options(evtest-qt_exe PRIVATE ${EVTEST_QT_WARNINGS_CXX_FLAGS})
target_compile_definitions(evtest-qt_exe PRIVATE -DEVTEST_QT_VERSION="${PROJECT_VERSION}")

add_executable(evdev-test src/evdev_test.cpp)
target_link_libraries(evdev-test evtest-qt)
target_compile_options(evdev-test PRIVATE ${EVTEST_QT_WARNINGS_CXX_FLAGS})

install(TARGETS evtest-qt_exe
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES
  evtest-qt.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

install(FILES
  evtest-qt.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

# EOF #
