target_sources(${PROJECT_NAME} PRIVATE
        ComputePipelineBuilder.cxx
        GraphicsPipelineBuilder.cxx
        RenderPass.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        pipeline.cppm
        ColorAttachment.ixx
        ComputePipeline.ixx
        ComputePipelineBuilder.ixx
        DepthAttachment.ixx
        GraphicsPipeline.ixx
        GraphicsPipelineBuilder.ixx
        PipelineContext.ixx
        PipelineContextBuilder.ixx
        RenderPass.ixx
        ShaderModule.ixx
)
