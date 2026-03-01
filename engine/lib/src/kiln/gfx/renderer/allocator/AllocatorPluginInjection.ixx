export module kiln.gfx.renderer.allocator.AllocatorPluginInjection;

import vulkan_hpp;

import kiln.gfx.renderer.allocator.AllocatorPlugin;
import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.vulkan.InstancePlugin;

namespace kiln::gfx::renderer {

export class AllocatorPluginInjection {
public:
    [[nodiscard]]
    static auto
        operator()(vulkan::InstancePlugin& instance_plugin, DevicePlugin& device_plugin)
            -> AllocatorPlugin;
};

}   // namespace kiln::gfx::renderer

module :private;

namespace kiln::gfx::renderer {

auto AllocatorPluginInjection::operator()(
    vulkan::InstancePlugin& instance_plugin,
    DevicePlugin&           device_plugin
) -> AllocatorPlugin
{
    // VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
    // VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT
    // VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT
    // VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT
    instance_plugin->enable_extension_if_available(
        vk::KHRGetPhysicalDeviceProperties2ExtensionName
    );

    // VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT
    instance_plugin->enable_extension_if_available(
        vk::KHRDeviceGroupCreationExtensionName
    );


    // VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT
    device_plugin->enable_extension_if_available(
        vk::KHRGetMemoryRequirements2ExtensionName
    );
    device_plugin->enable_extension_if_available(vk::KHRDedicatedAllocationExtensionName);

    // VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT
    device_plugin->enable_extension_if_available(vk::KHRBindMemory2ExtensionName);

    // VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
    device_plugin->enable_extension_if_available(vk::EXTMemoryBudgetExtensionName);

    // VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT
    device_plugin->enable_extension_if_available(vk::AMDDeviceCoherentMemoryExtensionName);
    device_plugin->enable_features_if_available(
        vk::PhysicalDeviceCoherentMemoryFeaturesAMD{ .deviceCoherentMemory = vk::True }
    );


    return AllocatorPlugin{};
}

}   // namespace kiln::gfx::renderer
