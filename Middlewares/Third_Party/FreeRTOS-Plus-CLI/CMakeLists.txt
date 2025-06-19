# FreeRTOS+CLI include paths
set(FreeRTOS_Plus_CLI_Include_Dirs
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS-Plus-CLI
)

# FreeRTOS+CLI sources
set(FreeRTOS_Plus_CLI_Src
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS-Plus-CLI/FreeRTOS_CLI.c
)

# Create FreeRTOS static library for FreeRTOS+CLI
add_library(FreeRTOS_CLI OBJECT)
target_include_directories(FreeRTOS_CLI PUBLIC ${FreeRTOS_Plus_CLI_Include_Dirs})
target_sources(FreeRTOS_CLI PRIVATE ${FreeRTOS_Plus_CLI_Src})
target_link_libraries(FreeRTOS_CLI PUBLIC FreeRTOS)
