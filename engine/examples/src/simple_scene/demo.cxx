module;

#include <filesystem>
#include <print>

#include <vk_mem_alloc.h>

module simple_scene;

import vulkan_hpp;

import kiln.app.context.ContextBuilderInterface;
import kiln.gfx.asset.gltf.Asset;
import kiln.gfx.asset.gltf.Parser;
import kiln.gfx.Bundle;
import kiln.gfx.renderer.command.QueueProviderBuilder;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.vulkan.InstanceBuilder;

namespace demo {

struct MetaContext {
    struct Builder : kiln::app::ContextBuilderInterface {
        [[nodiscard]]
        static auto create(
            kiln::gfx::vulkan::InstanceBuilder&        instance_builder,
            kiln::gfx::renderer::DeviceBuilder&        device_builder,
            kiln::gfx::renderer::QueueProviderBuilder& queue_provider_builder
        ) -> Builder
        {
            instance_builder.target_api_version(vk::ApiVersion14);
            device_builder.enable_features(
                vk::PhysicalDeviceVulkan14Features{ .maintenance5 = vk::True }
            );
            queue_provider_builder.require_graphics_queue();

            return Builder{};
        }

        [[nodiscard]]
        static auto build() -> MetaContext
        {
            return MetaContext{};
        }
    };
};

auto Bundle::operator()(kiln::app::Builder& builder) -> void
{
    builder.apply_bundle(kiln::gfx::Bundle{});
    builder.use_context<MetaContext>();
}

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

auto run(kiln::app::App& app, const std::filesystem::path& model_filepath) -> void
{
    const std::optional asset{
        app.contexts().at<kiln::gfx::asset::gltf::Parser>().load(model_filepath)
    };
    if (asset.has_value())
    {
        std::println("Model loaded from {}", model_filepath.generic_string());
    }

    const auto& gpu_allocator{ app.contexts().at<kiln::gfx::renderer::Allocator>() };
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
    auto [buffer, allocation, info]{
        gpu_allocator.create_buffer(buffer_create_info, allocation_create_info)
    };
}

}   // namespace demo
