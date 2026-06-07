add_subdirectory(gltf)

target_sources(${PROJECT_NAME} PRIVATE
        Buffer.cxx
        Bundle.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        asset.cppm
        Buffer.ixx
        Bundle.ixx
        IndexArrayView.ixx
        VertexArrayView.ixx
)
