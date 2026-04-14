module;

#include <fastgltf/core.hpp>

export module examples.simple_scene.workflow.Asset;

import kiln.gfx.renderer.memory.BufferRegion;
import kiln.gfx.renderer.stream.LazyCopy;

namespace demo {

export class Asset {
public:
    explicit Asset(fastgltf::Asset&& asset);

    [[nodiscard]]
    auto indices_size_bytes() const noexcept -> uint32_t;
    [[nodiscard]]
    auto positions_size_bytes() const noexcept -> uint32_t;
    [[nodiscard]]
    auto rest_of_vertices_size_bytes() const noexcept -> uint32_t;

    [[nodiscard]]
    auto lazy_indices_copy() const noexcept -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto lazy_positions_copy() const noexcept -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto lazy_rest_of_vertices_copy() const noexcept -> kiln::gfx::renderer::LazyCopy;

private:
    fastgltf::Asset m_asset;
};

}   // namespace demo
