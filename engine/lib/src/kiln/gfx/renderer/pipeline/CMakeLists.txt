target_sources(${PROJECT_NAME} PRIVATE
        GraphicsPipeline.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        pipeline.cppm
        GraphicsPipeline.ixx
        PipelinePlugin.ixx
        PipelinePluginInjection.ixx
        RenderPass.ixx
        ShaderModule.ixx
)
