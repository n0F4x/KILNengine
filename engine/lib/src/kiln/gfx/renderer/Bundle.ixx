export module kiln.gfx.renderer.Bundle;

import kiln.app.Builder;
import kiln.gfx.renderer.debug_messenger.DebugMessengerPluginInjection;
import kiln.gfx.renderer.device.DevicePluginInjection;
import kiln.gfx.renderer.memory.AllocatorPluginInjection;
import kiln.gfx.renderer.pipeline.PipelinePluginInjection;

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

namespace kiln::gfx::renderer {

Bundle::Bundle(const CreateInfo& create_info) : m_headless{ create_info.headless } {}

auto Bundle::operator()(app::Builder& builder) const -> void
{
#ifdef KILN_DEBUG
    builder.inject_plugin(DebugMessengerPluginInjection{});
#endif

    builder.inject_plugin(DevicePluginInjection{ m_headless });
    builder.inject_plugin(AllocatorPluginInjection{});
    builder.inject_plugin(PipelinePluginInjection{});
}

}   // namespace kiln::gfx::renderer
