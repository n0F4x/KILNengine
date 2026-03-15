export module kiln.gfx.renderer.debug_messenger.DebugMessengerPlugin;

import vulkan_hpp;

import kiln.app.App;
import kiln.gfx.renderer.debug_messenger.DebugMessenger;
import kiln.gfx.vulkan.default_debug_messenger_callback;
import kiln.gfx.vulkan.result.check_result;

namespace kiln::gfx::renderer {

export class DebugMessengerPlugin {
public:
    static auto operator()(app::App& app) -> void;
};

}   // namespace kiln::gfx::renderer

module :private;

namespace kiln::gfx::renderer {

auto DebugMessengerPlugin::operator()(app::App& app) -> void
{
    const vk::raii::Instance& instance{ app.context().at<vk::raii::Instance>() };

    constexpr static vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{
        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                         | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        .pfnUserCallback = vulkan::default_debug_messenger_callback,
    };

    app.context().insert(
        vulkan::check_result(
            instance.createDebugUtilsMessengerEXT(debug_messenger_create_info)
        )
    );
}

}   // namespace kiln::gfx::renderer
