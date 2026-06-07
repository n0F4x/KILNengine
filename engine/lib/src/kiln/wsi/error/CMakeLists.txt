target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        error.cppm
        Error.ixx
        error_category_from.ixx
        ErrorCategory.ixx
        handle_glfw_error.ixx
)
