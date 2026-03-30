module;

#include <cstdint>
#include <utility>

module kiln.gfx.renderer.command.QueueProviderPlugin;

import vulkan_hpp;

import kiln.gfx.renderer.command.queue_requests;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.result.check_result;
import kiln.wsi.Context;

namespace kiln::gfx::renderer {

auto QueueProviderPlugin::create_plugin(DevicePlugin& device_plugin, const CommandPlugin&)
    -> QueueProviderPlugin
{
    return QueueProviderPlugin{ device_plugin };
}

QueueProviderPlugin::QueueProviderPlugin(DevicePlugin& device_plugin)
    : m_device_plugin_ref{ device_plugin }
{
}

auto QueueProviderPlugin::require_graphics_queue() -> void
{
    if (m_graphics_queue_info.requested)
    {
        return;
    }

    m_device_plugin_ref.get().register_configuration(
        [this](
            DevicePlugin&             u_device_plugin,
            const vk::raii::Instance& instance,
            const wsi::Context&       context
        ) -> void
        {
            u_device_plugin->request_queue(
                GraphicsQueueRequest{
                    instance,
                    context,
                    QueueRequestResultCallback{
                        [this](
                            vulkan::QueueFamilyIndex family_index, std::uint32_t queue_index
                        ) -> void
                        {
                            // This callback only works
                            // because plugins don't move once they are in place
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

auto QueueProviderPlugin::operator()(const Device& device) const -> QueueProvider
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

}   // namespace kiln::gfx::renderer
