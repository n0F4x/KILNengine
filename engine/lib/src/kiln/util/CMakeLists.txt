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
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        Any.ixx
        concepts.ixx
        contracts.ixx
        Deleter.ixx
        Dummy.ixx
        for_each.ixx
        Function.ixx
        GenericStack.ixx
        OptionalRef.ixx
        reflection.ixx
        StringLiteral.ixx
        transform.ixx
        type_traits.ixx
        TypeList.ixx
)
