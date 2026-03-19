target_sources(${PROJECT_NAME} PRIVATE
        CommandPoolBase.cxx
        GraphicsCommandBuffer.cxx
        GraphicsQueueRef.cxx
        queue_requests.cxx
        QueueProvider.cxx
        QueueProviderPlugin.cxx
        TransferCommandBuffer.cxx
        TransferQueueRef.cxx
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
        GraphicsQueueRef.ixx
        queue_requests.ixx
        QueueProvider.ixx
        QueueProviderPlugin.ixx
        QueueProviderPluginInjection.ixx
        SubmitInfo.ixx
        TransferCommandBuffer.ixx
        TransferCommandPool.ixx
        TransferQueueRef.ixx
)
