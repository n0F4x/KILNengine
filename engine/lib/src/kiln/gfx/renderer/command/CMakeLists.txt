target_sources(${PROJECT_NAME} PRIVATE
        OneTimeTransferCommandBuffer.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        command.cppm
        CommandPool.ixx
        CommandPoolPlugin.ixx
        CommandPoolPluginInjection.ixx
        GraphicsCommandBuffer.ixx
        OneTimeTransferCommandBuffer.ixx
)
