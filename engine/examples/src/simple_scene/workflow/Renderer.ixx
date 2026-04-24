module;

#include <cstdint>

export module examples.simple_scene.workflow.Renderer;

import vulkan_hpp;

import kiln.app.context.ContextBuilderInterface;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.pipeline.GraphicsPipeline;
import kiln.gfx.renderer.pipeline.ShaderModule;
import kiln.gfx.renderer.pipeline.PipelineContext;
import kiln.gfx.renderer.presentation.RenderSurface;
import kiln.gfx.renderer.presentation.PresentationContext;
import kiln.gfx.vulkan.Instance;

import examples.simple_scene.Window;

namespace demo {

export class Renderer {
public:
    class Builder;

    explicit Renderer(
        const kiln::gfx::vulkan::Instance& vulkan_instance,
        const kiln::gfx::renderer::Device& device,
        Window&                            window
    );

private:
    uint8_t                               m_number_of_frames{ 2 };
    kiln::gfx::renderer::RenderSurface    m_surface;
    vk::raii::PipelineLayout              m_graphics_pipeline_layout;
    kiln::gfx::renderer::ShaderModule     m_graphics_shader_module;
    kiln::gfx::renderer::GraphicsPipeline m_graphics_pipeline;
};

class Renderer::Builder : public kiln::app::ContextBuilderInterface {
public:
    static auto build(
        const kiln::gfx::vulkan::Instance&              vulkan_instance,
        const kiln::gfx::renderer::Device&              device,
        Window&                                         window,
        const kiln::gfx::renderer::PresentationContext& presentation_context,
        const kiln::gfx::renderer::PipelineContext&     pipeline_context
    ) -> Renderer;
};

}   // namespace demo
