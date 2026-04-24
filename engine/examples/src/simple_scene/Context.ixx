module;

#include <filesystem>
#include <functional>

export module examples.simple_scene.Context;

import kiln.app.App;
import kiln.app.Builder;
import kiln.app.config.Config;
import kiln.app.context.ContextBuilderInterface;
import kiln.app.memory.MemoryArena;
import kiln.gfx.asset.gltf.Parser;
import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.stream.StagingStream;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.presentation.PresentationContext;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.InstanceBuilder;
import kiln.wsi.Context;
import kiln.wsi.Window;

import examples.simple_scene.workflow.Renderer;
import examples.simple_scene.Window;

namespace demo {

export class Context {
public:
    class Builder;

    explicit Context(
        kiln::app::MemoryArena&             memory_arena,
        const kiln::gfx::renderer::Device&  render_device,
        kiln::gfx::renderer::QueueProvider& gpu_queue_provider,
        kiln::gfx::renderer::Allocator&     gpu_allocator,
        kiln::gfx::asset::gltf::Parser&     gltf_parser,
        Window&                             window,
        Renderer&                           renderer
    );

    auto run(const std::filesystem::path& model_filepath) -> void;

private:
    std::reference_wrapper<const kiln::gfx::renderer::Device> m_gpu;
    std::reference_wrapper<kiln::gfx::renderer::Allocator>    m_gpu_allocator;
    kiln::gfx::renderer::StagingStream                        m_staging_stream;
    std::reference_wrapper<kiln::gfx::asset::gltf::Parser>    m_gltf_parser;
    std::reference_wrapper<Window>                            m_window;
    std::reference_wrapper<Renderer>                          m_renderer;
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
        kiln::app::MemoryArena&             memory_arena,
        const kiln::gfx::renderer::Device&  render_device,
        kiln::gfx::renderer::QueueProvider& gpu_queue_provider,
        kiln::gfx::renderer::Allocator&     gpu_allocator,
        kiln::gfx::asset::gltf::Parser&     gltf_parser,
        Window&                             window,
        Renderer&                           renderer
    ) -> Context;
};

}   // namespace demo
