module;

#include <filesystem>
#include <memory_resource>
#include <span>
#include <utility>
#include <variant>

#include <fastgltf/core.hpp>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.asset.gltf.Asset;

import kiln.gfx.asset.ByteViewType;
import kiln.util.contracts;
import kiln.util.Overloaded;

namespace kiln::gfx::asset::gltf {

AssetPrecondition::AssetPrecondition(
    const AssetAllocator&           allocator,
    const std::pmr::vector<Buffer>& custom_buffers
)
{
    PRECOND(allocator == custom_buffers.get_allocator());
}

Asset::Asset(fastgltf::Asset&& asset, std::pmr::vector<Buffer>&& custom_buffers)
    : Asset{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          std::move(asset),
          std::move(custom_buffers),
      }
{
}

Asset::Asset(
    std::allocator_arg_t,
    const allocator_type&      allocator,
    fastgltf::Asset&&          asset,
    std::pmr::vector<Buffer>&& custom_buffers
)
    : AssetPrecondition{ allocator, custom_buffers },
      m_asset{ std::move(asset) },
      m_buffers{ std::move(custom_buffers) },
      m_buffer_views{ make_buffer_views(m_asset, m_buffers, allocator) }
{
}

auto Asset::get_allocator() const noexcept -> allocator_type
{
    return m_buffer_views.get_allocator();
}

auto Asset::byte_views() const noexcept -> std::span<const ByteView>
{
    return m_buffer_views;
}

[[nodiscard]]
// ReSharper disable once CppNotAllPathsReturnValue
auto buffer_view_type_from(const fastgltf::BufferTarget buffer_target) -> ByteViewType
{
    switch (buffer_target)
    {
        case fastgltf::BufferTarget::ArrayBuffer:        return ByteViewType::eVertex;
        case fastgltf::BufferTarget::ElementArrayBuffer: return ByteViewType::eIndex;
    }
}

[[nodiscard]]
auto buffer_view_type_from(const fastgltf::Optional<fastgltf::BufferTarget>& buffer_target)
    -> ByteViewType
{
    return buffer_target
        .transform(
            static_cast<auto (*)(fastgltf::BufferTarget)->ByteViewType>(
                buffer_view_type_from
            )
        )
        .value_or(ByteViewType::eUnknown);
}

[[nodiscard]]
auto bytes_ref_from(
    const fastgltf::BufferView&     buffer_view,
    const fastgltf::Asset&          asset,
    const std::pmr::vector<Buffer>& custom_buffers
) -> std::variant<std::filesystem::path, std::span<const std::byte>>
{
    return std::visit(
        util::Overloaded{
            [] [[noreturn]] (auto&&)
                -> std::variant<std::filesystem::path, std::span<const std::byte>>
            {
                std::unreachable();   //
            },
            [](const fastgltf::sources::URI& uri)
                -> std::variant<std::filesystem::path, std::span<const std::byte>>
            {
                return uri.uri.fspath();   //
            },
            [&custom_buffers](const fastgltf::sources::CustomBuffer& custom_buffer)
                -> std::variant<std::filesystem::path, std::span<const std::byte>>
            {
                return custom_buffers[custom_buffer.id].bytes();   //
            },
        },
        asset.buffers[buffer_view.bufferIndex].data
    );
}

auto Asset::make_buffer_views(
    const fastgltf::Asset&          asset,
    const std::pmr::vector<Buffer>& custom_buffers,
    const allocator_type&           allocator
) -> std::pmr::vector<ByteView>
{
    std::pmr::vector<ByteView> result{ allocator };
    result.reserve(asset.bufferViews.size());

    for (const fastgltf::BufferView& buffer_view : asset.bufferViews)
    {
        result.push_back(
            ByteView{
                .type        = buffer_view_type_from(buffer_view.target),
                .bytes       = bytes_ref_from(buffer_view, asset, custom_buffers),
                .byte_length = buffer_view.byteLength,
                .byte_offset = buffer_view.byteOffset,
                .byte_stride = static_cast<uint32_t>(buffer_view.byteStride.value_or(0)),
            }
        );
    }

    return result;
}

}   // namespace kiln::gfx::asset::gltf
