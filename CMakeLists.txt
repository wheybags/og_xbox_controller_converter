cmake_minimum_required(VERSION 3.22)
project(xbox)

set(CMAKE_CXX_STANDARD 14)

# this is not intended to actually build the project, just provide enough information to clion that I can get usable indexing

add_definitions(-D__IMXRT1062__ -DTEENSYDUINO=156 -DARDUINO=10819 -DARDUINO_TEENSY41 -DF_CPU=600000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH)
include_directories("C:\\Program Files (x86)\\Arduino\\hardware\\teensy\\avr\\cores\\teensy4")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h src/*.hpp)
add_library(xbox STATIC ${sources})
