module;

#include <cstdint>

export module kiln.gfx.renderer.command.GraphicsCommandBuffer;

import vulkan_hpp;

import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.gfx.renderer.pipeline.GraphicsPipeline;
import kiln.gfx.renderer.pipeline.RenderPass;

namespace kiln::gfx::renderer {

export class GraphicsCommandBuffer : public TransferCommandBuffer {
public:
    using TransferCommandBuffer::TransferCommandBuffer;

    auto record_render_pass_start(const RenderPass& render_pass) -> void;
    auto record_render_pass_finish() -> void;

    auto record_pipeline_bind(const GraphicsPipeline& pipeline) -> void;
    auto record_push_constants(const vk::PushConstantsInfo& info) -> void;

    auto record_draw(uint32_t vertex_count) -> void;
};

}   // namespace kiln::gfx::renderer
