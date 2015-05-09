cmake_minimum_required(VERSION 3.0)
project(jstest-qt)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(Qt5Widgets)

file(GLOB JSTEST_QT_SOURCES src/*.cpp)
add_executable(jstest-qt ${JSTEST_QT_SOURCES})
target_link_libraries(jstest-qt Qt5::Widgets)

# EOF #