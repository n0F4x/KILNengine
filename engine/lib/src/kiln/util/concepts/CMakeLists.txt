target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        allocator.hpp
        decayed.hpp
        function.hpp
        function_pointer.hpp
        function_reference.hpp
        lvalue_reference.hpp
        member_function_pointer.hpp
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
        type_list.hpp
        type_list_all_of.hpp
        unambiguous_explicit_functor.hpp
        unambiguous_functor.hpp
        unambiguous_implicit_functor.hpp
        unambiguous_static_functor.hpp
        unambiguously_invocable.hpp
)
