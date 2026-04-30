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

import examples.simple_scene.shaders;
import examples.simple_scene.workflow.GltfModelLoader;

namespace demo {

[[nodiscard]]
auto index_buffer_byte_offset(const GltfModelLoader&) -> vk::DeviceSize
{
    return 0;
}

[[nodiscard]]
auto position_buffer_byte_offset(const GltfModelLoader& model_loader) -> vk::DeviceSize
{
    vk::DeviceSize result{
        index_buffer_byte_offset(model_loader) + model_loader.indices_size_bytes()   //
    };

    if (const auto remainder{ result % model_loader.position_alignment() };
        remainder != 0)
    {
        result += model_loader.position_alignment() - remainder;
    }

    return result;
}

[[nodiscard]]
auto vertex_buffer_byte_offset(const GltfModelLoader& model_loader) -> vk::DeviceSize
{
    vk::DeviceSize result{
        position_buffer_byte_offset(model_loader)
        + model_loader.positions_size_bytes()   //
    };

    if (const auto remainder{ result % model_loader.vertex_alignment() }; remainder != 0)
    {
        result += model_loader.vertex_alignment() - remainder;
    }

    return result;
}

[[nodiscard]]
auto geometry_buffer_size_from(const GltfModelLoader& model_loader) -> vk::DeviceSize
{
    return vertex_buffer_byte_offset(model_loader) + model_loader.vertices_size_bytes();
}

[[nodiscard]]
auto geometry_alignment_from(const GltfModelLoader& model_loader) -> vk::DeviceSize
{
    return std::max(
        std::max(model_loader.index_alignment(), model_loader.position_alignment()),
        model_loader.vertex_alignment()
    );
}

[[nodiscard]]
auto create_geometry_buffer(
    kiln::gfx::renderer::Allocator& gpu_allocator,
    const GltfModelLoader&          model_loader
) -> kiln::gfx::renderer::Buffer
{
    constexpr static vk::BufferUsageFlags2CreateInfo buffer_usage_flags{
        .usage = vk::BufferUsageFlagBits2::eTransferDst
               | vk::BufferUsageFlagBits2::eShaderDeviceAddress,
    };
    const vk::BufferCreateInfo buffer_create_info{
        .pNext       = &buffer_usage_flags,
        .size        = geometry_buffer_size_from(model_loader),
        .sharingMode = vk::SharingMode::eExclusive,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    return gpu_allocator.create_buffer(
        buffer_create_info,
        allocation_create_info,
        geometry_alignment_from(model_loader)
    );
}

[[nodiscard]]
auto create_material_buffer(
    kiln::gfx::renderer::Allocator& gpu_allocator,
    const GltfModelLoader&          model_loader
) -> kiln::gfx::renderer::Buffer
{
    constexpr static vk::BufferUsageFlags2CreateInfo buffer_usage_flags{
        .usage = vk::BufferUsageFlagBits2::eTransferDst
               | vk::BufferUsageFlagBits2::eShaderDeviceAddress,
    };
    const vk::BufferCreateInfo buffer_create_info{
        .pNext       = &buffer_usage_flags,
        .size        = model_loader.materials_size_bytes(),
        .sharingMode = vk::SharingMode::eExclusive,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    return gpu_allocator.create_buffer(
        buffer_create_info,
        allocation_create_info,
        model_loader.material_alignment()
    );
}

[[nodiscard]]
auto create_transform_buffer(
    kiln::gfx::renderer::Allocator& gpu_allocator,
    const GltfModelLoader&          model_loader
) -> kiln::gfx::renderer::Buffer
{
    constexpr static vk::BufferUsageFlags2CreateInfo buffer_usage_flags{
        .usage = vk::BufferUsageFlagBits2::eTransferDst
               | vk::BufferUsageFlagBits2::eShaderDeviceAddress,
    };
    const vk::BufferCreateInfo buffer_create_info{
        .pNext       = &buffer_usage_flags,
        .size        = model_loader.transforms_size_bytes(),
        .sharingMode = vk::SharingMode::eExclusive,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    return gpu_allocator.create_buffer(
        buffer_create_info,
        allocation_create_info,
        model_loader.transform_alignment()
    );
}

[[nodiscard]]
consteval auto draw_command_count_size() noexcept -> uint32_t
{
    return sizeof(uint32_t);
}

[[nodiscard]]
auto create_draw_command_buffer(
    kiln::gfx::renderer::Allocator& gpu_allocator,
    const GltfModelLoader&          model_loader
) -> kiln::gfx::renderer::Buffer
{
    constexpr static vk::BufferUsageFlags2CreateInfo buffer_usage_flags{
        .usage = vk::BufferUsageFlagBits2::eTransferDst
               | vk::BufferUsageFlagBits2::eIndirectBuffer
               | vk::BufferUsageFlagBits2::eShaderDeviceAddress,
    };
    const vk::BufferCreateInfo buffer_create_info{
        .pNext = &buffer_usage_flags,
        .size  = draw_command_count_size()
              + sizeof(shaders::DrawCommand) * model_loader.draw_command_count(),
        .sharingMode = vk::SharingMode::eExclusive,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    return gpu_allocator.create_buffer(
        buffer_create_info,
        allocation_create_info,
        alignof(shaders::DrawCommand)
    );
}

auto stage_indices(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    GltfModelLoader&                    model_loader,
    kiln::gfx::renderer::Buffer&        geometry_buffer,
    const vk::MemoryPropertyFlags       memory_properties,
    vk::DeviceSize&                     buffer_offset
) -> void
{
    model_loader.set_index_offset(0);

    if (const auto remainder{ buffer_offset % model_loader.index_alignment() };
        remainder != 0)
    {
        buffer_offset += model_loader.index_alignment() - remainder;
    }

    const kiln::gfx::renderer::BufferRegion destination{
        geometry_buffer,
        buffer_offset,
        model_loader.indices_size_bytes(),
    };
    if (memory_properties & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        model_loader.index_writer()(allocator.map(destination));
        allocator.unmap(destination);
        std::println("Copied {} bytes (indices) from host to gpu", destination.size());
    }
    else
    {
        staging_stream.record(
            kiln::gfx::renderer::StagingRequest{
                model_loader.index_writer(),
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
    GltfModelLoader&                    model_loader,
    kiln::gfx::renderer::Buffer&        geometry_buffer,
    const vk::MemoryPropertyFlags       memory_properties,
    vk::DeviceSize&                     buffer_offset
) -> void
{
    model_loader.set_position_offset(0);

    if (const auto remainder{ buffer_offset % model_loader.position_alignment() };
        remainder != 0)
    {
        buffer_offset += model_loader.position_alignment() - remainder;
    }

    const kiln::gfx::renderer::BufferRegion destination{
        geometry_buffer,
        buffer_offset,
        model_loader.positions_size_bytes(),
    };

    if (memory_properties & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        model_loader.position_writer()(allocator.map(destination));
        allocator.unmap(destination);
        std::println("Copied {} bytes (positions) from host to gpu", destination.size());
    }
    else
    {
        staging_stream.record(
            kiln::gfx::renderer::StagingRequest{
                model_loader.position_writer(),
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
    GltfModelLoader&                    model_loader,
    kiln::gfx::renderer::Buffer&        geometry_buffer,
    const vk::MemoryPropertyFlags       memory_properties,
    vk::DeviceSize&                     buffer_offset
) -> void
{
    model_loader.set_vertex_offset(0);

    if (const auto remainder{ buffer_offset % model_loader.vertex_alignment() };
        remainder != 0)
    {
        buffer_offset += model_loader.vertex_alignment() - remainder;
    }

    const kiln::gfx::renderer::BufferRegion destination{
        geometry_buffer,
        buffer_offset,
        model_loader.vertices_size_bytes(),
    };

    if (memory_properties & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        model_loader.vertex_writer()(allocator.map(destination));
        allocator.unmap(destination);
        std::println("Copied {} bytes (vertices) from host to gpu", destination.size());
    }
    else
    {
        staging_stream.record(
            kiln::gfx::renderer::StagingRequest{
                model_loader.vertex_writer(),
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
    GltfModelLoader&                    model_loader,
    kiln::gfx::renderer::Buffer&        geometry_buffer
) -> void
{
    const vk::MemoryPropertyFlags memory_properties{
        geometry_buffer.allocation().memory_properties()
    };

    vk::DeviceSize buffer_offset{};
    if (model_loader.indices_size_bytes() != 0)
    {
        stage_indices(
            allocator,
            staging_stream,
            model_loader,
            geometry_buffer,
            memory_properties,
            buffer_offset
        );
    }
    if (model_loader.positions_size_bytes() != 0)
    {
        stage_positions(
            allocator,
            staging_stream,
            model_loader,
            geometry_buffer,
            memory_properties,
            buffer_offset
        );
    }
    if (model_loader.vertices_size_bytes() != 0)
    {
        stage_vertices(
            allocator,
            staging_stream,
            model_loader,
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
    GltfModelLoader&                    model_loader,
    kiln::gfx::renderer::Buffer&        material_buffer
) -> void
{
    model_loader.set_material_offset(0);

    if (material_buffer.allocation().memory_properties()
        & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        allocator.host_copy(model_loader.material_writer(), material_buffer);
        std::println(
            "Copied {} bytes (materials) from host to gpu",
            material_buffer.size()
        );
    }
    else
    {
        staging_stream.record(
            kiln::gfx::renderer::StagingRequest{
                model_loader.material_writer(),
                material_buffer,
            }
        );
        std::println(
            "Staged {} bytes (materials) from host to gpu",
            material_buffer.size()
        );
    }
}

auto stage_transforms(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    GltfModelLoader&                    model_loader,
    kiln::gfx::renderer::Buffer&        transform_buffer
) -> void
{
    model_loader.set_transform_offset(0);

    if (transform_buffer.allocation().memory_properties()
        & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        allocator.host_copy(model_loader.transform_writer(), transform_buffer);
        std::println(
            "Copied {} bytes (transforms) from host to gpu",
            transform_buffer.size()
        );
    }
    else
    {
        staging_stream.record(
            kiln::gfx::renderer::StagingRequest{
                model_loader.transform_writer(),
                transform_buffer,
            }
        );
        std::println(
            "Staged {} bytes (transforms) from host to gpu",
            transform_buffer.size()
        );
    }
}

[[nodiscard]]
auto lazy_copy_draw_commands(const GltfModelLoader& model_loader)
    -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [draw_count                    = model_loader.draw_command_count(),
         lazy_model_draw_commands_copy = model_loader.draw_command_writer()](
            const std::span<std::byte> staging_buffer
        ) -> void
        {
            std::memcpy(staging_buffer.data(), &draw_count, sizeof(decltype(draw_count)));
            lazy_model_draw_commands_copy(
                staging_buffer.subspan(draw_command_count_size())
            );
        }
    };
}

auto stage_draw_commands(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    const GltfModelLoader&              model_loader,
    kiln::gfx::renderer::Buffer&        draw_command_buffer
) -> void
{
    if (draw_command_buffer.allocation().memory_properties()
        & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        allocator.host_copy(lazy_copy_draw_commands(model_loader), draw_command_buffer);
        std::println(
            "Copied {} bytes (draw commands) from host to gpu",
            draw_command_buffer.size()
        );
    }
    else
    {
        staging_stream.record(
            kiln::gfx::renderer::StagingRequest{
                lazy_copy_draw_commands(model_loader),
                draw_command_buffer,
            }
        );
        std::println(
            "Staged {} bytes (draw commands) from host to gpu",
            draw_command_buffer.size()
        );
    }
}

auto stage_model(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    GltfModelLoader&                    model_loader,
    kiln::gfx::renderer::Buffer&        geometry_buffer,
    kiln::gfx::renderer::Buffer&        material_buffer,
    kiln::gfx::renderer::Buffer&        transform_buffer,
    kiln::gfx::renderer::Buffer&        draw_command_buffer
) -> void
{
    if (geometry_buffer.size() != 0)
    {
        stage_geometry(allocator, staging_stream, model_loader, geometry_buffer);
    }
    if (material_buffer.size() != 0)
    {
        stage_materials(allocator, staging_stream, model_loader, material_buffer);
    }
    if (transform_buffer.size() != 0)
    {
        stage_transforms(allocator, staging_stream, model_loader, transform_buffer);
    }
    if (draw_command_buffer.size() != 0)
    {
        stage_draw_commands(allocator, staging_stream, model_loader, draw_command_buffer);
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
    vk::DeviceSize index_byte_offset;
    vk::DeviceSize position_byte_offset;
    vk::DeviceSize vertex_byte_offset;

    kiln::gfx::renderer::Buffer geometry_buffer;
    kiln::gfx::renderer::Buffer material_buffer;
    kiln::gfx::renderer::Buffer transform_buffer;
    kiln::gfx::renderer::Buffer draw_command_buffer;
    uint32_t                    max_draw_count{};

    {
        const auto model{
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

        GltfModelLoader model_loader{ model };

        index_byte_offset    = index_buffer_byte_offset(model_loader);
        position_byte_offset = position_buffer_byte_offset(model_loader);
        vertex_byte_offset   = vertex_buffer_byte_offset(model_loader);

        geometry_buffer     = create_geometry_buffer(gpu_allocator, model_loader);
        material_buffer     = create_material_buffer(gpu_allocator, model_loader);
        transform_buffer    = create_transform_buffer(gpu_allocator, model_loader);
        draw_command_buffer = create_draw_command_buffer(gpu_allocator, model_loader);
        max_draw_count      = model_loader.draw_command_count();

        stage_model(
            gpu_allocator,
            staging_stream,
            model_loader,
            geometry_buffer,
            material_buffer,
            transform_buffer,
            draw_command_buffer
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
        index_byte_offset,
        position_byte_offset,
        vertex_byte_offset,
        std::move(material_buffer),
        std::move(transform_buffer),
        std::move(draw_command_buffer),
        draw_command_count_size(),
        max_draw_count,
    };
}

}   // namespace demo
