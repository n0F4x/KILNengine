add_subdirectory(extension_inspection)
add_subdirectory(format)
add_subdirectory(result)
add_subdirectory(structure_chain)

target_sources(${PROJECT_NAME} PRIVATE
        default_debug_messenger_callback.cxx
        global_definitions.cpp
        InstanceBuilder.cxx
        vma_global_definitions.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        vulkan.cppm
        context.ixx
        InstanceBuilder.ixx
        InstancePlugin.ixx
        InstancePluginInjection.ixx
        default_debug_messenger_callback.ixx
        Device.ixx
        DeviceBuilder.ixx
        PhysicalDeviceCapabilities.ixx
        PhysicalDeviceSelector.ixx
        Profile.ixx
        queue_properties.ixx
        QueueFamilyIndex.ixx
        QueueGroup.ixx
        QueuePack.ixx
        Swapchain.ixx
)
