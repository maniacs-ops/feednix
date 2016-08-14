cmake_minimum_required(VERSION 2.8.7)
project(feednix)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Curses REQUIRED)
find_package(CURL REQUIRED)
find_package(JsonCpp REQUIRED)

add_subdirectory(src)
