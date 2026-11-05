cmake_minimum_required(VERSION 3.22)
project(sitri C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(sitri.elf
    firmware/src/main.c)

target_include_directories(sitri.elf PRIVATE firmware/include)
target_compile_options(sitri.elf PRIVATE -Wall -Wextra -g -ffunction-sections -fdata-sections)