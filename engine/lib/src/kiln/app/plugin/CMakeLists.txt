target_sources(${PROJECT_NAME} PRIVATE
        PluginInterface.cxx
        PluginStack.cxx
        PluginTree.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        plugin.cppm
        ErasedPlugin.ixx
        hash_plugin.ixx
        meta_plugin_c.ixx
        meta_plugin_injection_c.ixx
        plugin_c.ixx
        plugin_injection_c.ixx
        PluginInterface.ixx
        PluginStack.ixx
        PluginTree.ixx
        strip_plugin_dependency_t.ixx
)
