target_sources(${PROJECT_NAME} PRIVATE
        CommandBufferBase.cxx
        CommandPoolBase.cxx
        ComputeCommandBuffer.cxx
        ComputeQueueRef.cxx
        GraphicsCommandBuffer.cxx
        GraphicsQueueRef.cxx
        PresentQueueRef.cxx
        Queue.cxx
        QueueRef.cxx
        QueueRefBase.cxx
        QueueProvider.cxx
        QueueProviderBuilder.cxx
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
        CommandPoolBase.ixx
        CommandPoolFlags.ixx
        ComputeCommandBuffer.ixx
        ComputeCommandPool.ixx
        ComputeQueueRef.ixx
        DependencyInfo.ixx
        GraphicsCommandBuffer.ixx
        GraphicsCommandPool.ixx
        GraphicsQueueRef.ixx
        PresentQueueRef.ixx
        Queue.ixx
        QueueRef.ixx
        QueueRefBase.ixx
        QueueProvider.ixx
        QueueProviderBuilder.ixx
        SubmitInfo.ixx
        TransferCommandBuffer.ixx
        TransferCommandPool.ixx
        TransferQueueRef.ixx
)
