module;

#include <span>

export module kiln.gfx.renderer.pipeline.GraphicsPipeline;

import vulkan_hpp;

import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.pipeline.ShaderModule;

namespace kiln::gfx::renderer {

export class GraphicsPipeline {
public:
    GraphicsPipeline(
        const Device&                   device,
        const vk::raii::PipelineLayout& layout,
        const ShaderModule&             vertex_shader_module,
        const ShaderModule&             fragment_shader_module,
        std::span<const vk::Format>     color_formats = {},
        vk::Format                      depth_format  = vk::Format::eUndefined
    );

    [[nodiscard]]
    auto get() const noexcept -> const vk::raii::Pipeline&;

private:
    vk::raii::Pipeline m_pipeline;
};

}   // namespace kiln::gfx::renderer
