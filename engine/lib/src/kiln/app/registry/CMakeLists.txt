target_sources(${PROJECT_NAME} PRIVATE
        EntryBuilderInterface.cxx
        RegistryBuilder.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        registry.cppm
        entry_builder_c.ixx
        entry_c.ixx
        EntryBase.ixx
        EntryBuilderFailedError.ixx
        EntryBuilderInterface.ixx
        ErasedEntry.ixx
        ErasedEntryBuilder.ixx
        Registry.ixx
        RegistryBuilder.ixx
        strip_dependency_t.ixx
)
