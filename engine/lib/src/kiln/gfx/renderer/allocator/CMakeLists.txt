target_sources(${PROJECT_NAME} PRIVATE
        Allocator.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        allocator.cppm
        Allocator.ixx
        AllocatorPlugin.ixx
        AllocatorPluginInjection.ixx
)
