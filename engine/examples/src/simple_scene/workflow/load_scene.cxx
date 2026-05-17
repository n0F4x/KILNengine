module;

#include <filesystem>
#include <format>
#include <memory_resource>
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
        .size        = model_loader.geometry_buffer_size_bytes(),
        .sharingMode = vk::SharingMode::eExclusive,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    return gpu_allocator.create_buffer(
        buffer_create_info,
        allocation_create_info,
        model_loader.geometry_buffer_alignment()
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
        .size        = model_loader.material_buffer_size_bytes(),
        .sharingMode = vk::SharingMode::eExclusive,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    return gpu_allocator.create_buffer(
        buffer_create_info,
        allocation_create_info,
        model_loader.material_buffer_alignment()
    );
}

[[nodiscard]]
auto create_instance_buffer(
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
        .size        = model_loader.instance_buffer_size_bytes(),
        .sharingMode = vk::SharingMode::eExclusive,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    return gpu_allocator.create_buffer(
        buffer_create_info,
        allocation_create_info,
        model_loader.instance_buffer_alignment()
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

auto stage_geometry_buffer(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    const std::span<GltfModelLoader>    model_loaders,
    kiln::gfx::renderer::Buffer&        geometry_buffer
) -> void
{
    const vk::MemoryPropertyFlags geometry_buffer_memory_properties{
        geometry_buffer.allocation().memory_properties()
    };

    uint32_t buffer_byte_offset{};
    for (GltfModelLoader& model_loader : model_loaders)
    {
        if (model_loader.geometry_buffer_size_bytes() != 0)
        {
            if (const auto remainder{ buffer_byte_offset
                                      % model_loader.geometry_buffer_alignment() };
                remainder != 0)
            {
                buffer_byte_offset
                    += model_loader.geometry_buffer_alignment() - remainder;
            }

            model_loader.set_geometry_buffer_byte_offset(buffer_byte_offset);

            const kiln::gfx::renderer::BufferRegion destination{
                geometry_buffer,
                buffer_byte_offset,
                model_loader.geometry_buffer_size_bytes(),
            };
            if (geometry_buffer_memory_properties
                & vk::MemoryPropertyFlagBits::eHostVisible)
            {
                allocator.host_copy(model_loader.geometry_writer(), destination);
            }
            else
            {
                staging_stream.record(
                    kiln::gfx::renderer::StagingRequest{
                        model_loader.geometry_writer(),
                        destination,
                    }
                );
            }

            buffer_byte_offset += destination.size();
        }
    }

    if (geometry_buffer_memory_properties & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        std::println(
            "Copied {} bytes (geometry) from host to gpu",
            geometry_buffer.size()
        );
    }
    else
    {
        std::println(
            "Staged {} bytes (geometry) from host to gpu",
            geometry_buffer.size()
        );
    }
}

auto stage_material_buffer(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    const std::span<GltfModelLoader>    model_loaders,
    kiln::gfx::renderer::Buffer&        material_buffer
) -> void
{
    const vk::MemoryPropertyFlags material_buffer_memory_properties{
        material_buffer.allocation().memory_properties()
    };

    uint32_t buffer_byte_offset{};
    for (GltfModelLoader& model_loader : model_loaders)
    {
        if (model_loader.material_buffer_size_bytes() != 0)
        {
            if (const auto remainder{ buffer_byte_offset
                                      % model_loader.material_buffer_alignment() };
                remainder != 0)
            {
                buffer_byte_offset
                    += model_loader.material_buffer_alignment() - remainder;
            }

            model_loader.set_material_buffer_byte_offset(buffer_byte_offset);

            const kiln::gfx::renderer::BufferRegion destination{
                material_buffer,
                buffer_byte_offset,
                model_loader.material_buffer_size_bytes(),
            };
            if (material_buffer_memory_properties
                & vk::MemoryPropertyFlagBits::eHostVisible)
            {
                allocator.host_copy(model_loader.material_writer(), destination);
            }
            else
            {
                staging_stream.record(
                    kiln::gfx::renderer::StagingRequest{
                        model_loader.material_writer(),
                        destination,
                    }
                );
            }

            buffer_byte_offset += destination.size();
        }
    }

    if (material_buffer_memory_properties & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        std::println(
            "Copied {} bytes (materials) from host to gpu",
            material_buffer.size()
        );
    }
    else
    {
        std::println(
            "Staged {} bytes (materials) from host to gpu",
            material_buffer.size()
        );
    }
}

auto stage_instance_buffer(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    const std::span<GltfModelLoader>    model_loaders,
    kiln::gfx::renderer::Buffer&        instance_buffer
) -> void
{
    const vk::MemoryPropertyFlags buffer_memory_properties{
        instance_buffer.allocation().memory_properties()
    };

    uint32_t buffer_byte_offset{};
    for (GltfModelLoader& model_loader : model_loaders)
    {
        if (model_loader.instance_buffer_size_bytes() != 0)
        {
            if (const auto remainder{ buffer_byte_offset
                                      % model_loader.instance_buffer_alignment() };
                remainder != 0)
            {
                buffer_byte_offset
                    += model_loader.instance_buffer_alignment() - remainder;
            }

            model_loader.set_instance_buffer_byte_offset(buffer_byte_offset);

            const kiln::gfx::renderer::BufferRegion destination{
                instance_buffer,
                buffer_byte_offset,
                model_loader.instance_buffer_size_bytes(),
            };
            if (buffer_memory_properties & vk::MemoryPropertyFlagBits::eHostVisible)
            {
                allocator.host_copy(model_loader.instance_writer(), destination);
            }
            else
            {
                staging_stream.record(
                    kiln::gfx::renderer::StagingRequest{
                        model_loader.instance_writer(),
                        destination,
                    }
                );
            }

            buffer_byte_offset += destination.size();
        }
    }

    if (buffer_memory_properties & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        std::println(
            "Copied {} bytes (instances) from host to gpu",
            instance_buffer.size()
        );
    }
    else
    {
        std::println(
            "Staged {} bytes (instances) from host to gpu",
            instance_buffer.size()
        );
    }
}

[[nodiscard]]
auto draw_count_from(const std::span<const GltfModelLoader> model_loaders) -> uint32_t
{
    uint32_t result{};

    for (const GltfModelLoader& model_loader : model_loaders)
    {
        result += model_loader.draw_command_count();
    }

    return result;
}

[[nodiscard]]
auto lazy_copy_draw_commands(const std::span<const GltfModelLoader> model_loaders)
    -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [draw_count = draw_count_from(model_loaders),
         model_loaders]   //
        (const std::span<std::byte> staging_buffer) -> void
        {
            std::memcpy(staging_buffer.data(), &draw_count, sizeof(decltype(draw_count)));

            uint32_t buffer_byte_offset{ sizeof(decltype(draw_count)) };
            for (const GltfModelLoader& model_loader : model_loaders)
            {
                const uint32_t draw_commands_size_bytes{
                    static_cast<uint32_t>(
                        model_loader.draw_command_count() * sizeof(shaders::DrawCommand)
                    ),
                };
                model_loader.draw_command_writer()(
                    staging_buffer.subspan(buffer_byte_offset, draw_commands_size_bytes)
                );
                buffer_byte_offset += draw_commands_size_bytes;
            }
        }
    };
}

