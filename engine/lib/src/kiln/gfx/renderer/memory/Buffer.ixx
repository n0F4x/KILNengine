module;

#include <utility>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.memory.Buffer;

import vulkan_hpp;

namespace kiln::gfx::renderer {

export class Buffer {
public:
    explicit Buffer(nullptr_t);
    Buffer(vk::raii::Buffer&& buffer, vk::DeviceSize buffer_size) noexcept;

    [[nodiscard]]
    auto get() const [[kiln_lifetimebound]] -> vk::Buffer;

    [[nodiscard]]
    auto size() const -> vk::DeviceSize;

    auto reset() -> void;

private:
    vk::raii::Buffer m_buffer;
    vk::DeviceSize   m_size{};
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

auto Buffer::get() const -> vk::Buffer
{
    return *m_buffer;
}

auto Buffer::size() const -> vk::DeviceSize
{
    return m_size;
}

auto Buffer::reset() -> void
{
    m_buffer.clear();
    m_size = 0;
}

Buffer::Buffer(nullptr_t) : m_buffer{ nullptr } {}

Buffer::Buffer(vk::raii::Buffer&& buffer, const vk::DeviceSize buffer_size) noexcept
    : m_buffer{ std::move(buffer) },
      m_size{ buffer_size }
{
}

}   // namespace kiln::gfx::renderer
