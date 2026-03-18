module;

#include <utility>

module kiln.gfx.renderer.device.DevicePlugin;

import kiln.gfx.renderer.device.DevicePluginFailedError;
import kiln.gfx.vulkan.Device;
import kiln.util.Lazy;

namespace kiln::gfx::renderer {

auto DevicePlugin::build(const vk::raii::Instance& instance) const -> Device
{
    auto [physical_device, logical_device, queues, enabled_capabilities] =
        m_device_builder.build(instance).value_or(
            util::Lazy{
                [] [[noreturn]] -> vulkan::Device
                {
                    throw DevicePluginFailedError{ "No supported device found" };   //
                }   //
            }
        );

    return Device{
        std::move(physical_device),
        std::move(logical_device),
        std::move(queues),
        std::move(enabled_capabilities),
    };
}

}   // namespace kiln::gfx::renderer
