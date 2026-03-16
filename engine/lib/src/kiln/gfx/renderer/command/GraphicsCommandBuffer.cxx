module;

#include <utility>

module kiln.gfx.renderer.command.GraphicsCommandBuffer;

namespace kiln::gfx::renderer {

GraphicsCommandBuffer::GraphicsCommandBuffer(
    vk::raii::CommandBuffer&&                     command_buffer,
    const util::EnumMask<CommandBufferUsageFlags> usage_flags
)
    : TransferCommandBuffer{ std::move(command_buffer), usage_flags }
{
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto GraphicsCommandBuffer::begin_rendering(const RenderPass& render_pass) -> void
{
    get().beginRendering(render_pass.get());

    const vk::Viewport viewport{
        .x        = static_cast<float>(render_pass.get().renderArea.offset.x),
        .y        = static_cast<float>(render_pass.get().renderArea.offset.y),
        .width    = static_cast<float>(render_pass.get().renderArea.extent.width),
        .height   = static_cast<float>(render_pass.get().renderArea.extent.height),
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };
    get().setViewport(0, viewport);

    const vk::Rect2D scissor{
        .offset = render_pass.get().renderArea.offset,
        .extent = render_pass.get().renderArea.extent,
    };
    get().setScissor(0, scissor);
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto GraphicsCommandBuffer::end_rendering() -> void
{
    get().endRendering();
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto GraphicsCommandBuffer::bind_pipeline(const GraphicsPipeline& pipeline) -> void
{
    get().bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto GraphicsCommandBuffer::draw(const uint32_t vertex_count) -> void
{
    get().draw(vertex_count, 1, 0, 0);
}

}   // namespace kiln::gfx::renderer
