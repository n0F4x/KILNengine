module;

export module kiln.gfx.renderer.memory.MemoryView;

import vulkan_hpp;

namespace kiln::gfx::renderer {

export struct MemoryView {
    vk::DeviceMemory memory;
    vk::DeviceSize   offset{};
    vk::DeviceSize   size{ vk::WholeSize };
};

}   // namespace kiln::gfx::renderer
