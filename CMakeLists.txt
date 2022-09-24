cmake_minimum_required(VERSION 3.22)
project(xbox)

set(CMAKE_CXX_STANDARD 14)

# this is not intended to actually build the project, just provide enough information to clion that I can get usable indexing

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h src/*.hpp)


set(ARDUINO_DIR "C:/Program Files (x86)/Arduino")

add_definitions(-D__IMXRT1062__ -DTEENSYDUINO=156 -DARDUINO=10819 -DARDUINO_TEENSY41 -DF_CPU=600000000 -DUSB_XINPUT)

function(add_dependency_dir dir_path)
    include_directories("${dir_path}")
    file(GLOB_RECURSE temp CONFIGURE_DEPENDS "${dir_path}/*.c" "${dir_path}/*.cpp" "${dir_path}/*.h" "${dir_path}/*.hpp")
    set(sources ${sources} ${temp} PARENT_SCOPE)
endfunction()

add_dependency_dir("${ARDUINO_DIR}/hardware/teensy/avr/cores/teensy4")
add_dependency_dir("${ARDUINO_DIR}/hardware/teensy/avr/libraries/USBHost_t36")
add_dependency_dir("${ARDUINO_DIR}/libraries/ArduinoXInput/src")

add_library(xbox STATIC ${sources})
