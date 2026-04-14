module;

#include <cstddef>
#include <span>
#include <utility>

export module kiln.gfx.renderer.stream.StagingRequest;

import vulkan_hpp;

import kiln.gfx.renderer.memory.BufferRegion;
import kiln.gfx.renderer.stream.LazyCopy;

namespace kiln::gfx::renderer {

export class StagingRequest {
public:
    explicit StagingRequest(LazyCopy&& callback, const BufferRegion& destination)
        : m_callback{ std::move(callback) },
          m_destination{ destination }
    {
    }

    auto operator()(const std::span<std::byte> out) && -> void
    {
        std::move(m_callback)(out);
    }

    [[nodiscard]]
    auto destination() const noexcept -> const BufferRegion&
    {
        return m_destination;
    }

private:
    LazyCopy     m_callback;
    BufferRegion m_destination;
};

}   // namespace kiln::gfx::renderer
