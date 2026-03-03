module;

#include <algorithm>
#include <memory>

#include <vk_mem_alloc.h>

module kiln.gfx.renderer.memory.Allocator;

import vulkan_hpp;

import kiln.gfx.renderer.memory.MemoryTypeID;
import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.StringLiteral;

namespace kiln::gfx::renderer {

[[nodiscard]]
auto collect_vulkan_functions(
    const vk::raii::Instance& instance,
    const vk::raii::Device&   device
) -> VmaVulkanFunctions
{
    return VmaVulkanFunctions{
        .vkGetInstanceProcAddr = instance.getDispatcher()->vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr   = device.getDispatcher()->vkGetDeviceProcAddr,
        .vkGetPhysicalDeviceProperties =
            instance.getDispatcher()->vkGetPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties =
            instance.getDispatcher()->vkGetPhysicalDeviceMemoryProperties,
        .vkAllocateMemory          = device.getDispatcher()->vkAllocateMemory,
        .vkFreeMemory              = device.getDispatcher()->vkFreeMemory,
        .vkMapMemory               = device.getDispatcher()->vkMapMemory,
        .vkUnmapMemory             = device.getDispatcher()->vkUnmapMemory,
        .vkFlushMappedMemoryRanges = device.getDispatcher()->vkFlushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges =
            device.getDispatcher()->vkInvalidateMappedMemoryRanges,
        .vkBindBufferMemory = device.getDispatcher()->vkBindBufferMemory,
        .vkBindImageMemory  = device.getDispatcher()->vkBindImageMemory,
        .vkGetBufferMemoryRequirements =
            device.getDispatcher()->vkGetBufferMemoryRequirements,
        .vkGetImageMemoryRequirements =
            device.getDispatcher()->vkGetImageMemoryRequirements,
        .vkCreateBuffer  = device.getDispatcher()->vkCreateBuffer,
        .vkDestroyBuffer = device.getDispatcher()->vkDestroyBuffer,
        .vkCreateImage   = device.getDispatcher()->vkCreateImage,
        .vkDestroyImage  = device.getDispatcher()->vkDestroyImage,
        .vkCmdCopyBuffer = device.getDispatcher()->vkCmdCopyBuffer,
        .vkGetBufferMemoryRequirements2KHR =
            device.getDispatcher()->vkGetBufferMemoryRequirements2,
        .vkGetImageMemoryRequirements2KHR =
            device.getDispatcher()->vkGetImageMemoryRequirements2,
        .vkBindBufferMemory2KHR = device.getDispatcher()->vkBindBufferMemory2,
        .vkBindImageMemory2KHR  = device.getDispatcher()->vkBindImageMemory2,
        .vkGetPhysicalDeviceMemoryProperties2KHR =
            instance.getDispatcher()->vkGetPhysicalDeviceMemoryProperties2,
        .vkGetDeviceBufferMemoryRequirements =
            device.getDispatcher()->vkGetDeviceBufferMemoryRequirements,
        .vkGetDeviceImageMemoryRequirements =
            device.getDispatcher()->vkGetDeviceImageMemoryRequirements,
    };
}

[[nodiscard]]
auto vma_allocator_create_flags(
    const vulkan::PhysicalDeviceCapabilities& device_capabilities
) noexcept -> VmaAllocatorCreateFlags
{
    VmaAllocatorCreateFlags flags{};

    if (std::ranges::contains(
            device_capabilities.extensions(),
            util::StringLiteral{ vk::KHRDedicatedAllocationExtensionName }
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    }

    if (std::ranges::contains(
            device_capabilities.extensions(),
            util::StringLiteral{ vk::KHRBindMemory2ExtensionName }
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;
    }

    if (std::ranges::contains(
            device_capabilities.extensions(),
            util::StringLiteral{ vk::EXTMemoryBudgetExtensionName }
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    }

    constexpr static vk::PhysicalDeviceCoherentMemoryFeaturesAMD
        coherent_memory_features_amd{
            .deviceCoherentMemory = vk::True,
        };
    if (std::ranges::contains(
            device_capabilities.extensions(),
            util::StringLiteral{ vk::AMDDeviceCoherentMemoryExtensionName }
        )
        && device_capabilities.contains_features(coherent_memory_features_amd))
    {
        flags |= VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT;
    }

    constexpr static vk::PhysicalDeviceBufferDeviceAddressFeatures
        buffer_device_address_features{
            .bufferDeviceAddress = vk::True,
        };
    if (device_capabilities.contains_features(buffer_device_address_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    }

    constexpr static vk::PhysicalDeviceMemoryPriorityFeaturesEXT memory_priority_features{
        .memoryPriority = vk::True,
    };
    if (std::ranges::contains(
            device_capabilities.extensions(),
            util::StringLiteral{ vk::EXTMemoryPriorityExtensionName }
        )
        && device_capabilities.contains_features(memory_priority_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
    }

    constexpr static vk::PhysicalDeviceMaintenance4Features maintenance4_features{
        .maintenance4 = vk::True,
    };
    if (std::ranges::contains(
            device_capabilities.extensions(),
            util::StringLiteral{ vk::KHRMaintenance4ExtensionName }
        )
        && device_capabilities.contains_features(maintenance4_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT;
    }

    constexpr static vk::PhysicalDeviceMaintenance5FeaturesKHR maintenance5_features{
        .maintenance5 = vk::True,
    };
    if (std::ranges::contains(
            device_capabilities.extensions(),
            util::StringLiteral{ vk::KHRMaintenance5ExtensionName }
        )
        && device_capabilities.contains_features(maintenance5_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT;
    }

    return flags;
}

[[nodiscard]]
auto create_allocator(const vk::raii::Instance& instance, const Device& device)
    -> std::unique_ptr<VmaAllocator_T, decltype(&vmaDestroyAllocator)>
{
    const VmaVulkanFunctions vulkan_functions{
        collect_vulkan_functions(instance, device.logical_device())
    };
    const VmaAllocatorCreateInfo create_info{
        .flags            = vma_allocator_create_flags(device.capabilities()),
        .physicalDevice   = *device.physical_device(),
        .device           = *device.logical_device(),
        .pVulkanFunctions = &vulkan_functions,
        .instance         = *instance,
    };

    VmaAllocator allocator{};
    vulkan::check_result(::vmaCreateAllocator(&create_info, &allocator));

    return std::unique_ptr<VmaAllocator_T, decltype(&::vmaDestroyAllocator)>{
        allocator, ::vmaDestroyAllocator
    };
}

Allocator::Allocator(const vk::raii::Instance& instance, const Device& device)
    : m_device{ device },
      m_handle{ create_allocator(instance, device) }
{
}

auto Allocator::create_buffer(
    const vk::BufferCreateInfo&    buffer_create_info,
    const VmaAllocationCreateInfo& allocation_create_info
) const -> std::tuple<Buffer, Allocation, VmaAllocationInfo>
{
    vk::Buffer        buffer;
    VmaAllocation     allocation{};
    VmaAllocationInfo allocation_info{};
    auto              result = ::vmaCreateBuffer(
        m_handle.get(),
        reinterpret_cast<const VkBufferCreateInfo*>(&buffer_create_info),
        &allocation_create_info,
        reinterpret_cast<VkBuffer*>(&buffer),
        &allocation,
        &allocation_info
    );

    vulkan::check_result(result);

    return std::make_tuple(
        Buffer{
            vk::raii::Buffer{ m_device.get().logical_device(), buffer },
            buffer_create_info.size,
    },
        Allocation{
            m_handle.get(),
            allocation,
            MemoryTypeID{ allocation_info.memoryType },
            allocation_info.size,
        },
        allocation_info
    );
}

}   // namespace kiln::gfx::renderer
