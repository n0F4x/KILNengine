add_subdirectory(App)
add_subdirectory(config)
add_subdirectory(util)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        app.ixx
        config.ixx
        util.ixx
)
