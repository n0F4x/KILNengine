target_sources(${PROJECT_NAME} PRIVATE
        PluginTree.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        PluginTree.ixx
)
