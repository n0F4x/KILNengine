target_sources(${PROJECT_NAME} PRIVATE
        ModelBundle.cxx
        GltfAsset.cxx
        GltfLoader.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        model.cppm
        ModelBundle.ixx
        GltfAsset.ixx
        GltfLoader.ixx
)
