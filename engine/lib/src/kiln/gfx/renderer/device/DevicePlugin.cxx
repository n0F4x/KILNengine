module;

#include <utility>

module kiln.gfx.renderer.device.DevicePlugin;

import kiln.gfx.renderer.device.DevicePluginFailedError;
import kiln.gfx.vulkan.Device;
import kiln.util.Lazy;

namespace kiln::gfx::renderer {

DevicePlugin::DevicePlugin(const allocator_type& allocator)
    : m_device_builder{ allocator }
{
}

DevicePlugin::DevicePlugin(const DevicePlugin& other, const allocator_type& allocator)
    : m_device_builder{ other.m_device_builder, allocator }
{
}

DevicePlugin::DevicePlugin(DevicePlugin&& other, const allocator_type& allocator)
    : m_device_builder{ std::move(other.m_device_builder), allocator }
{
}

auto DevicePlugin::get_allocator() const -> allocator_type
{
    return m_device_builder.get_allocator();
}

auto DevicePlugin::operator()(const vk::raii::Instance& instance) const -> Device
{
    auto [physical_device, logical_device, enabled_capabilities, queue_families] =
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
        std::move(enabled_capabilities),
    };
}

}   // namespace kiln::gfx::renderer
