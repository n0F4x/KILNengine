module;

#include <string>

export module kiln.gfx.renderer.device.Device;

import vulkan_hpp;

import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.QueueGroup;
import kiln.wsi.Context;
import kiln.wsi.VulkanWindow;
import kiln.wsi.Window;

namespace kiln::gfx::renderer {

export class Device {
public:
    Device(
        vk::DebugUtilsMessengerEXT         debug_messenger,
        vk::raii::PhysicalDevice           physical_device,
        vk::raii::Device                   logical_device,
        vulkan::QueueGroup                 queues,
        vulkan::PhysicalDeviceCapabilities capabilities
    );

    [[nodiscard]]
    auto name() const -> std::string;

    [[nodiscard]]
    auto physical_device() const noexcept -> const vk::raii::PhysicalDevice&;

    [[nodiscard]]
    auto logical_device() const noexcept -> const vk::raii::Device&;

    [[nodiscard]]
    auto capabilities() const noexcept -> const vulkan::PhysicalDeviceCapabilities&;

    [[nodiscard]]
    auto create_window(
        const vk::raii::Instance&      vulkan_instance,
        const wsi::Context&            wsi_context,
        const wsi::Window::CreateInfo& create_info
    ) const -> wsi::VulkanWindow;

private:
    vk::DebugUtilsMessengerEXT         m_debug_messenger;
    vk::raii::PhysicalDevice           m_physical_device;
    vk::raii::Device                   m_logical_device;
    vulkan::QueueGroup                 m_queues;
    vulkan::PhysicalDeviceCapabilities m_capabilities;
    uint32_t                           m_number_of_frames{ 2 };
};

}   // namespace kiln::gfx::renderer

module :private;

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
