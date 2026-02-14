add_subdirectory(event)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        Context.ixx
        Error.ixx
        event.ixx
        FullScreenWindowSettings.ixx
        monitor.ixx
        Plugin.ixx
        Size.ixx
        Window.ixx
        WindowedWindowSettings.ixx
        WindowSettings.ixx
)
