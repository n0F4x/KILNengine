module;

#include <memory>

#include <vk_mem_alloc.h>

export module kiln.gfx.renderer.allocator.Allocator;

import vulkan_hpp;

import kiln.gfx.renderer.device.Device;

namespace kiln::gfx::renderer {

export class Allocator {
public:
    Allocator(const vk::raii::Instance& instance, const Device& device);

private:
    std::unique_ptr<VmaAllocator_T, decltype(&vmaDestroyAllocator)> m_handle;
};

}   // namespace kiln::gfx::renderer
