export module kiln.gfx.renderer.debug_messenger.DebugMessengerPluginInjection;

import vulkan_hpp;

import kiln.gfx.renderer.debug_messenger.DebugMessengerPlugin;
import kiln.gfx.vulkan.InstancePlugin;

namespace kiln::gfx::renderer {

export class DebugMessengerPluginInjection {
public:
    static auto operator()(vulkan::InstancePlugin& instance_plugin)
        -> DebugMessengerPlugin
    {
        instance_plugin->enable_layer("VK_LAYER_KHRONOS_validation");
        instance_plugin->enable_extension(vk::EXTDebugUtilsExtensionName);

        return DebugMessengerPlugin{};
    }
};

}   // namespace kiln::gfx::renderer
