module;

#include <cstddef>
#include <span>
#include <utility>

#include "kiln/util/contract_macros.hpp"

export module kiln.gfx.renderer.stream.StagingRequest;

import vulkan_hpp;

import kiln.gfx.renderer.memory.BufferRegion;
import kiln.gfx.renderer.memory.LazyCopy;
import kiln.util.contracts;

namespace kiln::gfx::renderer {

class StagingRequestPrecondition {
public:
    explicit StagingRequestPrecondition([[maybe_unused]] const BufferRegion& destination)
    {
        PRECOND(destination.size() != 0);
    }
};

export class StagingRequest : StagingRequestPrecondition {
public:
    explicit StagingRequest(LazyCopy&& callback, const BufferRegion& destination)
        : StagingRequestPrecondition{ destination },
          m_callback{ std::move(callback) },
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
