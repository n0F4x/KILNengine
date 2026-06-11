module kiln.gfx.renderer.memory.AllocatorBuilder;

import vulkan_hpp;

import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.InstanceBuilder;

namespace kiln::gfx::renderer {

auto AllocatorBuilder::create(
    vulkan::InstanceBuilder& instance_builder,
    DeviceBuilder&           device_builder
) -> AllocatorBuilder
{
    // VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
    // VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT
    // VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT
    // VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT
    instance_builder.enable_extension_if_available(
        vk::KHRGetPhysicalDeviceProperties2ExtensionName
    );

    // VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT
    instance_builder.enable_extension_if_available(
        vk::KHRDeviceGroupCreationExtensionName
    );


    // VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT
    device_builder.enable_extension_if_available(
        vk::KHRGetMemoryRequirements2ExtensionName
    );
    device_builder.enable_extension_if_available(vk::KHRDedicatedAllocationExtensionName);

    // VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT
    device_builder.enable_extension_if_available(vk::KHRBindMemory2ExtensionName);

    // VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
    device_builder.enable_extension_if_available(vk::EXTMemoryBudgetExtensionName);

    // VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT
    device_builder.enable_extension_if_available(vk::AMDDeviceCoherentMemoryExtensionName);
    device_builder.enable_features_if_available(
        vk::PhysicalDeviceCoherentMemoryFeaturesAMD{ .deviceCoherentMemory = vk::True }
    );


    return AllocatorBuilder{};
}

auto AllocatorBuilder::build(const vulkan::Instance& instance, const Device& device)
    -> Allocator
{
    return Allocator{ instance, device };
}

}   // namespace kiln::gfx::renderer
