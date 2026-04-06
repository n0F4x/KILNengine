module;

#include <cstdint>
#include <optional>
#include <ranges>
#include <utility>
#include <vector>

module kiln.gfx.renderer.command.queue_requests;

import vulkan_hpp;

import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.wsi.vulkan_queue_family_supports_presenting;

namespace kiln::gfx::renderer {

GraphicsQueueRequest::GraphicsQueueRequest(
    const vulkan::Instance&      instance,
    const wsi::Context&          wsi_context,
    QueueRequestResultCallback&& result_callback
)
    : m_instance_ref{ instance },
      m_wsi_context_ref{ wsi_context },
      m_result_callback{ std::move(result_callback) }
{
}

auto GraphicsQueueRequest::is_suitable(
    const vk::raii::PhysicalDevice& physical_device
) const -> bool
{
    // TODO: use std::views::enumerate
    for (auto&& [family_index, family_properties] : std::views::zip(
             std::views::iota(uint32_t{}), physical_device.getQueueFamilyProperties2()
         ))
    {
        if (is_suitable(
                physical_device, vulkan::QueueFamilyIndex{ family_index }, family_properties
            ))
        {
            return true;
        }
    }

    return false;
}

[[nodiscard]]
auto full(
    const std::span<const vulkan::QueueFamilyInfo> family_infos,
    const vulkan::QueueFamilyIndex                 family_index,
    const uint32_t                                 capacity
) -> bool
{
    uint32_t count{};

    for (const vulkan::QueueFamilyInfo& family_info : family_infos)
    {
        if (family_info.index() == family_index)
        {
            count += family_info.queue_count();
        }
    }

    return count >= capacity;
}

auto GraphicsQueueRequest::prepare_queue(
    std::vector<vulkan::QueueFamilyInfo>& current_selection,
    const vk::raii::PhysicalDevice&       physical_device
) const -> void
{
    for (auto&& [family_index, family_properties] : std::views::zip(
             std::views::iota(uint32_t{}), physical_device.getQueueFamilyProperties2()
         ))
    {
        if (is_suitable(
                physical_device, vulkan::QueueFamilyIndex{ family_index }, family_properties
            )
            && !full(
                current_selection,
                vulkan::QueueFamilyIndex{ family_index },
                family_properties.queueFamilyProperties.queueCount
            ))
        {
            constexpr vk::DeviceQueueCreateFlags flags{};

            auto iter = std::ranges::find_if(
                current_selection,
                [family_index, flags](const vulkan::QueueFamilyInfo& family_info) -> bool
                {
                    return family_info.index() == vulkan::QueueFamilyIndex{ family_index }
                        && family_info.flags() == flags;
                }
            );
            if (iter == current_selection.cend())
            {
                current_selection.emplace_back(
                    vulkan::QueueFamilyIndex(family_index), flags
                );
                iter = std::prev(current_selection.end());
            }

            iter->emplace_back();
            m_result_callback(iter->index(), iter->queue_count() - 1);

            return;
        }
    }
}

auto GraphicsQueueRequest::is_suitable(
    const vk::raii::PhysicalDevice&   physical_device,
    const vulkan::QueueFamilyIndex    family_index,
    const vk::QueueFamilyProperties2& family_properties
) const -> bool
{
    // TODO: move presentation support logic elsewhere
    return family_properties.queueFamilyProperties.queueFlags
             & vk::QueueFlagBits::eGraphics
        && wsi::vulkan_queue_family_supports_presenting(
               m_wsi_context_ref,
               m_instance_ref.get().get(),
               physical_device,
               vulkan::QueueFamilyIndex{ family_index }
        );
}

}   // namespace kiln::gfx::renderer
