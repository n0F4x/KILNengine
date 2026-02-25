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
        no_unique_address.hpp
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        util.cppm
        Any.ixx
        Bool.ixx
        contracts.ixx
        Deleter.ixx
        Dummy.ixx
        FlatMap.ixx
        for_each.ixx
        Function.ixx
        FunctionWrapper.ixx
        GenericStack.ixx
        Lazy.ixx
        OptionalRef.ixx
        Overloaded.ixx
        reflection.ixx
        StringLiteral.ixx
        Strong.ixx
        transform.ixx
        TypeList.ixx
)
