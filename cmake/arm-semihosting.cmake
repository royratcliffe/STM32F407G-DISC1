# CMake module for setting up ARM semihosting tests.

# CMake function to add ARM semihosting tests.
# Parameters:
# TEST_NAME - Name of the test executable.
# TEST_SOURCES - List of source files for the test.
# APP_SOURCES - List of application source files to include in the test.
# Usage:
# add_arm_semihosting_test(TEST_NAME my_test
#     TEST_SOURCES
#         test1.c
#         test2.c
#     APP_SOURCES
#         app1.c
#         app2.c
# )
function(add_arm_semihosting_test)
    set(options)
    set(oneValueArgs TEST_NAME)
    set(multiValueArgs TEST_SOURCES APP_SOURCES)
    cmake_parse_arguments(AAST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${AAST_TEST_NAME})
    target_sources(${AAST_TEST_NAME} PRIVATE
        ${AAST_TEST_SOURCES}
        ${AAST_APP_SOURCES}
        ${CMAKE_SOURCE_DIR}/Core/Src/system_stm32f4xx.c
        ${CMAKE_SOURCE_DIR}/startup_stm32f407xx.s
    )
    target_include_directories(${AAST_TEST_NAME} PRIVATE ${ARMSemihostingIncludeDirectories})
    target_compile_definitions(${AAST_TEST_NAME} PRIVATE ${ARMSemihostingCompileDefinitions})

    # Link against the ARM Cortex-M4 math library.
    target_link_libraries(${AAST_TEST_NAME} PRIVATE arm_cortexM4lf_math)

    target_link_options(${AAST_TEST_NAME} PRIVATE --specs=rdimon.specs -lrdimon)
    set_target_properties(${AAST_TEST_NAME} PROPERTIES TIMEOUT 30)
    add_test(NAME ${AAST_TEST_NAME} COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} ${CMAKE_BINARY_DIR}/${AAST_TEST_NAME}.elf)
endfunction()
