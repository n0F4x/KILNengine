module;

#include <array>
#include <cstdint>
#include <optional>

module kiln.gfx.renderer.command.QueueProviderBuilder;

import vulkan_hpp;

import kiln.gfx.renderer.command.Queue;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.vulkan.QueueInfo;
import kiln.util.containers.OptionalRef;

namespace kiln::gfx::renderer {

auto QueueProviderBuilder::create(
    vulkan::InstanceBuilder& instance_builder,
    DeviceBuilder&           device_builder
) -> QueueProviderBuilder
{
    /*
     * Vulkan 1.4 requires that graphics and command queues also support transfer
     * operations
     */
    instance_builder.target_api_version(vk::ApiVersion14);
    device_builder.require_minimum_version(vk::ApiVersion14);

    device_builder.enable_features(
        vk::PhysicalDeviceVulkan13Features{ .synchronization2 = vk::True }
    );
    device_builder.enable_features(
        vk::PhysicalDeviceVulkan14Features{ .maintenance6 = vk::True }
    );

    return QueueProviderBuilder{};
}

[[nodiscard]]
auto find_queue_slot_index(
    const std::array<std::optional<Queue>, 3>& queue_slots,
    const vulkan::QueueInfo&                   queue_info
) -> std::optional<uint32_t>
{
    for (uint32_t index{}; index < queue_slots.size(); ++index)
    {
        if (!queue_slots[index].has_value())
        {
            return std::nullopt;
        }

        if (const Queue& queue{ *queue_slots[index] };
            queue.family_index() == queue_info.family_index
            && queue.index() == queue_info.index)
        {
            return index;
        }
    }

    return std::nullopt;
}

[[nodiscard]]
auto next_empty_queue_slot_index(const std::array<std::optional<Queue>, 3>& queue_slots)
    -> uint32_t
{
    for (uint32_t index{}; index < queue_slots.size(); ++index)
    {
        if (!queue_slots[index].has_value())
        {
            return index;
        }
    }
    std::unreachable();
}

auto QueueProviderBuilder::build(const Device& device) -> QueueProvider
{
    std::array<std::optional<Queue>, 3> queue_slots{};
    QueueProvider::QueueIndices         queue_indices;

    const auto assign{
        [&device, &queue_slots](
            const util::OptionalRef<const vulkan::QueueInfo> queue_info,
            std::optional<uint32_t>&                         queue_index
        ) -> void
        {
            if (queue_info.has_value())
            {
                std::optional<uint32_t> queue_slot_index{
                    find_queue_slot_index(queue_slots, *queue_info)
                };
                if (!queue_slot_index.has_value())
                {
                    queue_slot_index = next_empty_queue_slot_index(queue_slots);
                    queue_slots[*queue_slot_index].emplace(
                        device,
                        queue_info->family_index,
                        device.queue_family(queue_info->family_index).flags(),
                        queue_info->index
                    );
                }
                queue_index = *queue_slot_index;
            }
        },
    };

    assign(device.compute_queue_info(), queue_indices.compute_queue_index);
    assign(device.graphics_queue_info(), queue_indices.graphics_queue_index);
    assign(
        device.host_to_device_transfer_queue_info(),
        queue_indices.host_to_device_transfer_queue_index
    );


    return QueueProvider{ std::move(queue_slots), queue_indices };
}

}   // namespace kiln::gfx::renderer
