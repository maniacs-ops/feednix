cmake_minimum_required(VERSION 3.2.0)
project(feednix)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Curses REQUIRED)
find_package(CURL REQUIRED)
find_package(JsonCpp REQUIRED)

add_subdirectory(src)
