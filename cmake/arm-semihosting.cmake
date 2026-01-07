# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2025, Roy Ratcliffe, Northumberland, United Kingdom
# CMake module for setting up ARM semihosting tests.

# CMake function to add ARM semihosting tests.
# Parameters:
# TEST_NAME - Name of the test executable.
# TEST_SOURCES - List of source files for the test.
# APP_SOURCES - List of application source files to include in the test.
# SYSTEM_SOURCES - List of system source files, e.g. startup code.
# LINK_LIBRARIES - List of libraries to link against.
# Usage:
# add_arm_semihosting_test(TEST_NAME my_test
#     TEST_SOURCES
#         test1.c
#         test2.c
#     APP_SOURCES
#         app1.c
#         app2.c
#     SYSTEM_SOURCES
#         system1.c
#         system2.c
#     LINK_LIBRARIES
#         library1
#         library2
# )
function(add_arm_semihosting_test)
    set(options)
    set(oneValueArgs TEST_NAME TIMEOUT)
    set(multiValueArgs TEST_SOURCES APP_SOURCES SYSTEM_SOURCES LINK_LIBRARIES)
    cmake_parse_arguments(AAST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${AAST_TEST_NAME})
    target_sources(${AAST_TEST_NAME} PRIVATE
        ${ARMSemihostingTestSources}
        ${AAST_TEST_SOURCES}
        ${ARMSemihostingAppSources}
        ${AAST_APP_SOURCES}
        ${ARMSemihostingSystemSources}
        ${AAST_SYSTEM_SOURCES}
    )
    target_include_directories(${AAST_TEST_NAME} PRIVATE ${ARMSemihostingIncludeDirectories})
    target_compile_definitions(${AAST_TEST_NAME} PRIVATE ${ARMSemihostingCompileDefinitions})

    # Link against the any additional libraries, e.g. ARM Cortex-M4 math.
    target_link_libraries(${AAST_TEST_NAME} PRIVATE
        ${ARMSemihostingLinkLibraries}
        ${AAST_LINK_LIBRARIES}
    )

    # Link with semihosting specifications. RDI monitor provides semihosting
    # support for ARM targets.
    target_link_options(${AAST_TEST_NAME} PRIVATE --specs=rdimon.specs -lrdimon)

    # Set timeout for the test if specified.
    if(AAST_TIMEOUT)
        set_target_properties(${AAST_TEST_NAME} PROPERTIES TIMEOUT ${AAST_TIMEOUT})
    endif()

    add_test(NAME ${AAST_TEST_NAME} COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:${AAST_TEST_NAME}>)
endfunction()
