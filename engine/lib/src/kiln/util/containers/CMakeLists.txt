target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        containers.cppm
        Any.ixx
        Bool.ixx
        CopyableAny.ixx
        CopyableFunction.ixx
        FlatMap.ixx
        Function.ixx
        GenericStack.ixx
        MoveOnlyAny.ixx
        MoveOnlyFunction.ixx
        OptionalRef.ixx
)
