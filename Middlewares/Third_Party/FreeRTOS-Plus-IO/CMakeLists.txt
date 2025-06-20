# FreeRTOS+IO include paths
set(FreeRTOS_Plus_IO_Include_Dirs
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS-Plus-IO/Include
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS-Plus-IO/Device/STM32xx
)

# FreeRTOS+IO sources
set(FreeRTOS_Plus_IO_Src
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS-Plus-IO/Common/FreeRTOS_DriverInterface.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS-Plus-IO/Common/FreeRTOS_IOUtils.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS-Plus-IO/Common/IOUtils_CharQueueTxAndRx.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS-Plus-IO/Common/IOUtils_CircularBufferRx.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS-Plus-IO/Common/IOUtils_ZeroCopyTx.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS-Plus-IO/Device/STM32xx/FreeRTOS_stm32xx_DriverInterface.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS-Plus-IO/Device/STM32xx/FreeRTOS_stm32xx_usb.c
)

# Create FreeRTOS static library for FreeRTOS+IO
add_library(FreeRTOS_IO OBJECT)
target_include_directories(FreeRTOS_IO PUBLIC ${FreeRTOS_Plus_IO_Include_Dirs})
target_sources(FreeRTOS_IO PRIVATE ${FreeRTOS_Plus_IO_Src})
target_link_libraries(FreeRTOS_IO PUBLIC FreeRTOS)
