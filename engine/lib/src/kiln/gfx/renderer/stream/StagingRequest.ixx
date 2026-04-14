module;

#include <cstddef>
#include <span>
#include <utility>

export module kiln.gfx.renderer.stream.StagingRequest;

import vulkan_hpp;

import kiln.gfx.renderer.memory.BufferRegion;
import kiln.util.containers.MoveOnlyFunction;

namespace kiln::gfx::renderer {

export class StagingRequest {
public:
    using Callback = util::MoveOnlyFunction<auto(std::span<std::byte>) &&->void>;

    explicit StagingRequest(Callback&& callback, const BufferRegion& destination)
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
    Callback     m_callback;
    BufferRegion m_destination;
};

}   // namespace kiln::gfx::renderer
