include(cmake/target_shader_sources.cmake)

add_custom_target(${PROJECT_NAME}_shaders)

add_subdirectory(kernels)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        shaders.cppm
)
