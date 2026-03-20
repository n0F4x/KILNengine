target_sources(${PROJECT_NAME} PRIVATE
        GraphicsPipeline.cxx
        RenderPass.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        pipeline.cppm
        ColorAttachment.ixx
        DepthAttachment.ixx
        GraphicsPipeline.ixx
        PipelinePlugin.ixx
        PipelinePluginInjection.ixx
        RenderPass.ixx
        ShaderModule.ixx
)
