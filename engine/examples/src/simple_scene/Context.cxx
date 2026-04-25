module;

#include <filesystem>
#include <print>
#include <span>

#include <vk_mem_alloc.h>

module examples.simple_scene.Context;

import vulkan_hpp;

import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.device.QueueType;
import kiln.gfx.vulkan.InstanceBuilder;

import examples.simple_scene.workflow.load_scene;

namespace demo {

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
    const kiln::gfx::renderer::Device&  render_device,
    kiln::gfx::renderer::QueueProvider& gpu_queue_provider,
    kiln::gfx::renderer::Allocator&     gpu_allocator,
    kiln::gfx::asset::gltf::Parser&     gltf_parser,
    Window&                             window,
    Renderer&                           renderer
)
    : m_gpu{ render_device },
      m_gpu_allocator{ gpu_allocator },
      m_staging_stream{
          std::allocator_arg,
          memory_arena.pool_allocator(),
          render_device,
          *gpu_queue_provider.select_transfer_queue(select_staging_queue),
      },
      m_gltf_parser{ gltf_parser },
      m_window{ window },
      m_renderer{ renderer }
{
}

auto Context::run(const std::filesystem::path& model_filepath) -> void
{
    [[maybe_unused]]
    auto scene = load_scene(
        model_filepath, m_gpu, m_gpu_allocator, m_gltf_parser, m_staging_stream
    );
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
    device_builder.enable_features(
        vk::PhysicalDeviceVulkan12Features{ .bufferDeviceAddress = vk::True }
    );
    device_builder.request_queue(kiln::gfx::renderer::QueueType::eGraphics);

    return Builder{};
}

auto Context::Builder::build(
    kiln::app::MemoryArena&             memory_arena,
    const kiln::gfx::renderer::Device&  render_device,
    kiln::gfx::renderer::QueueProvider& gpu_queue_provider,
    kiln::gfx::renderer::Allocator&     gpu_allocator,
    kiln::gfx::asset::gltf::Parser&     gltf_parser,
    Window&                             window,
    Renderer&                           renderer
) -> Context
{
    return Context{
        memory_arena,
        render_device,   //
        gpu_queue_provider,
        gpu_allocator,
        gltf_parser,   //
        window,
        renderer,
    };
}

}   // namespace demo
