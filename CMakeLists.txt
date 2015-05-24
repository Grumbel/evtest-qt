cmake_minimum_required(VERSION 3.0)
project(evtest-qt)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(Qt5Widgets)

add_library(jslib STATIC src/
  src/axis_widget.cpp
  src/button_widget.cpp
  src/evdev_controller.cpp
  src/evdev_device.cpp
  src/evdev_info.cpp
  src/evdev_enum.cpp
  src/evdev_state.cpp
  src/evdev_list.cpp
  src/evtest_app.cpp
  src/stick_widget.cpp)
target_link_libraries(jslib Qt5::Widgets)

file(GLOB EVTEST_QT_SOURCES src/main.cpp)
add_executable(evtest-qt ${EVTEST_QT_SOURCES})
target_link_libraries(evtest-qt Qt5::Widgets jslib)

add_executable(evdev-test src/evdev_test.cpp)
target_link_libraries(evdev-test jslib)

# EOF #