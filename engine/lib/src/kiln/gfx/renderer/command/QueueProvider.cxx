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

auto QueueProvider::graphics_queue() -> util::OptionalRef<GraphicsQueue>
{
    if (m_queues.graphics_queue_pack.has_value())
    {
        return *m_queues.graphics_queue_pack;
    }
    return std::nullopt;
}

auto QueueProvider::host_to_device_transfer_queue() -> util::OptionalRef<TransferQueue>
{
    if (m_queues.host_to_device_transfer_queue_pack.has_value())
    {
        return *m_queues.host_to_device_transfer_queue_pack;
    }
    return std::nullopt;
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto QueueProvider::available_queues() -> std::vector<std::reference_wrapper<QueueBase>>
{
    // TODO: join optionals when they have become ranges
    std::vector<std::reference_wrapper<QueueBase>> result;

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

    return Builder{};
}

auto QueueProvider::Builder::build(const Device& device) -> QueueProvider
{
    const auto queue_pack_from{
        [&device]<typename Queue_T>(std::in_place_type_t<Queue_T>, const QueueType type)
            -> auto
        {
            return [&device, type](const vulkan::QueueInfo& info) -> Queue_T
            {
                return Queue_T{
                    type,
                    info.family_index,
                    info.flags,
                    info.index,
                    vulkan::check_result(device.logical_device().getQueue2(
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
        .graphics_queue_pack = device.graphics_queue_info().transform(
            queue_pack_from(std::in_place_type<GraphicsQueue>, QueueType::eGraphics)
        ),
        .host_to_device_transfer_queue_pack =
            device.host_to_device_transfer_queue_info().transform(queue_pack_from(
                std::in_place_type<TransferQueue>, QueueType::eHostToDeviceTransfer
            )),
    };

    return QueueProvider{ std::move(queues) };
}

}   // namespace kiln::gfx::renderer
