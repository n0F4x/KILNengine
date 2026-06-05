module;

#include <filesystem>
#include <functional>

export module examples.frustum_culling.Context;

import kiln.app.App;
import kiln.app.Builder;
import kiln.app.config.Config;
import kiln.app.context.ContextBuilderInterface;
import kiln.app.memory.MemoryArena;
import kiln.event.EventBuffer;
import kiln.event.EventRecorder;
import kiln.gfx.asset.gltf.Parser;
import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.stream.StagingStream;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.pipeline.PipelineContext;
import kiln.gfx.renderer.presentation.PresentationContext;
import kiln.gfx.renderer.presentation.RenderSurface;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.InstanceBuilder;
import kiln.wsi.Context;
import kiln.wsi.Engine;
import kiln.wsi.event.Event;
import kiln.wsi.WindowProxy;

import examples.frustum_culling.SPSCQueue;
import examples.frustum_culling.workflow.Renderer;
import examples.frustum_culling.workflow.Scene;

namespace demo {

export class Context {
public:
    class Builder;

    explicit Context(
        kiln::app::MemoryArena&             memory_arena,
        const kiln::gfx::renderer::Device&  render_device,
        kiln::gfx::renderer::QueueProvider& render_queue_provider
    );

    auto run(
        kiln::app::App&              app,
        const std::filesystem::path& model_filepath,
        bool                         limit_fps,
        bool                         disable_culling,
        uint32_t                     grid_size
    ) -> void;

private:
    struct MainThread;
    class EventQueue;


    constexpr static uint32_t number_of_frames_in_flight{ 2 };
    constexpr static uint32_t max_enqueued_items{ 1'024 };


    kiln::event::EventBuffer<kiln::wsi::Event>   m_wsi_event_buffer;
    kiln::event::EventRecorder<kiln::wsi::Event> m_wsi_event_recorder;
    kiln::gfx::renderer::StagingStream           m_staging_stream;


    auto run_main_worker(
        kiln::app::App&              app,
        std::atomic_bool&            running,
        MainThread&                  main_thread,
        const std::filesystem::path& model_filepath,
        bool                         limit_fps,
        bool                         disable_culling,
        uint32_t                     grid_size
    ) -> void;

    [[nodiscard]]
    static auto create_window(const kiln::app::Config& config, MainThread& main_thread)
        -> kiln::wsi::WindowProxy;

    auto run_render_loop(
        const kiln::app::Config&            config,
        const kiln::gfx::renderer::Device&  render_device,
        kiln::gfx::renderer::QueueProvider& queue_provider,
        kiln::gfx::renderer::Allocator&     render_allocator,
        std::atomic_bool&                   running,
        MainThread&                         main_thread,
        const Scene&                        scene,
        kiln::wsi::WindowProxy&             window,
        kiln::gfx::renderer::RenderSurface& render_surface,
        Renderer&                           renderer,
        bool                                limit_fps,
        double                              movement_speed
    ) -> void;
    static auto run_main_thread_loop(
        const std::atomic_bool& running,
        MainThread&             main_thread
    ) -> void;
};

class Context::Builder : public kiln::app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto create(
        kiln::gfx::vulkan::InstanceBuilder& instance_builder,
        kiln::gfx::renderer::DeviceBuilder& device_builder
    ) -> Builder;

    [[nodiscard]]
    static auto build(
        const kiln::app::Config&                        config,
        kiln::app::MemoryArena&                         memory_arena,
        const kiln::gfx::vulkan::Instance&              vulkan_instance,
        const kiln::wsi::Context&                       wsi_context,
        const kiln::gfx::renderer::Device&              render_device,
        kiln::gfx::renderer::QueueProvider&             gpu_queue_provider,
        kiln::gfx::renderer::Allocator&                 gpu_allocator,
        const kiln::gfx::renderer::PipelineContext&     pipeline_context,
        const kiln::gfx::renderer::PresentationContext& presentation_context,
        kiln::gfx::asset::gltf::Parser&                 gltf_parser
    ) -> Context;
};

}   // namespace demo
