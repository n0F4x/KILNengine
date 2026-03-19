export module kiln.gfx.renderer.swapchain.SwapchainPluginInjection;

import vulkan_hpp;

import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.renderer.swapchain.SwapchainPlugin;
import kiln.gfx.vulkan.InstancePlugin;
import kiln.wsi.Plugin;

namespace kiln::gfx::renderer {

export class SwapchainPluginInjection {
public:
    [[nodiscard]]
    static auto operator()(
        vulkan::InstancePlugin& instance_plugin,
        const wsi::Plugin&      wsi_plugin,
        DevicePlugin&           device_plugin
    ) -> SwapchainPlugin;
};

}   // namespace kiln::gfx::renderer
