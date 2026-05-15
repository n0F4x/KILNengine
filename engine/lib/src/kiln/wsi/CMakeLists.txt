add_subdirectory(error)
add_subdirectory(event)

target_sources(${PROJECT_NAME} PRIVATE
        Engine.cxx
        monitor_functions.cxx
        Window.cxx
        window_functions.cxx
        WindowProxy.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        wsi.cppm
        Context.ixx
        ContextBuilder.ixx
        CursorMode.ixx
        Engine.ixx
        EventConsumerQueueInterface.ixx
        FullScreenWindowSettings.ixx
        monitor_functions.ixx
        MonitorHandle.ixx
        Position.ixx
        Size.ixx
        VideoMode.ixx
        vulkan_instance_extensions.ixx
        vulkan_queue_family_supports_presenting.ixx
        Window.ixx
        window_functions.ixx
        WindowCommand.ixx
        WindowHandle.ixx
        WindowProxy.ixx
        WindowedWindowSettings.ixx
        WindowSettings.ixx
)
