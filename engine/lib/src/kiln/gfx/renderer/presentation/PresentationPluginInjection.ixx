export module kiln.gfx.renderer.presentation.PresentationPluginInjection;

import vulkan_hpp;

import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.renderer.presentation.PresentationPlugin;
import kiln.gfx.vulkan.InstancePlugin;
import kiln.wsi.Plugin;

namespace kiln::gfx::renderer {

export class PresentationPluginInjection {
public:
    [[nodiscard]]
    static auto operator()(
        vulkan::InstancePlugin& instance_plugin,
        const wsi::Plugin&      wsi_plugin,
        DevicePlugin&           device_plugin
    ) -> PresentationPlugin;
};

}   // namespace kiln::gfx::renderer
