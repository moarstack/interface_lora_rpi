cmake_minimum_required(VERSION 3.0)
project(libmoar_interface_lora_rpi)

file(GLOB SRC_FILES
        "src/*.c"
        )

if(${DISABLE_IO})
    if(${EMULATE_IO})
        message("IO Emulated")
        add_definitions(-DDEBUG_THREADS)
    else(${EMULATE_IO})
        message("IO Disabled")
    endif(${EMULATE_IO})
else(${DISABLE_IO})
    message("IO Enabled")
    add_definitions(-DENABLE_IO)
endif(${DISABLE_IO})

message(${CMAKE_C_FLAGS})
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
#target_link_libraries(moar_interface_lora_rpi pthread)