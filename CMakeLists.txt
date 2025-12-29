cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME STM32F407G-DISC1)


# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# Import the ARM Cortex-M4 math library.
add_library(arm_cortexM4lf_math STATIC IMPORTED)
set_target_properties(arm_cortexM4lf_math PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Middlewares/ST/ARM/DSP/Lib/libarm_cortexM4lf_math.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/Middlewares/ST/ARM/DSP/Inc
)

include(CTest)
enable_testing()

# Unit tests setup. Define include paths and symbols for unit tests. These are
# similar to the main project, but can differ as the tests exclude
# hardware-specific sources and middlewares.
set(MX_Test_Include_Dirs
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)
set(MX_Test_Defines_Syms
    USE_HAL_DRIVER
    STM32F407xx
    $<$<CONFIG:Debug>:DEBUG>
)

# This test is run using QEMU emulating a Netduino Plus 2 board (STM32F407VG).
# The test uses semihosting to report results back to the host.
# The test has a timeout of 30 seconds.
add_executable(a_test)
target_sources(a_test PRIVATE
    # Add test sources.
    ${CMAKE_SOURCE_DIR}/Tests/a_test.c
    ${CMAKE_SOURCE_DIR}/Tests/fcvtf.c
    # Add application sources.
    ${CMAKE_SOURCE_DIR}/Core/Src/ring_buf.c
    ${CMAKE_SOURCE_DIR}/Core/Src/ring_buf_circ.c
    ${CMAKE_SOURCE_DIR}/Core/Src/ring_buf_item.c
    ${CMAKE_SOURCE_DIR}/Core/Src/ring_buf_yield.c
    # Add system and startup sources.
    ${CMAKE_SOURCE_DIR}/Core/Src/system_stm32f4xx.c
    ${CMAKE_SOURCE_DIR}/startup_stm32f407xx.s
)
target_include_directories(a_test PRIVATE ${MX_Test_Include_Dirs})
target_compile_definitions(a_test PRIVATE ${MX_Test_Defines_Syms})
target_link_libraries(a_test PRIVATE arm_cortexM4lf_math)
target_link_options(a_test PRIVATE --specs=rdimon.specs -lrdimon)
set_target_properties(a_test PROPERTIES TIMEOUT 30)

add_test(NAME a_test COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} ${CMAKE_BINARY_DIR}/a_test.elf)
