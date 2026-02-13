module;

#include <string>
#include <utility>

module kiln.gfx.renderer.Device;

namespace kiln::gfx::renderer {

Device::Device(
    vk::DebugUtilsMessengerEXT         debug_messenger,
    vk::raii::PhysicalDevice           physical_device,
    vk::raii::Device                   logical_device,
    vulkan::QueueGroup                 queues,
    vulkan::PhysicalDeviceCapabilities capabilities
)
    : m_debug_messenger{ std::move(debug_messenger) },
      m_physical_device{ std::move(physical_device) },
      m_logical_device{ std::move(logical_device) },
      m_queues{ std::move(queues) },
      m_capabilities{ std::move(capabilities) }
{
}

auto Device::name() const -> std::string
{
    return m_physical_device.getProperties2().properties.deviceName;
}

}   // namespace kiln::gfx::renderer
