module;

#include <filesystem>
#include <functional>
#include <memory_resource>

export module examples.frustum_culling.Context;

import kiln.app.App;
import kiln.app.config.Config;
import kiln.reg.BuildableEntry;
import kiln.reg.BuildDirector;
import kiln.event.EventBuffer;
import kiln.event.EventRecorder;
import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.stream.StagingStream;
import kiln.gfx.renderer.presentation.RenderSurface;
import kiln.wsi.event.Event;
import kiln.wsi.WindowProxy;

import examples.frustum_culling.workflow.Renderer;
import examples.frustum_culling.workflow.Scene;

namespace demo {

export class Context;

auto describe_builder(kiln::reg::BuildDirector<Context>& build_director) -> void;

export class Context : public kiln::reg::BuildableEntry<Context, describe_builder> {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    Context(Context&&, const allocator_type&);

    explicit Context(
        const kiln::gfx::renderer::Device&  render_device,
        kiln::gfx::renderer::QueueProvider& render_queue_provider
    );
    explicit Context(
        std::allocator_arg_t,
        const allocator_type&               allocator,
        const kiln::gfx::renderer::Device&  render_device,
        kiln::gfx::renderer::QueueProvider& render_queue_provider
    );

    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

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

}   // namespace demo
