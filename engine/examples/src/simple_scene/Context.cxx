module;

#include <atomic>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <future>
#include <memory_resource>
#include <print>
#include <ranges>
#include <thread>

#include <vk_mem_alloc.h>

module examples.simple_scene.Context;

import vulkan_hpp;

import kiln.event.Timestamp;
import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.device.QueueType;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.InstanceBuilder;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.containers.MoveOnlyFunction;
import kiln.util.ScopeFail;
import kiln.wsi.Engine;
import kiln.wsi.event.Event;
import kiln.wsi.event.events;
import kiln.wsi.event.EventType;
import kiln.wsi.event.Key;
import kiln.wsi.EventConsumerQueueInterface;
import kiln.wsi.WindowCommand;
import kiln.wsi.WindowedWindowSettings;
import kiln.wsi.WindowHandle;
import kiln.wsi.WindowProxy;

import examples.simple_scene.Camera;
import examples.simple_scene.SPSCQueue;
import examples.simple_scene.workflow.load_scene;
import examples.simple_scene.workflow.Renderer;
import examples.simple_scene.workflow.Scene;

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
    const kiln::app::Config&            config,
    kiln::app::MemoryArena&             memory_arena,
    const kiln::gfx::renderer::Device&  render_device,
    kiln::gfx::renderer::QueueProvider& render_queue_provider
)
    : m_app_config{ config },
      m_render_device{ render_device },
      m_render_queue_provider{ render_queue_provider },
      m_staging_stream{
          std::allocator_arg,
          memory_arena.pool_allocator(),
          render_device,
          *render_queue_provider.select_transfer_queue(select_staging_queue),
      }
{
}

[[nodiscard]]
auto aspect_ratio_from(const vk::Extent2D frame_buffer_size) -> double
{
    return static_cast<double>(frame_buffer_size.width)
         / static_cast<double>(frame_buffer_size.height);
}

class Context::EventQueue : public kiln::wsi::EventConsumerQueueInterface {
public:
    using value_type = std::pair<kiln::wsi::Event, kiln::event::Timestamp>;

    auto push(kiln::wsi::Event&& event, kiln::event::Timestamp timestamp) -> void override
    {
        [[maybe_unused]]
        const bool success = m_concurrent_queue.try_emplace(std::move(event), timestamp);
        assert(success);
    }

    template <typename F>
    auto pop_all(F&& callback) -> void
    {
        m_concurrent_queue.pop_all(
            [&callback](const auto popped_view) -> void
            {
                for (value_type&& value : popped_view)
                {
                    std::apply(callback, std::move(value));
                }
            }
        );
    }

private:
    SPSCQueue<value_type> m_concurrent_queue{ max_enqueued_items };
};

struct Context::MainThread {
    using Task = kiln::util::MoveOnlyFunction<auto(MainThread&) &&->void>;

    EventQueue        event_queue;
    kiln::wsi::Engine wsi_engine{ event_queue };
    SPSCQueue<Task>   work_queue{ max_enqueued_items };
};

auto Context::run_main_thread_loop(
    const std::atomic_bool& running,
    MainThread&             main_thread
) -> void
{
    while (running)
    {
        main_thread.wsi_engine.wait_events();

        for (std::optional<MainThread::Task> task{ main_thread.work_queue.pop() };
             task.has_value();
             task = main_thread.work_queue.pop())
        {
            std::move (*task)(main_thread);
        }
    }
}

auto Context::run(
    kiln::app::App&              app,
    const std::filesystem::path& model_filepath,
    const bool                   limit_fps
) -> void
{
    std::atomic_bool running{ true };
    MainThread       main_thread;

    std::jthread render_thread{
        [&app, &model_filepath, limit_fps, this, &running, &main_thread] -> void
        {
            run_worker_thread(app, running, main_thread, model_filepath, limit_fps);   //
        },
    };
    run_main_thread_loop(running, main_thread);

    render_thread.join();
}

auto Context::create_window(const kiln::app::Config& config, MainThread& main_thread)
    -> kiln::wsi::WindowProxy
{
    std::promise<kiln::wsi::WindowProxy> window_promise;

    [[maybe_unused]]
    const bool success = main_thread.work_queue.try_push(
        MainThread::Task{
            [title = config.app_name(),
             &window_promise](const MainThread& u_main_thread) -> void
            {
                const kiln::util::ScopeFail failure_guard{
                    [&window_promise, &u_main_thread] noexcept -> void
                    {
                        window_promise.set_value(
                            kiln::wsi::WindowProxy{
                                u_main_thread.wsi_engine.context(),
                                kiln::wsi::WindowHandle{ nullptr },
                            }
                        );
                    },
                };

                constexpr static kiln::wsi::WindowedWindowSettings screen_settings{
                    .content_size{ .width = 640, .height = 480 }
                };

                window_promise.set_value(
                    kiln::wsi::WindowProxy{
                        u_main_thread.wsi_engine.context(),
                        u_main_thread.wsi_engine.create_window(title, screen_settings),
                    }
                );
            },
        }
    );
    assert(success);

    main_thread.wsi_engine.post_empty_event();

    return window_promise.get_future().get();
}

