module;

#include <cstdint>

export module kiln.gfx.vulkan.QueuePack;

import vulkan_hpp;

import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::vulkan {

export struct QueuePack {
    QueueFamilyIndex family_index;
    uint32_t         queue_index;
    vk::raii::Queue  queue;
};

}   // namespace kiln::gfx::vulkan
