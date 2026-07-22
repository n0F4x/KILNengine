target_sources(${PROJECT_NAME} PRIVATE
        SignalTree.cxx
        WorkTree.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        data_structures.cppm
        ReleaseWorkContract.ixx
        SignalTree.ixx
        WorkContinuation.ixx
        WorkContract.ixx
        WorkID.ixx
        WorkTree.ixx
)