auto stage_draw_commands(
    kiln::gfx::renderer::Allocator&        allocator,
    kiln::gfx::renderer::StagingStream&    staging_stream,
    const std::span<const GltfModelLoader> model_loaders,
    kiln::gfx::renderer::Buffer&           draw_command_buffer
) -> void
{
    if (draw_command_buffer.allocation().memory_properties()
        & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        allocator.host_copy(lazy_copy_draw_commands(model_loaders), draw_command_buffer);
        std::println(
            "Copied {} bytes (draw commands) from host to gpu",
            draw_command_buffer.size()
        );
    }
    else
    {
        staging_stream.record(
            kiln::gfx::renderer::StagingRequest{
                lazy_copy_draw_commands(model_loaders),
                draw_command_buffer,
            }
        );
        std::println(
            "Staged {} bytes (draw commands) from host to gpu",
            draw_command_buffer.size()
        );
    }
}

auto stage_models(
    kiln::gfx::renderer::Allocator&     allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    const std::span<GltfModelLoader>    model_loaders,
    kiln::gfx::renderer::Buffer&        geometry_buffer,
    kiln::gfx::renderer::Buffer&        material_buffer,
    kiln::gfx::renderer::Buffer&        instance_buffer,
    kiln::gfx::renderer::Buffer&        draw_command_buffer
) -> void
{
    if (geometry_buffer.size() != 0)
    {
        stage_geometry_buffer(allocator, staging_stream, model_loaders, geometry_buffer);
    }
    if (material_buffer.size() != 0)
    {
        stage_material_buffer(allocator, staging_stream, model_loaders, material_buffer);
    }
    if (instance_buffer.size() != 0)
    {
        stage_instance_buffer(allocator, staging_stream, model_loaders, instance_buffer);
    }
    if (draw_command_buffer.size() != 0)
    {
        stage_draw_commands(allocator, staging_stream, model_loaders, draw_command_buffer);
    }
}

