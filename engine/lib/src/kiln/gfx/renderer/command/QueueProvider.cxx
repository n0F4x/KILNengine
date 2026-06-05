module;

#include <optional>
#include <utility>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.command.QueueProvider;

import kiln.gfx.vulkan.QueueInfo;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.containers.OptionalRef;
import kiln.util.contracts;

namespace kiln::gfx::renderer {

// ReSharper disable once CppNotAllPathsReturnValue
auto QueueIndices::at(const QueueType type) noexcept -> std::optional<uint32_t>&
{
    switch (type)
    {
        using enum QueueType;
        case eGraphics:             return graphics_queue_index;
        case eCompute:              return compute_queue_index;
        case eHostToDeviceTransfer: return host_to_device_transfer_queue_index;
    }
}

// ReSharper disable once CppNotAllPathsReturnValue
auto QueueIndices::at(const QueueType type) const noexcept
    -> const std::optional<uint32_t>&
{
    switch (type)
    {
        using enum QueueType;
        case eGraphics:             return graphics_queue_index;
        case eCompute:              return compute_queue_index;
        case eHostToDeviceTransfer: return host_to_device_transfer_queue_index;
    }
}

QueueProviderPrecondition::QueueProviderPrecondition(
    const std::array<std::optional<Queue>, 3>& queues,
    const QueueIndices&                        queue_indices
)
{
    if (queue_indices.compute_queue_index.has_value())
    {
        PRECOND(queues[*queue_indices.compute_queue_index].has_value());
        PRECOND(
            queues[*queue_indices.compute_queue_index]->flags()
            & vk::QueueFlagBits::eCompute
        );
    }
    if (queue_indices.graphics_queue_index.has_value())
    {
        PRECOND(queues[*queue_indices.graphics_queue_index].has_value());
        PRECOND(
            queues[*queue_indices.graphics_queue_index]->flags()
            & vk::QueueFlagBits::eGraphics
        );
    }
    if (queue_indices.host_to_device_transfer_queue_index.has_value())
    {
        PRECOND(queues[*queue_indices.host_to_device_transfer_queue_index].has_value());
        PRECOND(
            queues[*queue_indices.host_to_device_transfer_queue_index]->flags()
            & vk::QueueFlagBits::eTransfer
        );
    }
}

QueueProvider::QueueProvider(
    std::array<std::optional<Queue>, 3>&& queues,
    const QueueIndices&                   queue_indices
)
    : QueueProviderPrecondition{ queues, queue_indices },
      m_queues{ std::move(queues) },
      m_queue_indices{ queue_indices }
{
}

auto QueueProvider::graphics_queue() noexcept -> std::optional<GraphicsQueueRef>
{
    return graphics_queue_as<GraphicsQueueRef>();
}

auto QueueProvider::compute_queue() noexcept -> std::optional<ComputeQueueRef>
{
    return compute_queue_as<ComputeQueueRef>();
}

auto QueueProvider::host_to_device_transfer_queue() noexcept
    -> std::optional<TransferQueueRef>
{
    return host_to_device_transfer_queue_as<TransferQueueRef>();
}

auto QueueProvider::Builder::create(
    vulkan::InstanceBuilder& instance_builder,
    DeviceBuilder&           device_builder
) -> Builder
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

    return Builder{};
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

auto QueueProvider::Builder::build(const Device& device) -> QueueProvider
{
    std::array<std::optional<Queue>, 3> queue_slots{};
    QueueIndices                        queue_indices;

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
