module;

#include <optional>

module kiln.gfx.renderer.command.QueueProvider;

import kiln.gfx.vulkan.QueueInfo;
import kiln.gfx.vulkan.result.check_result;

namespace kiln::gfx::renderer {

QueueProvider::QueueProvider(Queues&& queues) : m_queues{ std::move(queues) } {}

auto QueueProvider::graphics_queue() const -> std::optional<GraphicsQueueRef>
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

auto QueueProvider::host_to_device_transfer_queue() const
    -> std::optional<TransferQueueRef>
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

auto QueueProvider::Builder::build(const Device& device) -> QueueProvider
{
    const auto queue_pack_from{
        [&device](const vulkan::QueueInfo& info) -> QueuePack
        {
            return QueuePack{
                .family_index = info.family_index,
                .index        = info.index,
                .queue        = vulkan::check_result(device.logical_device().getQueue2(
                    vk::DeviceQueueInfo2{
                               .queueFamilyIndex = info.family_index.underlying(),
                               .queueIndex       = info.index,
                    }
                )),
            };
        }
    };

    Queues queues{
        .graphics_queue_pack = device.graphics_queue_info().transform(queue_pack_from),
        .host_to_device_transfer_queue_pack =
            device.host_to_device_transfer_queue_info().transform(queue_pack_from),
    };

    return QueueProvider{ std::move(queues) };
}

}   // namespace kiln::gfx::renderer
