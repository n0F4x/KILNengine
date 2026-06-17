add_subdirectory(concepts)
add_subdirectory(containers)
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
        any_cast.ixx
        any_of.ixx
        Bool.ixx
        contracts.ixx
        Deleter.ixx
        Dummy.ixx
        EnumMask.ixx
        for_each.ixx
        FunctionWrapper.ixx
        hash.ixx
        Lazy.ixx
        Overloaded.ixx
        reflection.ixx
        reinterpret_any_cast.ixx
        ScopeFail.ixx
        StringLiteral.ixx
        Strong.ixx
        transform.ixx
        TypeList.ixx
)
