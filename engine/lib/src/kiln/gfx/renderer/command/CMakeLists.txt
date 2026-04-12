target_sources(${PROJECT_NAME} PRIVATE
        CommandBufferBase.cxx
        CommandContextBuilder.cxx
        CommandPoolBase.cxx
        GraphicsCommandBuffer.cxx
        GraphicsQueue.cxx
        QueueBase.cxx
        QueueProvider.cxx
        TransferCommandBuffer.cxx
        TransferQueue.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        command.cppm
        CommandBufferBase.ixx
        CommandBufferUsageFlags.ixx
        CommandContext.ixx
        CommandContextBuilder.ixx
        CommandPoolBase.ixx
        CommandPoolFlags.ixx
        DependencyInfo.ixx
        GraphicsCommandBuffer.ixx
        GraphicsCommandPool.ixx
        GraphicsQueue.ixx
        QueueBase.ixx
        QueueProvider.ixx
        QueueProviderBuilder.ixx
        SubmitInfo.ixx
        TransferCommandBuffer.ixx
        TransferCommandPool.ixx
        TransferQueue.ixx
)
