target_sources(${PROJECT_NAME} PRIVATE
        GraphicsCommandBuffer.cxx
        TransferCommandBuffer.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        command.cppm
        CommandBufferUsageFlags.ixx
        CommandPoolBase.ixx
        CommandPoolFlags.ixx
        GraphicsCommandBuffer.ixx
        GraphicsCommandPool.ixx
        TransferCommandBuffer.ixx
        TransferCommandPool.ixx
)
