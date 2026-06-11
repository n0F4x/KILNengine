target_sources(${PROJECT_NAME} PRIVATE
        PresentationContext.cxx
        PresentationContextBuilder.cxx
        RenderSurface.cxx
        Swapchain.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        presentation.cppm
        PresentationContext.ixx
        PresentationContextBuilder.ixx
        PresentationContextBuilderFailedError.ixx
        RenderSurface.ixx
        Swapchain.ixx
)
