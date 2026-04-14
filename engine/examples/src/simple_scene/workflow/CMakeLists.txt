target_sources(${PROJECT_NAME} PRIVATE
        Asset.cxx
)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        workflow.cppm
        Asset.ixx
)
