# setting up for WSL

set(CMAKE_SYSTEMN_NAME Generic) # no OS (b-m)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

# CMake's compiler test can't link a full program w/o startup code yet
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m3 -mthumb")
set(CMAKE_ASM_FLAGS_INIT "-mcpu=cortex-m3 -mthumb")
set(CMAKE_EXE_LINKER_FLAGS_INIT  
   "-mcpu=cortex-m3 
    -mthumb 
    -nostartfiles 
    --specs=nano.specs 
    -Wl, 
    --gc-sections")
