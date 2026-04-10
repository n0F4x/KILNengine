module;

#include <utility>

module kiln.gfx.renderer.memory.Buffer;

namespace kiln::gfx::renderer {

Buffer::Buffer(
    vk::raii::Buffer&&   buffer,
    const vk::DeviceSize buffer_size,
    Allocation&&         allocation
) noexcept
    : m_buffer{ std::move(buffer) },
      m_size{ buffer_size },
      m_allocation{ std::move(allocation) }
{
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto Buffer::get() noexcept -> vk::Buffer
{
    return *m_buffer;
}

auto Buffer::size() const noexcept -> vk::DeviceSize
{
    return m_size;
}

auto Buffer::reset() noexcept -> void
{
    m_allocation.reset();
    m_size = 0;
    m_buffer.clear();
}

}   // namespace kiln::gfx::renderer
