module;

#include <string>
#include <utility>

module kiln.gfx.renderer.device.Device;

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

auto Device::graphics_queue() const -> GraphicsQueueRef
{
    return GraphicsQueueRef{
        *m_queues.graphics_queue(),
        *m_queues.graphics_queue_family(),
    };
}

auto Device::host_to_device_transfer_queue() const -> TransferQueueRef
{
    return TransferQueueRef{
        *m_queues.host_to_device_tranfer_queue(),
        *m_queues.host_to_device_transfer_queue_family(),
    };
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

auto Device::create_window(
    const vk::raii::Instance&      vulkan_instance,
    const wsi::Context&            wsi_context,
    const wsi::Window::CreateInfo& create_info
) const -> wsi::VulkanWindow
{
    return wsi::VulkanWindow{
        wsi::Window{ wsi_context, create_info },
        vulkan_instance,
        m_physical_device,
        m_logical_device,
        m_number_of_frames
    };
}

}   // namespace kiln::gfx::renderer
