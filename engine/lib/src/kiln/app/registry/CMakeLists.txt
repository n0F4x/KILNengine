target_sources(${PROJECT_NAME} PRIVATE
        DependencyChainNode.cxx
        EntryBuilderContainer.cxx
        EntryInjectionContainer.cxx
        RegistryBuilder.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        registry.cppm
        BuildableEntry.ixx
        BuildableEntryBase.ixx
        BuildableEntryBuilder.ixx
        configuration_entry_c.ixx
        ConfigurationEntry.ixx
        CyclicDependencyDetected.ixx
        DependencyChainNode.ixx
        entry_c.ixx
        entry_builder_c.ixx
        EntryBase.ixx
        EntryBuildDirector.ixx
        EntryBuilderBase.ixx
        EntryBuilderContainer.ixx
        EntryBuilderFailedError.ixx
        EntryInjectionContainer.ixx
        ErasedEntry.ixx
        Registry.ixx
        RegistryBuilder.ixx
        represents_entry_dependency_c.ixx
        RepresentsEntryDependencyConcept.ixx
        strip_dependency_t.ixx
)
