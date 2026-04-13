module;

#include <functional>

#include "kiln/util/contract_macros.hpp"
#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.memory.BufferRegion;

import vulkan_hpp;

import kiln.gfx.renderer.memory.Allocation;
import kiln.gfx.renderer.memory.Buffer;
import kiln.util.contracts;

namespace kiln::gfx::renderer {

class BufferRegionPrecondition {
public:
    explicit BufferRegionPrecondition() = default;

    explicit BufferRegionPrecondition(
        const Buffer&        buffer,
        const vk::DeviceSize offset,
        const vk::DeviceSize size
    )
    {
        PRECOND(buffer.size() - offset >= size);
    }
};

export class BufferRegion : BufferRegionPrecondition {
public:
    explicit(false) BufferRegion(   //
        [[kiln_lifetimebound]]
        Buffer& buffer
    )
        : m_buffer{ buffer.get() },
          m_allocation_ref{ buffer.allocation() },
          m_offset{ 0 },
          m_size{ buffer.size() }
    {
    }

    explicit BufferRegion(
        [[kiln_lifetimebound]] Buffer& buffer,
        const vk::DeviceSize           offset,
        const vk::DeviceSize           size
    )
        : BufferRegionPrecondition{ buffer, offset, size },
          m_buffer{ buffer.get() },
          m_allocation_ref{ buffer.allocation() },
          m_offset{ offset },
          m_size{ size }
    {
    }

    [[nodiscard]]
    auto buffer() const noexcept -> vk::Buffer
    {
        return m_buffer;
    }

    [[nodiscard]]
    auto allocation() const noexcept -> Allocation&
    {
        return m_allocation_ref;
    }

    [[nodiscard]]
    auto offset() const noexcept -> vk::DeviceSize
    {
        return m_offset;
    }

    [[nodiscard]]
    auto size() const noexcept -> vk::DeviceSize
    {
        return m_size;
    }

private:
    vk::Buffer                         m_buffer;
    std::reference_wrapper<Allocation> m_allocation_ref;
    vk::DeviceSize                     m_offset;
    vk::DeviceSize                     m_size;
};

}   // namespace kiln::gfx::renderer
