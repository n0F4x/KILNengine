module;

#include <optional>

module kiln.gfx.renderer.command.QueueProvider;

import kiln.wsi.Context;
import kiln.gfx.renderer.command.queue_requests;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.result.check_result;

namespace kiln::gfx::renderer {

QueueProvider::QueueProvider(const Queues& queues) : m_queues{ queues } {}

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

namespace internal {

/*
 * We temporarily depend on presentation support as we ask for a presentation queue.
 * This should be refactored, so that someone else asks for it
 */
auto QueueProviderBuilder::create(
    const PresentationContextBuilder&,
    DeviceBuilder& device_builder
) -> QueueProviderBuilder
{
    return QueueProviderBuilder{ device_builder };
}

QueueProviderBuilder::QueueProviderBuilder(DeviceBuilder& device_builder)
    : m_device_builder_ref{ device_builder }
{
}

auto QueueProviderBuilder::require_graphics_queue() -> void
{
    if (m_graphics_queue_info.requested)
    {
        return;
    }

    m_device_builder_ref.get().register_configuration(
        [this](
            DeviceBuilder&          device_builder,
            const vulkan::Instance& instance,
            const wsi::Context&     context
        ) -> void
        {
            device_builder.request_queue(
                GraphicsQueueRequest{
                    instance,
                    context,
                    QueueRequestResultCallback{
                        [this](
                            vulkan::QueueFamilyIndex family_index, std::uint32_t queue_index
                        ) -> void
                        {
                            // This callback only works
                            // because context builders don't move once they are in place
                            m_graphics_queue_info.callback_result =
                                std::pair{ family_index, queue_index };
                        }   //
                    },
                }
            );   //
        }
    );

    m_graphics_queue_info.requested = true;
}

auto QueueProviderBuilder::build(const Device& device) const -> QueueProvider
{
    const QueueProvider::Queues queues{
        .graphics_queue_pack = m_graphics_queue_info.callback_result.transform(
            [&device](std::pair<vulkan::QueueFamilyIndex, std::uint32_t> info)
                -> QueueProvider::QueuePack
            {
                return QueueProvider::QueuePack{
                    .family_index = info.first,
                    .index        = info.second,
                    .queue = vulkan::check_result(device.logical_device().getQueue2(
                        vk::DeviceQueueInfo2{
                            .queueFamilyIndex = info.first.underlying(),
                            .queueIndex       = info.second,
                        }
                    )),
                };
            }
        )
    };

    return QueueProvider{ queues };
}
}   // namespace internal

}   // namespace kiln::gfx::renderer
