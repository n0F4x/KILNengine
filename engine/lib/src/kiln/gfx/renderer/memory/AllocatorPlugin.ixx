export module kiln.gfx.renderer.memory.AllocatorPlugin;

import vulkan_hpp;

import kiln.app.plugin.PluginInterface;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.vulkan.InstancePlugin;

namespace kiln::gfx::renderer {

export class AllocatorPlugin : public app::PluginInterface {
public:
    [[nodiscard]]
    static auto create_plugin(
        vulkan::InstancePlugin& instance_plugin,
        DevicePlugin&           device_plugin
    ) -> AllocatorPlugin;

    [[nodiscard]]
    static auto operator()(const vk::raii::Instance& instance, const Device& device)
        -> Allocator
    {
        return Allocator{ instance, device };
    }
};

}   // namespace kiln::gfx::renderer

module :private;

namespace kiln::gfx::renderer {

auto AllocatorPlugin::create_plugin(
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
