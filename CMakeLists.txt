cmake_minimum_required(VERSION 3.22)
project(sitri C CXX ASM)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(sitri.elf
    firmware/src/main.c
    firmware/startup/startup_an385.cpp
)

target_include_directories(sitri.elf PRIVATE firmware/include)
target_compile_options(sitri.elf PRIVATE
    -Wall -Wextra -g -ffunction-sections -fdata-sections
    "$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions;-fno-rtti>"
)

# linker script related CMAKE commands
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/firmware/linker/mps2_an385.ld)

target_link_options(sitri.elf PRIVATE
    -T${LINKER_SCRIPT}
    -Wl,-Map=$<TARGET_FILE_DIR:sitri.elf>/sitri.map
    -Wl,--print-memory-usage
)

set_target_properties(sitri.elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

add_custom_command(TARGET sitri.elf POST_BUILD
    COMMAND arm-none-eabi-size $<TARGET_FILE:sitri.elf>
)