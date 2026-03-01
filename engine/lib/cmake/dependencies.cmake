if (DEFINED CMAKE_TOOLCHAIN_FILE)
    # This message is here to mitigate the "CMAKE_TOOLCHAIN_FILE not used warning"
    message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif ()


# fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

# spdlog
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)

# GLFW
find_package(glfw3 CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLFW_INCLUDE_VULKAN
)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

# Vulkan
# The Vulkan module should really be provided and not hacked together
# This also brakes conan export
# See https://github.com/KhronosGroup/Vulkan-Hpp/issues/2473,
#  and https://github.com/conan-io/conan-center-index/issues/29558
find_package(VulkanHeaders CONFIG REQUIRED)
get_target_property(VulkanHeaders_INCLUDE_DIRS Vulkan::Headers INTERFACE_INCLUDE_DIRECTORIES)
add_library(VulkanCppModule)
target_compile_features(VulkanCppModule PUBLIC cxx_std_20)
target_sources(VulkanCppModule PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${VulkanHeaders_INCLUDE_DIRS}
        FILES ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan.cppm
)
target_compile_definitions(VulkanCppModule PUBLIC
        VK_NO_PROTOTYPES
        VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
        VULKAN_HPP_NO_EXCEPTIONS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
)
target_link_libraries(VulkanCppModule PUBLIC Vulkan::Headers)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Headers)
target_link_libraries(${PROJECT_NAME} PUBLIC $<BUILD_LOCAL_INTERFACE:VulkanCppModule>)

# VulkanMemoryAllocator
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PUBLIC
        VMA_STATIC_VULKAN_FUNCTIONS=0
        VMA_DYNAMIC_VULKAN_FUNCTIONS=0
)
target_link_libraries(${PROJECT_NAME} PUBLIC GPUOpen::VulkanMemoryAllocator)
