find_program(SLANGC slangc HINTS $ENV{VULKAN_SDK}/bin REQUIRED)

function(target_shader_sources TARGET)
    cmake_parse_arguments(ARG "" "" "" ${ARGN})

    set(SLANG_MODULES_ROOT_DIR "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../modules")
    set(SLANG_INCLUDE_FLAGS "-I" "${SLANG_MODULES_ROOT_DIR}")
    set(SLANG_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    set(SLANG_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

    foreach (SHADER_SRC ${ARG_UNPARSED_ARGUMENTS})
        get_filename_component(SHADER_ABS "${SHADER_SRC}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        get_filename_component(SHADER_NAME ${SHADER_ABS} NAME_WE)

        set(SPV_OUT "${SLANG_BINARY_DIR}/${SHADER_NAME}.spv")
        set(DEP_FILE "${SLANG_BUILD_DIR}/${SHADER_NAME}.spv.d")

        add_custom_command(
                OUTPUT "${SPV_OUT}"
                MAIN_DEPENDENCY "${SHADER_ABS}"
                DEPFILE "${DEP_FILE}"
                COMMAND ${SLANGC} "${SHADER_ABS}"
                -target spirv
                -profile spirv_1_6
                -fvk-use-scalar-layout
                -matrix-layout-column-major
                -depfile "${DEP_FILE}"
                -o "${SPV_OUT}"
                ${SLANG_INCLUDE_FLAGS}
                COMMENT "Compiling Slang Target: ${SHADER_NAME}.spv"
                VERBATIM
        )

        string(MAKE_C_IDENTIFIER "${TARGET}_${SHADER_NAME}_spv_target" TARGET_DUMMY_NAME)
        add_custom_target(${TARGET_DUMMY_NAME} DEPENDS "${SPV_OUT}")

        add_dependencies(${TARGET} ${TARGET_DUMMY_NAME})
    endforeach ()
endfunction()
