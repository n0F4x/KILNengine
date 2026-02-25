target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        Bundle.ixx
        CommandBufferHandle.ixx
        CommandCenter.ixx
        CommandPool.ixx
        Device.ixx
        DevicePlugin.ixx
        DevicePluginFailedError.ixx
        DevicePluginInjection.ixx
)
