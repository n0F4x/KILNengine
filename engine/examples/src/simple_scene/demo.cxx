module;

#include <filesystem>
#include <print>

#include <vk_mem_alloc.h>

module simple_scene;

import vulkan_hpp;

import kiln.gfx.asset.gltf.Asset;
import kiln.gfx.asset.gltf.Parser;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.device.QueueType;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.memory.Buffer;
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

Demo::Demo(
    kiln::gfx::renderer::Allocator&        gpu_allocator,
    kiln::gfx::renderer::StreamingService& gpu_streaming_service,
    kiln::gfx::asset::gltf::Parser&        gltf_parser
)
    : m_gpu_allocator{ gpu_allocator },
      m_gpu_streaming_service{ gpu_streaming_service },
      m_gltf_parser{ gltf_parser }
{
}

auto Demo::run(const std::filesystem::path& model_filepath) const -> void
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
}

auto Demo::Builder::create(
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

auto Demo::Builder::build(
    kiln::gfx::renderer::Allocator& gpu_allocator,
    const kiln::gfx::renderer::PresentationContext&,
    kiln::gfx::asset::gltf::Parser& gltf_parser
) -> Demo
{
    return Demo{ gpu_allocator, gltf_parser };
}

}   // namespace demo