auto load_scene(
    const std::filesystem::path&        model_path,
    const kiln::gfx::renderer::Device&  device,
    kiln::gfx::renderer::Allocator&     gpu_allocator,
    kiln::gfx::asset::gltf::Parser&     model_parser,
    kiln::gfx::renderer::StagingStream& staging_stream,
    std::pmr::memory_resource&          transient_memory_resource
) -> Scene
{
    kiln::gfx::renderer::Buffer geometry_buffer;
    kiln::gfx::renderer::Buffer material_buffer;
    kiln::gfx::renderer::Buffer instance_buffer;
    kiln::gfx::renderer::Buffer draw_command_buffer;
    uint32_t                    max_draw_count{};

    {
        const auto model{
            model_parser.load(model_path, true)
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

        GltfModelLoader model_loader{
            std::allocator_arg,
            &transient_memory_resource,
            model,
            model.defaultScene.value_or(
                kiln::util::Lazy{
                    [&model_path, &model] -> std::size_t
                    {
                        if (model.scenes.empty())
                        {
                            throw std::runtime_error{
                                std::format(
                                    "The provided glTF asset ({}) is a library"
                                    " (it has no scenes)"
                                    " and therefor cannot be loaded directly.",
                                    model_path.generic_string()
                                ),
                            };
                        }
                        return 0;
                    },
                }
            ),
        };

        geometry_buffer     = create_geometry_buffer(gpu_allocator, model_loader);
        material_buffer     = create_material_buffer(gpu_allocator, model_loader);
        instance_buffer     = create_instance_buffer(gpu_allocator, model_loader);
        draw_command_buffer = create_draw_command_buffer(gpu_allocator, model_loader);
        max_draw_count      = model_loader.draw_command_count();

        stage_models(
            gpu_allocator,
            staging_stream,
            std::span{ &model_loader, 1 },
            geometry_buffer,
            material_buffer,
            instance_buffer,
            draw_command_buffer
        );

        gpu_allocator.try_flush(geometry_buffer);
        gpu_allocator.try_flush(material_buffer);
        gpu_allocator.try_flush(instance_buffer);
        gpu_allocator.try_flush(draw_command_buffer);

        if (!staging_stream.empty())
        {
            staging_stream.flush(gpu_allocator);
        }
    }

    staging_stream.reset(device);

    return Scene{
        device,
        std::move(geometry_buffer),
        std::move(material_buffer),
        std::move(instance_buffer),
        std::move(draw_command_buffer),
        draw_command_count_size(),
        max_draw_count,
    };
}

}   // namespace demo
