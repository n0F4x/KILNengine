module;

#include <string>

export module kiln.gfx.renderer.Device;

import vulkan_hpp;

import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.QueueGroup;

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

private:
    vk::DebugUtilsMessengerEXT         m_debug_messenger;
    vk::raii::PhysicalDevice           m_physical_device;
    vk::raii::Device                   m_logical_device;
    vulkan::QueueGroup                 m_queues;
    vulkan::PhysicalDeviceCapabilities m_capabilities;
};

}   // namespace kiln::gfx::renderer
