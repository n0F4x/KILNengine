export module kiln.gfx.renderer.Bundle;

import kiln.app.Builder;
import kiln.gfx.renderer.command.CommandPlugin;
import kiln.gfx.renderer.command.QueueProviderPlugin;
import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.renderer.memory.AllocatorPlugin;
import kiln.gfx.renderer.pipeline.PipelinePlugin;
import kiln.gfx.renderer.presentation.PresentationPlugin;

namespace kiln::gfx::renderer {

export class Bundle {
public:
    struct CreateInfo {
        bool headless{ true };
    };

    Bundle() = default;
    explicit Bundle(const CreateInfo& create_info);

    auto operator()(app::Builder& builder) const -> void;

private:
    bool m_headless;
};

}   // namespace kiln::gfx::renderer

module :private;

namespace kiln::gfx::renderer {

Bundle::Bundle(const CreateInfo& create_info) : m_headless{ create_info.headless } {}

auto Bundle::operator()(app::Builder& builder) const -> void
{
    builder.use_plugin<DevicePlugin>();
    builder.use_meta_plugin<CommandPlugin>();
    builder.use_plugin<QueueProviderPlugin>();
    builder.use_plugin<AllocatorPlugin>();
    if (!m_headless)
    {
        builder.use_meta_plugin<PresentationPlugin>();
    }
    builder.use_meta_plugin<PipelinePlugin>();
}

}   // namespace kiln::gfx::renderer
