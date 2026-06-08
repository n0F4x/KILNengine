target_sources(${PROJECT_NAME} PRIVATE
        ContextBuilderInterface.cxx
        ContextBuildTree.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        context.cppm
        context_builder_c.ixx
        context_c.ixx
        ContextBase.ixx
        ContextBuilderFailedError.ixx
        ContextBuilderInterface.ixx
        ContextBuildTree.ixx
        Contexts.ixx
        ErasedContext.ixx
        ErasedContextBuilder.ixx
        strip_dependency_t.ixx
)
