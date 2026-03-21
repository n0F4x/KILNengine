module;

#include <cstdint>

export module kiln.gfx.renderer.command.GraphicsCommandBuffer;

import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.gfx.renderer.pipeline.GraphicsPipeline;
import kiln.gfx.renderer.pipeline.RenderPass;

namespace kiln::gfx::renderer {

export class GraphicsCommandBuffer : public TransferCommandBuffer {
public:
    using TransferCommandBuffer::TransferCommandBuffer;

    auto begin_render_pass(const RenderPass& render_pass) -> void;
    auto end_render_pass() -> void;

    auto bind_pipeline(const GraphicsPipeline& pipeline) -> void;

    auto draw(uint32_t vertex_count) -> void;
};

}   // namespace kiln::gfx::renderer
