target_sources(${PROJECT_NAME} PRIVATE
        VulkanError.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        check_result.ixx
        make_typed_result_code_variant.ixx
        Result.ixx
        result_category.ixx
        result_description.ixx
        ResultCategory.ixx
        TypedResultCode.ixx
        VulkanError.ixx
)
