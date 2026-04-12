module;

#include <filesystem>
#include <functional>

export module simple_scene;

import kiln.app.App;
import kiln.app.context.ContextBuilderInterface;
import kiln.app.Builder;
import kiln.gfx.asset.gltf.Parser;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.memory.StreamingService;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.presentation.PresentationContext;
import kiln.gfx.vulkan.InstanceBuilder;

namespace demo {

export class Demo {
public:
    class Builder;

    explicit Demo(
        kiln::gfx::renderer::Allocator&        gpu_allocator,
        kiln::gfx::renderer::StreamingService& gpu_streaming_service,
        kiln::gfx::asset::gltf::Parser&        gltf_parser
    );

    auto run(const std::filesystem::path& model_filepath) const -> void;

private:
    std::reference_wrapper<kiln::gfx::renderer::Allocator>        m_gpu_allocator;
    std::reference_wrapper<kiln::gfx::renderer::StreamingService> m_gpu_streaming_service;
    std::reference_wrapper<kiln::gfx::asset::gltf::Parser>        m_gltf_parser;
};

class Demo::Builder : public kiln::app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto create(
        kiln::gfx::vulkan::InstanceBuilder& instance_builder,
        kiln::gfx::renderer::DeviceBuilder& device_builder
    ) -> Builder;

    [[nodiscard]]
    static auto build(
        kiln::gfx::renderer::Allocator&                 gpu_allocator,
        const kiln::gfx::renderer::PresentationContext& presentation_context,
        kiln::gfx::asset::gltf::Parser&                 gltf_parser
    ) -> Demo;
};

}   // namespace demo
