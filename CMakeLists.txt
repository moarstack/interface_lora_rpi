cmake_minimum_required(VERSION 3.0)
project(libmoar_interface_lora_rpi) 

set(LIBRARY_OUTPUT_PATH ../../lib)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g3 -fPIC")

file(GLOB SRC_FILES
    "src/*.c"
)

#add shared layers headers
include_directories(../shared/inc)

#add local headers
include_directories(inc)
#include wiring pi here
include_directories(${WIRING_PI_INC_PATH})
message("WiringPI include path = " ${WIRING_PI_INC_PATH})
#add library
add_library(moar_interface_lora_rpi SHARED ${SRC_FILES})
target_link_libraries(moar_interface_lora_rpi moar_shared)
target_link_libraries(moar_interface_lora_rpi wiringPi)