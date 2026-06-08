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
    builder.register_entry<Device>();
    builder.register_entry<QueueProvider>();
    builder.register_entry<Allocator>();
    if (!m_headless)
    {
        builder.register_entry<PresentationContext>();
    }
    builder.register_entry<PipelineContext>();
}

}   // namespace kiln::gfx::renderer
