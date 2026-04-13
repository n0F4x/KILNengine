module kiln.gfx.renderer.Bundle;

import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.pipeline.PipelineContext;
import kiln.gfx.renderer.presentation.PresentationContext;

namespace kiln::gfx::renderer {

Bundle::Bundle(const CreateInfo& create_info) : m_headless{ create_info.headless } {}

auto Bundle::operator()(app::Builder& builder) const -> void
{
    builder.use_context<Device>();
    builder.use_context<QueueProvider>();
    builder.use_context<Allocator>();
    if (!m_headless)
    {
        builder.use_context<PresentationContext>();
    }
    builder.use_context<PipelineContext>();
}

}   // namespace kiln::gfx::renderer
