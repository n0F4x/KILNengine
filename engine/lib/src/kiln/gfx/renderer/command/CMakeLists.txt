target_sources(${PROJECT_NAME} PRIVATE
        CommandBufferBase.cxx
        CommandPoolBase.cxx
        CommandPluginInjection.cxx
        GraphicsCommandBuffer.cxx
        GraphicsQueueRef.cxx
        queue_requests.cxx
        QueueProvider.cxx
        QueueProviderPlugin.cxx
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
        CommandPoolBase.ixx
        CommandPoolFlags.ixx
        CommandPlugin.ixx
        CommandPluginInjection.ixx
        GraphicsCommandBuffer.ixx
        GraphicsCommandPool.ixx
        GraphicsQueueRef.ixx
        queue_requests.ixx
        QueueProvider.ixx
        QueueProviderPlugin.ixx
        QueueProviderPluginInjection.ixx
        QueueRefBase.ixx
        SubmitInfo.ixx
        TransferCommandBuffer.ixx
        TransferCommandPool.ixx
        TransferQueueRef.ixx
)
