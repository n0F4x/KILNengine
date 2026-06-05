module;

#include <cstdint>

export module kiln.gfx.vulkan.QueueInfo;

import vulkan_hpp;

import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::vulkan {

export struct QueueInfo {
    QueueFamilyIndex           family_index;
    vk::DeviceQueueCreateFlags flags;
    uint32_t                   index;
};

}   // namespace kiln::gfx::vulkan
