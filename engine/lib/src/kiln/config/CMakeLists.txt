target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        engine_name.hpp
        engine_version.hpp
)
