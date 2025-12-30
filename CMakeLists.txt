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
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

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
set(ARMSemihostingCompileDefinitions
    USE_HAL_DRIVER
    STM32F407xx
    $<$<CONFIG:Debug>:DEBUG>
)
set(ARMSemihostingIncludeDirectories
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)

include(arm-semihosting)

add_arm_semihosting_test(TEST_NAME correlate_f32_test
    TEST_SOURCES
        ${CMAKE_SOURCE_DIR}/Tests/correlate_f32_test.c
        ${CMAKE_SOURCE_DIR}/Tests/fcvtf.c
    APP_SOURCES
        ${CMAKE_SOURCE_DIR}/Core/Src/correlate_f32.c
        ${CMAKE_SOURCE_DIR}/Core/Src/ring_buf.c
        ${CMAKE_SOURCE_DIR}/Core/Src/ring_buf_circ.c
)
