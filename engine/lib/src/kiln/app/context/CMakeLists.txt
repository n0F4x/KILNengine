target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        context.cppm
        Context.ixx
        context_variable_c.ixx
        context_variable_injection_c.ixx
        decays_to_context_variable_c.ixx
        decays_to_context_variable_injection_c.ixx
)
