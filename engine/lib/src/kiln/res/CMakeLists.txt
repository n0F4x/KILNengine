target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        res.cppm
        resource_c.ixx
        resource_id_from_string.ixx
        ResourceID.ixx
        ResourceManager.ixx
        ResourceManagerBuilder.ixx
        ResourcePool.ixx
        SharedResourceHandle.ixx
        WeakResourceHandle.ixx
)
