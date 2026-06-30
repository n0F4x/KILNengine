target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        algorithms.cppm
        any_of.ixx
        for_each.ixx
        hash.ixx
        transform.ixx
)
