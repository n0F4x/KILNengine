target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        decays_to_resource_c.ixx
        decays_to_resource_injection_c.ixx
        resource_c.ixx
        resource_injection_c.ixx
        ResourceStack.ixx
        ResourceInjectionStack.ixx
)
