find_program(SLANGC slangc HINTS $ENV{VULKAN_SDK}/bin REQUIRED)

set(SHADERS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SHADERS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR})

function(compile_slang_target TARGET_NAME)
    cmake_parse_arguments(ARG "" "PROFILE" "SOURCES;MODULES" ${ARGN})

    if(NOT ARG_PROFILE)
        set(ARG_PROFILE "spirv_1_5")
    endif()

    set(OUTPUT_SPV_FILES "")

    foreach(SHADER_SRC ${ARG_SOURCES})
        get_filename_component(SHADER_NAME ${SHADER_SRC} NAME_WE)

        set(SPV_OUT "${SHADERS_BUILD_DIR}/${SHADER_NAME}.spv")
        list(APPEND OUTPUT_SPV_FILES "${SPV_OUT}")

        add_custom_command(
                OUTPUT "${SPV_OUT}"
                MAIN_DEPENDENCY "${SHADER_SRC}"
                DEPENDS ${ARG_MODULES}
                COMMAND ${SLANGC} "${SHADER_SRC}"
                -target spirv
                -matrix-layout-column-major
                -profile ${ARG_PROFILE}
                -o "${SPV_OUT}"
                -I "${SHADERS_SRC_DIR}"
                COMMENT "Compiling Slang Target: ${SHADER_NAME}.spv"
                VERBATIM
        )
    endforeach()

    add_custom_target(${TARGET_NAME} DEPENDS ${OUTPUT_SPV_FILES})
endfunction()


compile_slang_target(${PROJECT_NAME}_shaders
        MODULES
        ${SHADERS_SRC_DIR}/Camera.slang
        ${SHADERS_SRC_DIR}/Index.slang
        ${SHADERS_SRC_DIR}/Material.slang
        ${SHADERS_SRC_DIR}/Primitive.slang
        ${SHADERS_SRC_DIR}/Scene.slang
        SOURCES
        ${SHADERS_SRC_DIR}/main.slang
        PROFILE
        "spirv_1_5"
)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        shaders.cppm
)
