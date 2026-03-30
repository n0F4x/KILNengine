export module kiln.gfx.vulkan.DebugMessengerPlugin;

import vulkan_hpp;

import kiln.app.plugin.PluginInterface;
import kiln.gfx.vulkan.InstancePlugin;

namespace kiln::gfx::vulkan {

export class DebugMessengerPlugin : public app::PluginInterface {
public:
    [[nodiscard]]
    static auto create_plugin(InstancePlugin& instance_plugin) -> DebugMessengerPlugin
    {
        instance_plugin->enable_layer("VK_LAYER_KHRONOS_validation");
        instance_plugin->enable_extension(vk::EXTDebugUtilsExtensionName);

        return DebugMessengerPlugin{};
    }

    [[nodiscard]]
    static auto operator()(const vk::raii::Instance& instance)
        -> vk::raii::DebugUtilsMessengerEXT;
};

}   // namespace kiln::gfx::vulkan
