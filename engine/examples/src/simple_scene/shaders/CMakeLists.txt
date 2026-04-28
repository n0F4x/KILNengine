find_program(SLANGC slangc HINTS $ENV{VULKAN_SDK}/bin REQUIRED)

set(SHADERS_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SHADERS_BUILD_DIR ${SHADERS_SRC_DIR})

set(SHADERS
        ${SHADERS_SRC_DIR}/main.slang
)

file(MAKE_DIRECTORY ${SHADERS_BUILD_DIR})

foreach (SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    set(SHADER_OUT_FILE "${SHADERS_BUILD_DIR}/${SHADER_NAME}.spv")
    list(APPEND SHADER_OUT_FILES ${SHADER_OUT_FILE})
    add_custom_command(
            MAIN_DEPENDENCY ${SHADER}
            OUTPUT ${SHADER_OUT_FILE}
            COMMAND ${SLANGC} ${SHADER}
            -target spirv
            -o ${SHADER_OUT_FILE}
            -I ${SHADERS_SRC_DIR}
            VERBATIM
    )
endforeach ()

add_custom_target(${PROJECT_NAME}_shaders DEPENDS ${SHADER_OUT_FILES})

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        shaders.cppm
)
