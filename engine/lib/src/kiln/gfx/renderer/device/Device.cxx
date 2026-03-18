module;

#include <string>
#include <utility>

module kiln.gfx.renderer.device.Device;

namespace kiln::gfx::renderer {

Device::Device(
    vk::raii::PhysicalDevice           physical_device,
    vk::raii::Device                   logical_device,
    vulkan::PhysicalDeviceCapabilities capabilities
)
    : m_physical_device{ std::move(physical_device) },
      m_logical_device{ std::move(logical_device) },
      m_capabilities{ std::move(capabilities) }
{
}

auto Device::name() const -> std::string
{
    return m_physical_device.getProperties2().properties.deviceName;
}

auto Device::physical_device() const noexcept -> const vk::raii::PhysicalDevice&
{
    return m_physical_device;
}

auto Device::logical_device() const noexcept -> const vk::raii::Device&
{
    return m_logical_device;
}

auto Device::capabilities() const noexcept -> const vulkan::PhysicalDeviceCapabilities&
{
    return m_capabilities;
}

}   // namespace kiln::gfx::renderer
