module;

#include <filesystem>
#include <format>
#include <print>
#include <span>
#include <stdexcept>

#include <vk_mem_alloc.h>

module examples.simple_scene.workflow.load_scene;

import vulkan_hpp;

import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.memory.Buffer;
import kiln.gfx.renderer.memory.BufferRegion;
import kiln.gfx.renderer.memory.LazyCopy;
import kiln.gfx.renderer.stream.StagingRequest;
import kiln.gfx.renderer.stream.StagingStream;
import kiln.util.Lazy;

import examples.simple_scene.workflow.Asset;

namespace demo {

[[nodiscard]]
auto index_buffer_byte_offset(const AssetLoader&) -> vk::DeviceSize
{
    return 0;
}

[[nodiscard]]
auto position_buffer_byte_offset(const AssetLoader& asset_loader) -> vk::DeviceSize
{
    vk::DeviceSize result{
        index_buffer_byte_offset(asset_loader) + asset_loader.indices_size_bytes()   //
    };

    if (const auto remainder{ result % asset_loader.position_alignment() };
        remainder != 0)
    {
        result += asset_loader.position_alignment() - remainder;
    }

    return result;
}

[[nodiscard]]
auto vertex_buffer_byte_offset(const AssetLoader& asset_loader) -> vk::DeviceSize
{
    vk::DeviceSize result{
        position_buffer_byte_offset(asset_loader)
        + asset_loader.positions_size_bytes()   //
    };

    if (const auto remainder{ result % asset_loader.vertex_alignment() }; remainder != 0)
    {
        result += asset_loader.vertex_alignment() - remainder;
    }

    return result;
}

[[nodiscard]]
auto geometry_buffer_size_from(const AssetLoader& asset_loader) -> vk::DeviceSize
{
    return vertex_buffer_byte_offset(asset_loader) + asset_loader.vertices_size_bytes();
}

[[nodiscard]]
auto geometry_alignment_from(const AssetLoader& asset_loader) -> vk::DeviceSize
{
    return std::max(
        std::max(asset_loader.index_alignment(), asset_loader.position_alignment()),
        asset_loader.vertex_alignment()
    );
}

[[nodiscard]]
auto create_geometry_buffer(
    kiln::gfx::renderer::Allocator& gpu_allocator,
    const AssetLoader&              asset_loader
) -> kiln::gfx::renderer::Buffer
{
    constexpr static vk::BufferUsageFlags2CreateInfo buffer_usage_flags{
        .usage = vk::BufferUsageFlagBits2::eTransferDst
               | vk::BufferUsageFlagBits2::eShaderDeviceAddress,
    };
    const vk::BufferCreateInfo buffer_create_info{
        .pNext       = &buffer_usage_flags,
        .size        = geometry_buffer_size_from(asset_loader),
        .sharingMode = vk::SharingMode::eExclusive,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    return gpu_allocator.create_buffer(
        buffer_create_info,
        allocation_create_info,
        geometry_alignment_from(asset_loader)
    );
}

[[nodiscard]]
auto create_material_buffer(
    kiln::gfx::renderer::Allocator& gpu_allocator,
    const AssetLoader&              asset_loader
) -> kiln::gfx::renderer::Buffer
{
    constexpr static vk::BufferUsageFlags2CreateInfo buffer_usage_flags{
        .usage = vk::BufferUsageFlagBits2::eTransferDst
               | vk::BufferUsageFlagBits2::eShaderDeviceAddress,
    };
    const vk::BufferCreateInfo buffer_create_info{
        .pNext       = &buffer_usage_flags,
        .size        = asset_loader.materials_size_bytes(),
        .sharingMode = vk::SharingMode::eExclusive,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    return gpu_allocator.create_buffer(
        buffer_create_info,
        allocation_create_info,
        asset_loader.material_alignment()
    );
}

[[nodiscard]]
auto create_primitive_buffer(
    kiln::gfx::renderer::Allocator& gpu_allocator,
    const AssetLoader&              asset_loader
) -> kiln::gfx::renderer::Buffer
{
    constexpr static vk::BufferUsageFlags2CreateInfo buffer_usage_flags{
        .usage = vk::BufferUsageFlagBits2::eTransferDst
               | vk::BufferUsageFlagBits2::eShaderDeviceAddress,
    };
    const vk::BufferCreateInfo buffer_create_info{
        .pNext       = &buffer_usage_flags,
        .size        = asset_loader.primitives_size_bytes(),
        .sharingMode = vk::SharingMode::eExclusive,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    return gpu_allocator.create_buffer(
        buffer_create_info,
        allocation_create_info,
        asset_loader.primitive_alignment()
    );
}

auto stage_indices(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    AssetLoader&                        asset_loader,
    kiln::gfx::renderer::Buffer&        geometry_buffer,
    const vk::MemoryPropertyFlags       memory_properties,
    vk::DeviceSize&                     buffer_offset
) -> void
{
    asset_loader.set_index_offset(0);

    if (const auto remainder{ buffer_offset % asset_loader.index_alignment() };
        remainder != 0)
    {
        buffer_offset += asset_loader.index_alignment() - remainder;
    }

    const kiln::gfx::renderer::BufferRegion destination{
        geometry_buffer,
        buffer_offset,
        asset_loader.indices_size_bytes(),
    };
    if (memory_properties & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        asset_loader.lazy_indices_copy()(allocator.map(destination));
        allocator.unmap(destination);
        std::println("Copied {} bytes (indices) from host to gpu", destination.size());
    }
    else
    {
        staging_stream.record(
            kiln::gfx::renderer::StagingRequest{
                asset_loader.lazy_indices_copy(),
                destination,
            }
        );
        std::println("Staged {} bytes (indices) from host to gpu", destination.size());
    }

    buffer_offset += destination.size();
}

auto stage_positions(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    AssetLoader&                        asset_loader,
    kiln::gfx::renderer::Buffer&        geometry_buffer,
    const vk::MemoryPropertyFlags       memory_properties,
    vk::DeviceSize&                     buffer_offset
) -> void
{
    asset_loader.set_position_offset(0);

    if (const auto remainder{ buffer_offset % asset_loader.position_alignment() };
        remainder != 0)
    {
        buffer_offset += asset_loader.position_alignment() - remainder;
    }

    const kiln::gfx::renderer::BufferRegion destination{
        geometry_buffer,
        buffer_offset,
        asset_loader.positions_size_bytes(),
    };

    if (memory_properties & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        asset_loader.lazy_positions_copy()(allocator.map(destination));
        allocator.unmap(destination);
        std::println("Copied {} bytes (positions) from host to gpu", destination.size());
    }
    else
    {
        staging_stream.record(
            kiln::gfx::renderer::StagingRequest{
                asset_loader.lazy_positions_copy(),
                destination,
            }
        );
        std::println("Staged {} bytes (positions) from host to gpu", destination.size());
    }

    buffer_offset += destination.size();
}

auto stage_vertices(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    AssetLoader&                        asset_loader,
    kiln::gfx::renderer::Buffer&        geometry_buffer,
    const vk::MemoryPropertyFlags       memory_properties,
    vk::DeviceSize&                     buffer_offset
) -> void
{
    asset_loader.set_vertex_offset(0);

    if (const auto remainder{ buffer_offset % asset_loader.vertex_alignment() };
        remainder != 0)
    {
        buffer_offset += asset_loader.vertex_alignment() - remainder;
    }

    const kiln::gfx::renderer::BufferRegion destination{
        geometry_buffer,
        buffer_offset,
        asset_loader.vertices_size_bytes(),
    };

    if (memory_properties & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        asset_loader.lazy_vertices_copy()(allocator.map(destination));
        allocator.unmap(destination);
        std::println("Copied {} bytes (vertices) from host to gpu", destination.size());
    }
    else
    {
        staging_stream.record(
            kiln::gfx::renderer::StagingRequest{
                asset_loader.lazy_vertices_copy(),
                destination,
            }
        );
        std::println("Staged {} bytes (vertices) from host to gpu", destination.size());
    }

    buffer_offset += destination.size();
}

auto stage_geometry(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    AssetLoader&                        asset_loader,
    kiln::gfx::renderer::Buffer&        geometry_buffer
) -> void
{
    const vk::MemoryPropertyFlags memory_properties{
        geometry_buffer.allocation().memory_properties()
    };

    vk::DeviceSize buffer_offset{};
    if (asset_loader.indices_size_bytes() != 0)
    {
        stage_indices(
            allocator,
            staging_stream,
            asset_loader,
            geometry_buffer,
            memory_properties,
            buffer_offset
        );
    }
    if (asset_loader.positions_size_bytes() != 0)
    {
        stage_positions(
            allocator,
            staging_stream,
            asset_loader,
            geometry_buffer,
            memory_properties,
            buffer_offset
        );
    }
    if (asset_loader.vertices_size_bytes() != 0)
    {
        stage_vertices(
            allocator,
            staging_stream,
            asset_loader,
            geometry_buffer,
            memory_properties,
            buffer_offset
        );
    }

    if ((memory_properties & vk::MemoryPropertyFlagBits::eHostVisible)
        && !(memory_properties & vk::MemoryPropertyFlagBits::eHostCoherent))
    {
        allocator.flush(geometry_buffer);
    }
}

auto stage_materials(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    AssetLoader&                        asset_loader,
    kiln::gfx::renderer::Buffer&        material_buffer
) -> void
{
    asset_loader.set_material_offset(0);

    if (material_buffer.allocation().memory_properties()
        & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        allocator.host_copy(asset_loader.lazy_materials_copy(), material_buffer);
        std::println(
            "Copied {} bytes (materials) from host to gpu",
            material_buffer.size()
        );
    }
    else
    {
        staging_stream.record(
            kiln::gfx::renderer::StagingRequest{
                asset_loader.lazy_materials_copy(),
                material_buffer,
            }
        );
        std::println(
            "Staged {} bytes (materials) from host to gpu",
            material_buffer.size()
        );
    }
}

auto stage_primitives(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    const AssetLoader&                  asset_loader,
    kiln::gfx::renderer::Buffer&        primitive_buffer
) -> void
{
    if (primitive_buffer.allocation().memory_properties()
        & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        allocator.host_copy(asset_loader.lazy_primitives_copy(), primitive_buffer);
        std::println(
            "Copied {} bytes (primitives) from host to gpu",
            primitive_buffer.size()
        );
    }
    else
    {
        staging_stream.record(
            kiln::gfx::renderer::StagingRequest{
                asset_loader.lazy_primitives_copy(),
                primitive_buffer,
            }
        );
        std::println(
            "Staged {} bytes (primitives) from host to gpu",
            primitive_buffer.size()
        );
    }
}

auto stage_asset(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    AssetLoader&                        asset_loader,
    kiln::gfx::renderer::Buffer&        geometry_buffer,
    kiln::gfx::renderer::Buffer&        material_buffer,
    kiln::gfx::renderer::Buffer&        primitive_buffer
) -> void
{
    if (geometry_buffer.size() != 0)
    {
        stage_geometry(allocator, staging_stream, asset_loader, geometry_buffer);
    }
    if (material_buffer.size() != 0)
    {
        stage_materials(allocator, staging_stream, asset_loader, material_buffer);
    }
    if (primitive_buffer.size() != 0)
    {
        stage_primitives(allocator, staging_stream, asset_loader, primitive_buffer);
    }
}

auto load_scene(
    const std::filesystem::path&        model_path,
    const kiln::gfx::renderer::Device&  device,
    kiln::gfx::renderer::Allocator&     gpu_allocator,
    kiln::gfx::asset::gltf::Parser&     model_parser,
    kiln::gfx::renderer::StagingStream& staging_stream
) -> Scene
{
    uint32_t       number_of_indices{};
    vk::DeviceSize index_byte_offset;
    vk::DeviceSize position_byte_offset;
    vk::DeviceSize vertex_byte_offset;

    kiln::gfx::renderer::Buffer geometry_buffer;
    kiln::gfx::renderer::Buffer material_buffer;
    kiln::gfx::renderer::Buffer transform_buffer;
    kiln::gfx::renderer::Buffer primitive_buffer;

    {
        const auto asset{
            model_parser.load(model_path)
                .value_or(
                    kiln::util::Lazy{
                        [&model_path] [[noreturn]]
                            -> decltype(model_parser.load(model_path))::value_type
                        {
                            throw std::runtime_error{
                                std::format(
                                    "Model could not be loaded from {}",
                                    model_path.generic_string()
                                )   //
                            };
                        }   //
                    }
                )   //
        };

        AssetLoader asset_loader{ asset };

        number_of_indices += asset_loader.number_of_indices();
        index_byte_offset    = index_buffer_byte_offset(asset_loader);
        position_byte_offset = position_buffer_byte_offset(asset_loader);
        vertex_byte_offset   = vertex_buffer_byte_offset(asset_loader);

        geometry_buffer  = create_geometry_buffer(gpu_allocator, asset_loader);
        material_buffer  = create_material_buffer(gpu_allocator, asset_loader);
        primitive_buffer = create_primitive_buffer(gpu_allocator, asset_loader);

        stage_asset(
            gpu_allocator,
            staging_stream,
            asset_loader,
            geometry_buffer,
            material_buffer,
            primitive_buffer
        );

        if (!staging_stream.empty())
        {
            staging_stream.flush(gpu_allocator);
        }
    }

    staging_stream.reset(device);

    return Scene{
        device,
        std::move(geometry_buffer),
        number_of_indices,
        index_byte_offset,
        position_byte_offset,
        vertex_byte_offset,
        std::move(material_buffer),
        std::move(transform_buffer),
        std::move(primitive_buffer),
    };
}

}   // namespace demo
