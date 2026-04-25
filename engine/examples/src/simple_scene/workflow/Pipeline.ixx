module;

#include <cstdint>

export module examples.simple_scene.workflow.Pipeline;

import vulkan_hpp;

import kiln.app.context.ContextBuilderInterface;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.pipeline.GraphicsPipeline;
import kiln.gfx.renderer.pipeline.ShaderModule;
import kiln.gfx.renderer.pipeline.PipelineContext;
import kiln.gfx.renderer.presentation.RenderSurface;
import kiln.gfx.renderer.presentation.PresentationContext;
import kiln.gfx.vulkan.Instance;

namespace demo {

export class Pipeline {
public:
    explicit Pipeline(
        const kiln::gfx::renderer::Device&        device,
        const kiln::gfx::renderer::RenderSurface& surface,
        uint8_t                                   number_of_frames_inf_flight
    );


private:
    vk::raii::PipelineLayout              m_graphics_pipeline_layout;
    kiln::gfx::renderer::ShaderModule     m_graphics_shader_module;
    kiln::gfx::renderer::GraphicsPipeline m_graphics_pipeline;
};

}   // namespace demo
