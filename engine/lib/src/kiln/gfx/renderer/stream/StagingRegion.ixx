module;

#include <cstddef>
#include <span>

#include "kiln/util/contract_macros.hpp"

export module kiln.gfx.renderer.stream.StagingRegion;

import vulkan_hpp;

import kiln.gfx.renderer.memory.BufferRegion;
import kiln.util.contracts;

namespace kiln::gfx::renderer {

class StagingRegionPrecondition {
public:
    explicit StagingRegionPrecondition(
        const std::span<const std::byte> data,
        const BufferRegion&              destination
    )
    {
        PRECOND(data.size() == destination.size());
        PRECOND(data.size() != 0);
    }
};

export class StagingRegion : StagingRegionPrecondition {
public:
    explicit StagingRegion(
        const std::span<const std::byte> data,
        const BufferRegion&              destination
    )
        : StagingRegionPrecondition{ data, destination },
          m_data{ data },
          m_destination{ destination }
    {
    }

    [[nodiscard]]
    auto data() const noexcept -> std::span<const std::byte>
    {
        return m_data;
    }

    [[nodiscard]]
    auto destination() const noexcept -> const BufferRegion&
    {
        return m_destination;
    }

    [[nodiscard]]
    auto size() const noexcept -> vk::DeviceSize
    {
        return m_destination.size();
    }

private:
    std::span<const std::byte> m_data;
    BufferRegion               m_destination;
};

}   // namespace kiln::gfx::renderer
