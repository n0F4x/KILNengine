target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        allocator.hpp
        lvalue_reference.hpp
        naked.hpp
        nothrow_assignable_from.hpp
        nothrow_constructible_from.hpp
        nothrow_convertible_to.hpp
        nothrow_movable.hpp
        nothrow_move_constructible.hpp
        preserves_const.hpp
        specialization_of.hpp
        storable.hpp
        strips_to.hpp
)
