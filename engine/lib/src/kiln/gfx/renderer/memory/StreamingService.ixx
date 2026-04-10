module;

#include <forward_list>
#include <memory_resource>

#include <vk_mem_alloc.h>

export module kiln.gfx.renderer.memory.StreamingService;

import kiln.app.Builder;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.memory.Allocation;
import kiln.gfx.renderer.memory.Buffer;
import kiln.gfx.renderer.memory.RingBuffer;
import kiln.util.containers.Indirect;

namespace kiln::gfx::renderer {

export class StreamingService {
public:
    class Builder;

    using allocator_type = std::pmr::polymorphic_allocator<>;


    explicit StreamingService(Allocator& allocator);
    explicit StreamingService(
        std::allocator_arg_t,
        const allocator_type& allocator,
        Allocator&            device_allocator
    );


private:
    RingBuffer                                             m_staging_ring_buffer;
    util::Indirect<std::pmr::unsynchronized_pool_resource> m_memory_resource;
    std::pmr::forward_list<Buffer>                         m_staging_buffers;
};

}   // namespace kiln::gfx::renderer
