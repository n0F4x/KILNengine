target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        event.cppm
        Key.ixx
        poll_events.ixx
        wait_events.ixx
)
