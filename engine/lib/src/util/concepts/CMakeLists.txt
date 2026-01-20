target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        concepts.cppm
        allocator.ixx
        lvalue_reference.ixx
        naked.ixx
        nothrow_assignable_from.ixx
        nothrow_constructible_from.ixx
        nothrow_convertible_to.ixx
        nothrow_movable.ixx
        nothrow_move_constructible.ixx
        preserves_const.ixx
        specialization_of.ixx
        storable.ixx
        strips_to.ixx
)
