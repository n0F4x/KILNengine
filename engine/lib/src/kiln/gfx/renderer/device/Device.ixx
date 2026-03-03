module;

#include <string>

export module kiln.gfx.renderer.device.Device;

import vulkan_hpp;

import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.QueueFamilyIndex;
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
    auto graphics_queue() const -> const vk::raii::Queue&;
    [[nodiscard]]
    auto graphics_queue_family_index() const -> vulkan::QueueFamilyIndex;
    [[nodiscard]]
    auto host_to_device_transfer_queue() const -> const vk::raii::Queue&;
    [[nodiscard]]
    auto host_to_device_transfer_queue_family_index() const -> vulkan::QueueFamilyIndex;
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
