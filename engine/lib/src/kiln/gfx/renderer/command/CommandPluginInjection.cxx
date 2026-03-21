module kiln.gfx.renderer.command.CommandPluginInjection;

import vulkan_hpp;

namespace kiln::gfx::renderer {

auto CommandPluginInjection::operator()(
    vulkan::InstancePlugin& instance_plugin,
    DevicePlugin&           device_plugin
) -> CommandPlugin
{
    instance_plugin->target_api_version(vk::ApiVersion13);
    device_plugin->require_minimum_version(vk::ApiVersion13);

    device_plugin->enable_features(
        vk::PhysicalDeviceVulkan13Features{ .synchronization2 = vk::True }
    );

    return CommandPlugin{};
}

}   // namespace kiln::gfx::renderer
