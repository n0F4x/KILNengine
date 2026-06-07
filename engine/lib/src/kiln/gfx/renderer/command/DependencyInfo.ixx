module;

#include <span>

export module kiln.gfx.renderer.command.DependencyInfo;

import vulkan_hpp;

namespace kiln::gfx::renderer {

export struct DependencyInfo {
    std::span<const vk::MemoryBarrier2>       memory_barriers;
    std::span<const vk::BufferMemoryBarrier2> buffer_memory_barriers;
    std::span<const vk::ImageMemoryBarrier2>  image_memory_barriers;
};

}   // namespace kiln::gfx::renderer
