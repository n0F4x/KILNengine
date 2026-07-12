target_sources(${PROJECT_NAME} PRIVATE
        TaskGroup.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        exec.cppm
        Access.ixx
        accessor_c.ixx
        AccessPattern.ixx
        Ref.ixx
        Task.ixx
        TaskGroup.ixx
)
