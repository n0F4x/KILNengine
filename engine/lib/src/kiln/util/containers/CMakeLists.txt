target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        containers.cppm
        Any.ixx
        CopyableAny.ixx
        CopyableFunction.ixx
        Function.ixx
        GenericStack.ixx
        Indirect.ixx
        MoveOnlyAny.ixx
        MoveOnlyFunction.ixx
        OptionalRef.ixx
        Polymorphic.ixx
        SmallBuffer.ixx
)
