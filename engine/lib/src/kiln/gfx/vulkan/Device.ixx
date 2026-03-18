module;

#include <vector>

export module kiln.gfx.vulkan.Device;

import vulkan_hpp;

import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.QueueFamilyInfo;

namespace kiln::gfx::vulkan {

export struct Device {
    vk::raii::PhysicalDevice                               physical_device;
    vk::raii::Device                                       logical_device;
    PhysicalDeviceCapabilities                             enabled_capabilities;
    // TODO: use std::inplace_vector
    std::vector<QueueFamilyInfo> queue_families;
};

}   // namespace kiln::gfx::vulkan
