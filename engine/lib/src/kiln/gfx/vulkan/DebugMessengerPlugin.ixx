export module kiln.gfx.vulkan.DebugMessengerPlugin;

import vulkan_hpp;

import kiln.app.plugin.PluginInterface;

namespace kiln::gfx::vulkan {

export class DebugMessengerPlugin : public app::PluginInterface {
public:
    static auto operator()(const vk::raii::Instance& instance)
        -> vk::raii::DebugUtilsMessengerEXT;
};

}   // namespace kiln::gfx::vulkan
