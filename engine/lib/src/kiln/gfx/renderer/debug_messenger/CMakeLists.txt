target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        debug_messenger.cppm
        DebugMessenger.ixx
        DebugMessengerPlugin.ixx
        DebugMessengerPluginInjection.ixx
)
