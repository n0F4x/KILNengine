target_sources(${PROJECT_NAME} PRIVATE
        StagingStream.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        stream.cppm
        StagingRegion.ixx
        StagingStream.ixx
)