auto Context::run_worker_thread(
    kiln::app::App&              app,
    std::atomic_bool&            running,
    MainThread&                  main_thread,
    const std::filesystem::path& model_filepath,
    const bool                   limit_fps
) -> void
{
    const auto& vulkan_instance{ app.contexts().at<kiln::gfx::vulkan::Instance>() };
    const auto& render_device{ app.contexts().at<kiln::gfx::renderer::Device>() };
    auto&       render_allocator{ app.contexts().at<kiln::gfx::renderer::Allocator>() };
    auto&       gltf_parser{ app.contexts().at<kiln::gfx::asset::gltf::Parser>() };

    const auto  scene_load_start_time{ std::chrono::steady_clock::now() };
    const Scene scene = load_scene(
        model_filepath,
        render_device,
        render_allocator,
        gltf_parser,
        m_staging_stream,
        *std::pmr::get_default_resource()
    );
    const auto scene_load_finish_time{ std::chrono::steady_clock::now() };
    std::println(
        "Loading the scene took {}",
        scene_load_finish_time - scene_load_start_time
    );


    kiln::wsi::WindowProxy window{ create_window(m_app_config, main_thread) };
    if (window == kiln::wsi::WindowHandle{ nullptr })
    {
        running = false;
        return;
    }
    kiln::gfx::renderer::RenderSurface render_surface{
        kiln::gfx::vulkan::check_result(
            window.create_vulkan_surface(vulkan_instance.get())
        ),
        render_device,
        std::max(number_of_frames_in_flight, 3u),
        true,
        window.framebuffer_size(),
    };
    Renderer renderer{
        m_render_device,
        number_of_frames_in_flight,
        render_surface.surface_format().format,
        render_surface.number_of_images(),
    };


    run_render_loop(
        running,
        main_thread,
        scene,
        window,
        render_surface,
        renderer,
        limit_fps
    );

    m_render_device.get().logical_device().waitIdle();
}

auto Context::run_render_loop(
    std::atomic_bool&                   running,
    MainThread&                         main_thread,
    const Scene&                        scene,
    kiln::wsi::WindowProxy&             window,
    kiln::gfx::renderer::RenderSurface& render_surface,
    Renderer&                           renderer,
    const bool                          limit_fps
) -> void
{
    using std::chrono_literals::operator""ms;
    constexpr static auto target_frame_duration = 1'000ms / 60;

    const Camera camera{ aspect_ratio_from(*render_surface.extent()) };

    while (running)
    {
        const auto frame_start_time{ std::chrono::steady_clock::now() };


        main_thread.event_queue.pop_all(
            [this](kiln::wsi::Event&& event, const kiln::event::Timestamp timestamp) -> void
            {
                m_wsi_event_recorder.record(std::move(event), timestamp);   //
            }
        );

        m_wsi_event_buffer.clear();
        m_wsi_event_recorder.flush(m_wsi_event_buffer);

        for (const auto& event : m_wsi_event_buffer | std::views::keys)
        {
            window.update(event);
        }
        window.flush_changes(
            [&](auto window_commands) -> void
            {
                [[maybe_unused]]
                const bool success_count = main_thread.work_queue.try_append_range(
                    window_commands
                    | std::views::transform(
                        [](kiln::wsi::WindowCommand&& window_command) -> MainThread::Task
                        {
                            return MainThread::Task{
                                [x_window_command = std::move(window_command)](
                                    const MainThread& u_main_thread
                                ) mutable -> void
                                {
                                    std::move(x_window_command)(
                                        u_main_thread.wsi_engine.context()
                                    );
                                },
                            };
                        }
                    )
                );
                assert(success_count == std::ranges::size(window_commands));
            }
        );
        if (window.should_close())
        {
            running = false;
            main_thread.wsi_engine.post_empty_event();
        }

        if (window.framebuffer_size() != render_surface.extent())
        {
            m_render_device.get().logical_device().waitIdle();
            render_surface.resize(window.framebuffer_size());
        }


        renderer.render(
            m_render_device,
            *m_render_queue_provider.get().graphics_queue(),
            render_surface,
            scene,
            camera,
            *std::pmr::get_default_resource()
        );


        const auto frame_finish_time{ std::chrono::steady_clock::now() };
        if (const auto frame_duration{ frame_finish_time - frame_start_time };
            limit_fps && frame_duration < target_frame_duration)
        {
            std::this_thread::sleep_for(target_frame_duration - frame_duration);
        }
    }
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
    const kiln::app::Config& config,
    kiln::app::MemoryArena&  memory_arena,
    const kiln::gfx::vulkan::Instance&,
    const kiln::wsi::Context&,
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
        render_device,
        gpu_queue_provider,
    };
}

}   // namespace demo
