add_subdirectory(concepts)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        always_true.hpp
        arguments_of.hpp
        const_like.hpp
        forward_like.hpp
        is_noexcept.hpp
        result_of.hpp
        Signature.hpp
        type_list_drop_front.hpp
        type_list_front.hpp
)
