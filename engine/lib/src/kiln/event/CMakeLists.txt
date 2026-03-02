target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        event.cppm
        event_c.ixx
        EventSystem.ixx
        Plugin.ixx
        SubscriberID.ixx
)
