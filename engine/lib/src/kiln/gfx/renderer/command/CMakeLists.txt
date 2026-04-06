target_sources(${PROJECT_NAME} PRIVATE
        CommandBufferBase.cxx
        CommandContextBuilder.cxx
        CommandPoolBase.cxx
        GraphicsCommandBuffer.cxx
        GraphicsQueueRef.cxx
        queue_requests.cxx
        QueueProvider.cxx
        QueueRefBase.cxx
        TransferCommandBuffer.cxx
        TransferQueueRef.cxx
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
        GraphicsQueueRef.ixx
        queue_requests.ixx
        QueueProvider.ixx
        QueueProviderBuilder.ixx
        QueueRefBase.ixx
        SubmitInfo.ixx
        TransferCommandBuffer.ixx
        TransferCommandPool.ixx
        TransferQueueRef.ixx
)
