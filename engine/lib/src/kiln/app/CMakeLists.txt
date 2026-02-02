add_subdirectory(plugin)
add_subdirectory(resource)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        App.ixx
        Builder.ixx
        create.ixx
        plugin.ixx
        resource.ixx
)
