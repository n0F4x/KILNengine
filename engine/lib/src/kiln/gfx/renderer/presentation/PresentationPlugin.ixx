export module kiln.gfx.renderer.presentation.PresentationPlugin;

import kiln.app.plugin.PluginInterface;
import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.vulkan.InstancePlugin;
import kiln.wsi.Plugin;

namespace kiln::gfx::renderer {

export class PresentationPlugin : public app::PluginInterface {
public:
    [[nodiscard]]
    static auto create_plugin(
        vulkan::InstancePlugin& instance_plugin,
        const wsi::Plugin&      wsi_plugin,
        DevicePlugin&           device_plugin
    ) -> PresentationPlugin;
};

}   // namespace kiln::gfx::renderer
