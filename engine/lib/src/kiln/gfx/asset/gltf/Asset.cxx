module;

#include <filesystem>
#include <memory_resource>
#include <span>
#include <utility>
#include <variant>

#include <fastgltf/core.hpp>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.asset.gltf.Asset;

import kiln.gfx.asset.IndexArrayView;
import kiln.gfx.asset.VertexArrayView;
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
      m_vertex_array_views{ allocator },
      m_index_array_views{ allocator }
{
    std::size_t number_of_vertex_array_views{};
    std::size_t number_of_index_array_views{};
    for (const fastgltf::BufferView& buffer_view : asset.bufferViews)
    {
        if (buffer_view.target.has_value())
        {
            if (*buffer_view.target == fastgltf::BufferTarget::ArrayBuffer)
            {
                ++number_of_vertex_array_views;
            }
            else if (*buffer_view.target == fastgltf::BufferTarget::ElementArrayBuffer)
            {
                ++number_of_index_array_views;
            }
        }
    }
    m_vertex_array_views.reserve(number_of_vertex_array_views);
    m_index_array_views.reserve(number_of_index_array_views);

    fill_vertex_array_views(m_vertex_array_views, m_asset, m_buffers);
    fill_index_array_views(m_index_array_views, m_asset, m_buffers);
}

auto Asset::get_allocator() const noexcept -> allocator_type
{
    return m_buffers.get_allocator();
}

auto Asset::vertex_array_views() const noexcept -> std::span<const VertexArrayView>
{
    return m_vertex_array_views;
}

auto Asset::index_array_views() const noexcept -> std::span<const IndexArrayView>
{
    return m_index_array_views;
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

[[nodiscard]]
constexpr auto byte_stride_from(const fastgltf::Optional<std::size_t>& byte_stride)
    -> std::optional<uint32_t>
{
    if (byte_stride.has_value())
    {
        return static_cast<uint32_t>(*byte_stride);
    }
    return std::nullopt;
}

auto Asset::fill_vertex_array_views(
    std::pmr::vector<VertexArrayView>& out,
    const fastgltf::Asset&             asset,
    const std::pmr::vector<Buffer>&    custom_buffers
) -> void
{
    for (const fastgltf::BufferView& buffer_view : asset.bufferViews)
    {
        if (buffer_view.target.has_value()
            && *buffer_view.target == fastgltf::BufferTarget::ArrayBuffer)
        {
            out.push_back(
                VertexArrayView{
                    .bytes       = bytes_ref_from(buffer_view, asset, custom_buffers),
                    .byte_length = static_cast<uint32_t>(buffer_view.byteLength),
                    .byte_offset = static_cast<uint32_t>(buffer_view.byteOffset),
                    .byte_stride = byte_stride_from(buffer_view.byteStride),
                }
            );
        }
    }
}

auto Asset::fill_index_array_views(
    std::pmr::vector<IndexArrayView>& out,
    const fastgltf::Asset&            asset,
    const std::pmr::vector<Buffer>&   custom_buffers
) -> void
{
    for (const fastgltf::BufferView& buffer_view : asset.bufferViews)
    {
        if (buffer_view.target.has_value()
            && *buffer_view.target == fastgltf::BufferTarget::ElementArrayBuffer)
        {
            out.push_back(
                IndexArrayView{
                    .bytes       = bytes_ref_from(buffer_view, asset, custom_buffers),
                    .byte_length = buffer_view.byteLength,
                    .byte_offset = buffer_view.byteOffset,
                }
            );
        }
    }
}

}   // namespace kiln::gfx::asset::gltf
