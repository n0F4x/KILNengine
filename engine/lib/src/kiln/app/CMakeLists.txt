add_subdirectory(config)
add_subdirectory(context)
add_subdirectory(memory)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        app.cppm
        App.ixx
        Builder.ixx
        create.ixx
)
