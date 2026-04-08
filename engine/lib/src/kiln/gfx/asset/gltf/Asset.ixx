module;

#include <memory_resource>
#include <span>
#include <vector>

#include <fastgltf/core.hpp>

export module kiln.gfx.asset.gltf.Asset;

import kiln.gfx.asset.Buffer;
import kiln.gfx.asset.ByteView;

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
    auto byte_views() const noexcept -> std::span<const ByteView>;

private:
    fastgltf::Asset            m_asset;
    std::pmr::vector<Buffer>   m_buffers;
    std::pmr::vector<ByteView> m_buffer_views;


    [[nodiscard]]
    static auto make_buffer_views(
        const fastgltf::Asset&          asset,
        const std::pmr::vector<Buffer>& custom_buffers,
        const allocator_type&           allocator
    ) -> std::pmr::vector<ByteView>;
};

}   // namespace kiln::gfx::asset::gltf
