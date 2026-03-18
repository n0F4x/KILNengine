add_subdirectory(concepts)
add_subdirectory(type_traits)

target_sources(${PROJECT_NAME} PRIVATE
        contracts.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        contract_macros.hpp
        lifetimebound.hpp
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        util.cppm
        Any.ixx
        contracts.ixx
        Deleter.ixx
        Dummy.ixx
        for_each.ixx
        Function.ixx
        GenericStack.ixx
        OptionalRef.ixx
        reflection.ixx
        ScopeFail.ixx
        StringLiteral.ixx
        Strong.ixx
        transform.ixx
        TypeList.ixx
)
