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

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Project version (VERSION file is the source of truth) ---
# Nix/packaging may pass -DPROJECT_VERSION_FULL=... to embed a git revision.
if(NOT DEFINED PROJECT_VERSION_FULL)
  if(NOT EXISTS "${CMAKE_SOURCE_DIR}/VERSION")
    message(FATAL_ERROR
      "VERSION file not found at ${CMAKE_SOURCE_DIR}/VERSION. "
      "The VERSION file is the sole source of the project version.")
  endif()
  file(STRINGS "${CMAKE_SOURCE_DIR}/VERSION" PROJECT_VERSION_FULL LIMIT_COUNT 1)
endif()
string(STRIP "${PROJECT_VERSION_FULL}" PROJECT_VERSION_FULL)
if(PROJECT_VERSION_FULL STREQUAL "")
  message(FATAL_ERROR "PROJECT_VERSION_FULL is empty")
endif()
set(PROJECT_VERSION "${PROJECT_VERSION_FULL}")
message(STATUS "Project version: ${PROJECT_VERSION}")

# --- Install directories ---
include(GNUInstallDirs)

# --- libudev (input hotplug monitoring) ---
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUDEV REQUIRED libudev)
pkg_check_modules(LIBEVDEV REQUIRED libevdev)

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

# --- Qt (Qt6 preferred, Qt5 supported; Qt4 is not supported) ---
find_package(Qt6 COMPONENTS Core Widgets)
if(Qt6_FOUND)
  set(QT_LIBRARIES Qt6::Core Qt6::Widgets)
  message(STATUS "Using Qt version: Qt6")
  qt_standard_project_setup()
else()
  find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
  set(QT_LIBRARIES Qt5::Core Qt5::Widgets)
  message(STATUS "Using Qt version: Qt5")
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
target_include_directories(evtest-qt PUBLIC src/ ${LIBUDEV_INCLUDE_DIRS} ${LIBEVDEV_INCLUDE_DIRS})
target_link_libraries(evtest-qt PUBLIC ${QT_LIBRARIES} ${LIBUDEV_LIBRARIES} ${LIBEVDEV_LIBRARIES})
target_compile_options(evtest-qt PRIVATE ${EVTEST_QT_WARNINGS_CXX_FLAGS} ${LIBUDEV_CFLAGS_OTHER} ${LIBEVDEV_CFLAGS_OTHER})
target_compile_definitions(evtest-qt PRIVATE -DEVTEST_QT_VERSION="${PROJECT_VERSION}")

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
