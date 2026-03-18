export module kiln.gfx.renderer.debug_messenger.DebugMessengerPlugin;

import vulkan_hpp;

import kiln.app.plugin.PluginInterface;
import kiln.gfx.renderer.debug_messenger.DebugMessenger;
import kiln.gfx.vulkan.default_debug_messenger_callback;
import kiln.gfx.vulkan.result.check_result;

namespace kiln::gfx::renderer {

export class DebugMessengerPlugin : public app::PluginInterface {
public:
    static auto build(const vk::raii::Instance& instance) -> DebugMessenger;
};

}   // namespace kiln::gfx::renderer

module :private;

namespace kiln::gfx::renderer {

auto DebugMessengerPlugin::build(const vk::raii::Instance& instance) -> DebugMessenger
{
    constexpr static vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{
        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                         | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        .pfnUserCallback = vulkan::default_debug_messenger_callback,
    };

    return DebugMessenger{
        vulkan::check_result(
            instance.createDebugUtilsMessengerEXT(debug_messenger_create_info)
        )   //
    };
}

}   // namespace kiln::gfx::renderer
