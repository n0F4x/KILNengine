module;

#include <vk_mem_alloc.h>

export module kiln.gfx.renderer.memory.RingBuffer;

import vulkan_hpp;

import kiln.gfx.renderer.memory.Allocator;

namespace kiln::gfx::renderer {

export class RingBuffer {
public:
    struct Options {
        vk::BufferUsageFlags2    buffer_flags;
        VmaAllocationCreateFlags allocation_flags;
        vk::DeviceSize           size;
    };


    RingBuffer(RingBuffer&&) noexcept;
    ~RingBuffer();

    explicit RingBuffer(Allocator& allocator, const Options& options);

    auto operator=(RingBuffer&&) noexcept -> RingBuffer&;

private:
    VmaAllocator m_allocator;
    VmaPool      m_pool;
};

}   // namespace kiln::gfx::renderer
