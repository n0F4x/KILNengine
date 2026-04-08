target_sources(${PROJECT_NAME} PRIVATE
        Asset.cxx
        Bundle.cxx
        Parser.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        gltf.cppm
        Asset.ixx
        Bundle.ixx
        Parser.ixx
)
