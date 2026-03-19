export module kiln.gfx.vulkan.DebugMessengerPluginInjection;

import vulkan_hpp;

import kiln.gfx.vulkan.DebugMessengerPlugin;
import kiln.gfx.vulkan.InstancePlugin;

namespace kiln::gfx::vulkan {

export class DebugMessengerPluginInjection {
public:
    [[nodiscard]]
    static auto operator()(InstancePlugin& instance_plugin) -> DebugMessengerPlugin
    {
        instance_plugin->enable_layer("VK_LAYER_KHRONOS_validation");
        instance_plugin->enable_extension(vk::EXTDebugUtilsExtensionName);

        return DebugMessengerPlugin{};
    }
};

}   // namespace kiln::gfx::vulkan
