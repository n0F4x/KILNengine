module;

#include <algorithm>
#include <vector>

module kiln.gfx.vulkan.PhysicalDeviceFilter;

namespace kiln::gfx::vulkan {

auto PhysicalDeviceFilter::required_capabilities() const noexcept
    -> const PhysicalDeviceCapabilities&
{
    return m_required_capabilities;
}

auto PhysicalDeviceFilter::required_queue_flags() const noexcept -> vk::QueueFlags
{
    return m_queue_flags;
}

auto PhysicalDeviceFilter::is_adequate(
    const vk::raii::PhysicalDevice& physical_device
) const -> bool
{
    return m_required_capabilities.supported_by(physical_device)
        && supports_queues_flags(physical_device)
        && supports_custom_requirements(physical_device);
}

auto PhysicalDeviceFilter::supports_queues_flags(
    const vk::raii::PhysicalDevice& physical_device
) const -> bool
{
    const std::vector<vk::QueueFamilyProperties2> queue_family_properties{
        physical_device.getQueueFamilyProperties2()
    };

    vk::QueueFlags remaining_flags{ m_queue_flags };

    for (const vk::QueueFamilyProperties2& queue_family : queue_family_properties)
    {
        remaining_flags &=
            ~(remaining_flags & queue_family.queueFamilyProperties.queueFlags);

        if (!remaining_flags)
        {
            return true;
        }
    }

    return false;
}

auto PhysicalDeviceFilter::supports_custom_requirements(
    const vk::raii::PhysicalDevice& physical_device
) const -> bool
{
    return std::ranges::all_of(
        m_custom_requirements,
        [&physical_device](const CustomRequirement& requirement) -> bool
        {
            return requirement(physical_device);   //
        }
    );
}

}   // namespace kiln::gfx::vulkan
