add_subdirectory(concepts)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        Any.hpp
        concepts.hpp
        contract_macros.hpp
        contracts.hpp
        Deallocator.hpp
        Dummy.hpp
        for_each.hpp
        Function.hpp
        GenericStack.hpp
        OptionalRef.hpp
        reflection.hpp
        StringLiteral.hpp
        type_traits.hpp
        TypeList.hpp
)
