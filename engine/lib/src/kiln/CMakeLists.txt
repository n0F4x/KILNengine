add_subdirectory(config)
add_subdirectory(util)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        config.hpp
        util.hpp
)
