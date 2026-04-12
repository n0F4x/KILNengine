module;

#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

module kiln.gfx.renderer.command.QueueProvider;

import kiln.gfx.vulkan.QueueInfo;
import kiln.gfx.vulkan.result.check_result;

namespace kiln::gfx::renderer {

QueueProvider::QueueProvider(Queues&& queues) : m_queues{ std::move(queues) } {}

auto QueueProvider::graphics_queue() -> std::optional<GraphicsQueueRef>
{
    return m_queues.graphics_queue_pack.transform(
        [](const QueuePack& queue_pack) -> GraphicsQueueRef
        {
            return GraphicsQueueRef{
                queue_pack.family_index,
                queue_pack.queue,
            };
        }
    );
}

auto QueueProvider::host_to_device_transfer_queue() -> std::optional<TransferQueueRef>
{
    return m_queues.host_to_device_transfer_queue_pack.transform(
        [](const QueuePack& queue_pack) -> TransferQueueRef
        {
            return TransferQueueRef{
                queue_pack.family_index,
                queue_pack.queue,
            };
        }
    );
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto QueueProvider::available_queues() -> std::vector<std::reference_wrapper<QueuePack>>
{
    // TODO: join optionals when they have become ranges
    std::vector<std::reference_wrapper<QueuePack>> result;

    uint32_t queue_count{};
    if (m_queues.graphics_queue_pack.has_value())
    {
        ++queue_count;
    }
    if (m_queues.host_to_device_transfer_queue_pack.has_value())
    {
        ++queue_count;
    }
    result.reserve(queue_count);

    if (m_queues.graphics_queue_pack.has_value())
    {
        result.push_back(*m_queues.graphics_queue_pack);
    }
    if (m_queues.host_to_device_transfer_queue_pack.has_value())
    {
        result.push_back(*m_queues.host_to_device_transfer_queue_pack);
    }

    return result;
}

auto QueueProvider::Builder::build(const Device& device) -> QueueProvider
{
    const auto queue_pack_from{
        [&device](const QueueType type) -> auto
        {
            return [&device, type](const vulkan::QueueInfo& info) -> QueuePack
            {
                return QueuePack{
                    .type         = type,
                    .family_index = info.family_index,
                    .flags        = info.flags,
                    .index        = info.index,
                    .queue = vulkan::check_result(device.logical_device().getQueue2(
                        vk::DeviceQueueInfo2{
                            .queueFamilyIndex = info.family_index.underlying(),
                            .queueIndex       = info.index,
                        }
                    )),
                };
            };
        }
    };

    Queues queues{
        .graphics_queue_pack =
            device.graphics_queue_info().transform(queue_pack_from(QueueType::eGraphics)),
        .host_to_device_transfer_queue_pack =
            device.host_to_device_transfer_queue_info().transform(
                queue_pack_from(QueueType::eHostToDeviceTransfer)
            ),
    };

    return QueueProvider{ std::move(queues) };
}

}   // namespace kiln::gfx::renderer
