target_sources(${PROJECT_NAME} PRIVATE
        PresentationPlugin.cxx
        RenderSurface.cxx
        Swapchain.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        presentation.cppm
        PresentationPlugin.ixx
        PresentationPluginFailedError.ixx
        RenderSurface.ixx
        Swapchain.ixx
)
