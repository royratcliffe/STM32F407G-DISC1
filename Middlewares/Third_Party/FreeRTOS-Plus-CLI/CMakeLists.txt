set(FreeRTOS_Plus_CLI_Src
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS-Plus-CLI/FreeRTOS_CLI.c
)

# Create FreeRTOS static library for FreeRTOS+CLI
add_library(FreeRTOS_CLI OBJECT)
target_sources(FreeRTOS_CLI PRIVATE ${FreeRTOS_Plus_CLI_Src})
target_link_libraries(FreeRTOS_CLI PUBLIC FreeRTOS)
