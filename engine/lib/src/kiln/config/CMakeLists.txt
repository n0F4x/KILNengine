target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        config.cppm
        engine_name.ixx
        engine_version.ixx
        Version.ixx
)
