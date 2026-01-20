add_subdirectory(concepts)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        contract_macros.hpp
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        util.cppm
        Any.ixx
        Deallocator.ixx
        contracts.ixx
        reflection.ixx
        type_traits.ixx
)
