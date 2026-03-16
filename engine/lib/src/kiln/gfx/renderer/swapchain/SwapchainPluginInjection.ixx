export module kiln.gfx.renderer.swapchain.SwapchainPluginInjection;

import vulkan_hpp;

import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.renderer.swapchain.SwapchainPlugin;

namespace kiln::gfx::renderer {

export class SwapchainPluginInjection {
public:
    static auto operator()(DevicePlugin& device_plugin) -> SwapchainPlugin
    {
        device_plugin->enable_extension(vk::KHRSwapchainExtensionName);

        return SwapchainPlugin{};
    }
};

}   // namespace kiln::gfx::renderer
