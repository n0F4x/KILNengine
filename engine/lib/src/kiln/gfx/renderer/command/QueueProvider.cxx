module;

#include <optional>
#include <utility>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.command.QueueProvider;

import kiln.gfx.renderer.command.QueueProviderBuilder;
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
    [[maybe_unused]] const std::array<std::optional<Queue>, 3>& queues,
    const QueueIndices&                                         queue_indices
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

auto describe_build(app::EntryBuildDirector<QueueProvider>& build_director) -> void
{
    build_director.use_builder<QueueProviderBuilder>();
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

}   // namespace kiln::gfx::renderer
