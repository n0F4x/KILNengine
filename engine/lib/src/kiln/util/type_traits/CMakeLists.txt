target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        type_traits.cppm
        always_true.ixx
        arguments_of.ixx
        const_like.ixx
        forward_like.ixx
        result_of.ixx
        Signature.ixx
        type_list_drop_front.ixx
        type_list_front.ixx
        type_list_to.ixx
)
