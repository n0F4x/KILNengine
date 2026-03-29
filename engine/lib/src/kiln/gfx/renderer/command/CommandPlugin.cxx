module kiln.gfx.renderer.command.CommandPlugin;

import vulkan_hpp;

namespace kiln::gfx::renderer {

auto make_command_plugin(
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
