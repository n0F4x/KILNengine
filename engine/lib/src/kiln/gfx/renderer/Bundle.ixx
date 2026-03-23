export module kiln.gfx.renderer.Bundle;

import kiln.app.Builder;
import kiln.gfx.renderer.command.CommandPluginInjection;
import kiln.gfx.renderer.command.QueueProviderPluginInjection;
import kiln.gfx.renderer.device.DevicePluginInjection;
import kiln.gfx.renderer.memory.AllocatorPluginInjection;
import kiln.gfx.renderer.pipeline.PipelinePluginInjection;
import kiln.gfx.renderer.presentation.PresentationPluginInjection;

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
    builder.inject_plugin(DevicePluginInjection{});
    builder.inject_meta_plugin(CommandPluginInjection{});
    builder.inject_plugin(QueueProviderPluginInjection{});
    builder.inject_plugin(AllocatorPluginInjection{});
    if (!m_headless)
    {
        builder.inject_meta_plugin(PresentationPluginInjection{});
    }
    builder.inject_meta_plugin(PipelinePluginInjection{});
}

}   // namespace kiln::gfx::renderer
