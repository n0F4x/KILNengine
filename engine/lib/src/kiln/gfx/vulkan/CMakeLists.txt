add_subdirectory(extension_inspection)
add_subdirectory(format)
add_subdirectory(result)
add_subdirectory(structure_chain)

target_sources(${PROJECT_NAME} PRIVATE
        Bundle.cxx
        DebugMessengerPlugin.cxx
        DeviceBuilder.cxx
        global_definitions.cpp
        InstanceBuilder.cxx
        InstancePlugin.cxx
        PhysicalDeviceFilter.cxx
        vma_global_definitions.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        vulkan.cppm
        Bundle.ixx
        context.ixx
        DebugMessengerPlugin.ixx
        DebugMessengerPluginInjection.ixx
        Device.ixx
        DeviceBuilder.ixx
        ErasedQueueRequest.ixx
        InstanceBuilder.ixx
        InstancePlugin.ixx
        InstancePluginInjection.ixx
        PhysicalDeviceCapabilities.ixx
        PhysicalDeviceFilter.ixx
        Profile.ixx
        QueueFamilyIndex.ixx
        QueueFamilyInfo.ixx
        QueueRequestInterface.ixx
)
