module;

#include <atomic>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory_resource>
#include <print>
#include <thread>

#include <vk_mem_alloc.h>

module examples.simple_scene.Context;

import vulkan_hpp;

import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.device.QueueType;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.InstanceBuilder;
import kiln.gfx.vulkan.result.check_result;
import kiln.wsi.event.Key;
import kiln.wsi.event.wait_events;
import kiln.wsi.WindowedWindowSettings;

import examples.simple_scene.workflow.load_scene;
import examples.simple_scene.workflow.Renderer;

namespace demo {

[[nodiscard]]
auto create_window(const kiln::app::Config& config, const kiln::wsi::Context& wsi_context)
    -> kiln::wsi::Window
{
    constexpr static kiln::wsi::WindowedWindowSettings screen_settings{
        .content_size{ .width = 640, .height = 480 }
    };
    const kiln::wsi::Window::CreateInfo window_info{
        .title    = config.app_name(),
        .settings = screen_settings,
    };

    return kiln::wsi::Window{ wsi_context, window_info };
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
    const kiln::app::Config&            config,
    kiln::app::MemoryArena&             memory_arena,
    const kiln::gfx::vulkan::Instance&  vulkan_instance,
    const kiln::wsi::Context&           wsi_context,
    const kiln::gfx::renderer::Device&  render_device,
    kiln::gfx::renderer::QueueProvider& gpu_queue_provider
)
    : m_staging_stream{
          std::allocator_arg,
          memory_arena.pool_allocator(),
          render_device,
          *gpu_queue_provider.select_transfer_queue(select_staging_queue),
      },
      m_window{ create_window(config, wsi_context) },
      m_render_surface{
          kiln::gfx::vulkan::check_result(
              m_window.create_vulkan_surface(vulkan_instance.get())
          ),
          render_device,
          number_of_frames_in_flight,
          true,
          m_window.resolution(),
      }
{
}

auto Context::run(kiln::app::App& app, const std::filesystem::path& model_filepath)
    -> void
{
    auto&       memory_arena{ app.contexts().at<kiln::app::MemoryArena>() };
    const auto& wsi_context{ app.contexts().at<kiln::wsi::Context>() };
    const auto& render_device{ app.contexts().at<kiln::gfx::renderer::Device>() };
    auto& render_queue_provider{ app.contexts().at<kiln::gfx::renderer::QueueProvider>() };
    auto& render_allocator{ app.contexts().at<kiln::gfx::renderer::Allocator>() };
    auto& gltf_parser{ app.contexts().at<kiln::gfx::asset::gltf::Parser>() };

    [[maybe_unused]]
    auto scene = load_scene(
        model_filepath,
        render_device,
        render_allocator,
        gltf_parser,
        m_staging_stream
    );

    Renderer renderer{
        std::allocator_arg,
        memory_arena.pool_allocator(),
        render_device,
        number_of_frames_in_flight,
        m_render_surface.surface_format().format,
        m_render_surface.number_of_images(),   //
    };

    std::atomic_bool running{ true };
    std::atomic_bool window_resized{ false };
    std::atomic      window_resolution{ m_window.resolution() };

    std::jthread render_thread{
        [this,
         &render_device,
         &render_queue_provider,
         &scene,
         &renderer,
         &running,
         &window_resized,
         &window_resolution] mutable -> void
        {
            using namespace std::chrono_literals;
            constexpr static auto target_frame_duration = 1'000ms / 60;

            while (running)
            {
                const auto frame_start_time{ std::chrono::steady_clock::now() };


                if (window_resized.exchange(false))
                {
                    if (const auto new_window_resolution{ window_resolution.load() };
                        m_render_surface.extent() != new_window_resolution)
                    {
                        render_device.logical_device().waitIdle();
                        m_render_surface.resize(new_window_resolution);
                    }
                }

                renderer.render(
                    render_device,
                    *render_queue_provider.graphics_queue(),
                    m_render_surface,
                    scene,
                    *std::pmr::get_default_resource()
                );


                const auto frame_finish_time{ std::chrono::steady_clock::now() };
                if (const auto frame_duration{ frame_finish_time - frame_start_time };
                    frame_duration < target_frame_duration)
                {
                    std::this_thread::sleep_for(target_frame_duration - frame_duration);
                }
            }
        }
    };

    while (!m_window.should_close())
    {
        kiln::wsi::wait_events(wsi_context);

        if (m_window.key_pressed(kiln::wsi::Key::eEscape))
        {
            m_window.request_close();
        }

        // TODO: only do this on a window resize event
        window_resolution = m_window.resolution();
        window_resized    = true;
    }

    running = false;
    render_thread.join();

    render_device.logical_device().waitIdle();
}

auto Context::Builder::create(
    kiln::gfx::vulkan::InstanceBuilder& instance_builder,
    kiln::gfx::renderer::DeviceBuilder& device_builder
) -> Builder
{
    instance_builder.target_api_version(vk::ApiVersion14);
    device_builder.enable_features(
        vk::PhysicalDeviceFeatures{
            .multiDrawIndirect = vk::True,
        }
    );
    device_builder.enable_features(
        vk::PhysicalDeviceVulkan12Features{
            .drawIndirectCount    = vk::True,
            .storagePushConstant8 = vk::True,
            .shaderInt8           = vk::True,
            .scalarBlockLayout    = vk::True,
            .bufferDeviceAddress  = vk::True,
        }
    );
    device_builder.enable_features(
        vk::PhysicalDeviceVulkan14Features{ .maintenance5 = vk::True }
    );
    device_builder.request_queue(kiln::gfx::renderer::QueueType::eGraphics);

    return Builder{};
}

auto Context::Builder::build(
    const kiln::app::Config&            config,
    kiln::app::MemoryArena&             memory_arena,
    const kiln::gfx::vulkan::Instance&  vulkan_instance,
    const kiln::wsi::Context&           wsi_context,
    const kiln::gfx::renderer::Device&  render_device,
    kiln::gfx::renderer::QueueProvider& gpu_queue_provider,
    kiln::gfx::renderer::Allocator&,
    const kiln::gfx::renderer::PipelineContext&,
    const kiln::gfx::renderer::PresentationContext&,
    kiln::gfx::asset::gltf::Parser&
) -> Context
{
    return Context{
        config,
        memory_arena,
        vulkan_instance,
        wsi_context,
        render_device,
        gpu_queue_provider,   //
    };
}

}   // namespace demo
