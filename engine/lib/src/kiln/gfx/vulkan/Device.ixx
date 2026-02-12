export module kiln.gfx.vulkan.Device;

import vulkan_hpp;

import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.QueueGroup;

namespace kiln::gfx::vulkan {

export struct Device {
    vk::raii::PhysicalDevice   physical_device;
    vk::raii::Device           logical_device;
    QueueGroup                 queues;
    PhysicalDeviceCapabilities enabled_capabilities;
};

}   // namespace kiln::gfx::vulkan
