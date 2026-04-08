module;

#include <memory_resource>
#include <span>
#include <vector>

#include <fastgltf/core.hpp>

export module kiln.gfx.asset.gltf.Asset;

import kiln.gfx.asset.Buffer;
import kiln.gfx.asset.IndexArrayView;
import kiln.gfx.asset.VertexArrayView;

namespace kiln::gfx::asset::gltf {

using AssetAllocator = std::pmr::polymorphic_allocator<>;

struct AssetPrecondition {
    explicit AssetPrecondition(
        const AssetAllocator&           allocator,
        const std::pmr::vector<Buffer>& custom_buffers
    );
};

export class Asset : AssetPrecondition {
public:
    using allocator_type = AssetAllocator;


    explicit Asset(fastgltf::Asset&& asset, std::pmr::vector<Buffer>&& custom_buffers);
    explicit Asset(
        std::allocator_arg_t,
        const allocator_type&      allocator,
        fastgltf::Asset&&          asset,
        std::pmr::vector<Buffer>&& custom_buffers
    );


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;
    [[nodiscard]]
    auto vertex_array_views() const noexcept -> std::span<const VertexArrayView>;
    [[nodiscard]]
    auto index_array_views() const noexcept -> std::span<const IndexArrayView>;

private:
    fastgltf::Asset                   m_asset;
    std::pmr::vector<Buffer>          m_buffers;
    std::pmr::vector<VertexArrayView> m_vertex_array_views;
    std::pmr::vector<IndexArrayView>  m_index_array_views;


    static auto fill_vertex_array_views(
        std::pmr::vector<VertexArrayView>& out,
        const fastgltf::Asset&             asset,
        const std::pmr::vector<Buffer>&    custom_buffers
    ) -> void;
    static auto fill_index_array_views(
        std::pmr::vector<IndexArrayView>& out,
        const fastgltf::Asset&            asset,
        const std::pmr::vector<Buffer>&   custom_buffers
    ) -> void;
};

}   // namespace kiln::gfx::asset::gltf
