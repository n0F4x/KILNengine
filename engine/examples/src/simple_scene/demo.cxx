module;

#include <filesystem>
#include <print>

#include <vk_mem_alloc.h>

module examples.simple_scene;

import vulkan_hpp;

import kiln.gfx.asset.gltf.Asset;
import kiln.gfx.asset.gltf.Parser;
import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.device.QueueType;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.memory.Buffer;
import kiln.gfx.renderer.stream.StagingRegion;
import kiln.gfx.vulkan.InstanceBuilder;

namespace demo {

[[nodiscard]]
auto mega_buffer_size_from(const kiln::gfx::asset::gltf::Asset& asset) -> uint32_t
{
    uint32_t result{};

    for (const auto& buffer_view : asset.vertex_array_views())
    {
        result += buffer_view.byte_length;
    }
    for (const auto& buffer_view : asset.index_array_views())
    {
        result += buffer_view.byte_length;
    }

    return result;
}

auto record_staging_transfers(
    kiln::gfx::renderer::StagingStream& staging_stream,
    const kiln::gfx::asset::gltf::Asset&,
    kiln::gfx::renderer::Buffer& final_buffer
) -> void
{
    const std::vector<std::byte> dummy_data{ final_buffer.size() };
    staging_stream.record(kiln::gfx::renderer::StagingRegion{ dummy_data, final_buffer });
}

[[nodiscard]]
// ReSharper disable once CppNotAllPathsReturnValue
auto select_staging_queue(const kiln::gfx::renderer::QueueType queue_type)
    -> std::optional<uint32_t>
{
    switch (queue_type)
    {
        case kiln::gfx::renderer::QueueType::eGraphics:             return 0;
        case kiln::gfx::renderer::QueueType::eHostToDeviceTransfer: return 1;
    }
}

Context::Context(
    kiln::app::MemoryArena&             memory_arena,
    const kiln::gfx::renderer::Device&  gpu_device,
    kiln::gfx::renderer::QueueProvider& gpu_queue_provider,
    kiln::gfx::renderer::Allocator&     gpu_allocator,
    kiln::gfx::asset::gltf::Parser&     gltf_parser
)
    : m_gpu{ gpu_device },
      m_gpu_allocator{ gpu_allocator },
      m_staging_stream{
          std::allocator_arg,
          memory_arena.pool_allocator(),
          gpu_device,
          *gpu_queue_provider.select_transfer_queue(select_staging_queue),
      },
      m_gltf_parser{ gltf_parser }
{
}

auto Context::run(const std::filesystem::path& model_filepath) -> void
{
    const std::optional asset{ m_gltf_parser.get().load(model_filepath) };
    if (asset.has_value())
    {
        std::println("Model loaded from {}", model_filepath.generic_string());
    }

    [[maybe_unused]]
    constexpr static vk::BufferUsageFlags2CreateInfo buffer_usage_flags{
        .usage = vk::BufferUsageFlagBits2::eTransferDst
               | vk::BufferUsageFlagBits2::eStorageBuffer,
    };
    const vk::BufferCreateInfo buffer_create_info{
        .pNext       = &buffer_usage_flags,
        .size        = mega_buffer_size_from(*asset),
        .sharingMode = vk::SharingMode::eExclusive,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    kiln::gfx::renderer::Buffer buffer{
        m_gpu_allocator.get().create_buffer(buffer_create_info, allocation_create_info)
    };

    record_staging_transfers(m_staging_stream, *asset, buffer);
    m_staging_stream.flush(m_gpu_allocator);
    m_staging_stream.reset(m_gpu);
}

auto Context::Builder::create(
    kiln::gfx::vulkan::InstanceBuilder& instance_builder,
    kiln::gfx::renderer::DeviceBuilder& device_builder
) -> Builder
{
    instance_builder.target_api_version(vk::ApiVersion14);
    device_builder.enable_features(
        vk::PhysicalDeviceVulkan14Features{ .maintenance5 = vk::True }
    );
    device_builder.request_queue(kiln::gfx::renderer::QueueType::eGraphics);

    return Builder{};
}

auto Context::Builder::build(
    kiln::app::MemoryArena&             memory_arena,
    const kiln::gfx::renderer::Device&  gpu_device,
    kiln::gfx::renderer::QueueProvider& gpu_queue_provider,
    kiln::gfx::renderer::Allocator&     gpu_allocator,
    const kiln::gfx::renderer::PresentationContext&,
    kiln::gfx::asset::gltf::Parser& gltf_parser
) -> Context
{
    return Context{
        memory_arena, gpu_device, gpu_queue_provider, gpu_allocator, gltf_parser
    };
}

}   // namespace demo
