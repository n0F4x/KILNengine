export module kiln.gfx.renderer.command.CommandPlugin;

import kiln.app.plugin.PluginInterface;
import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.vulkan.InstancePlugin;

namespace kiln::gfx::renderer {

export class CommandPlugin : public app::PluginInterface {};

export [[nodiscard]]
auto make_command_plugin(
    vulkan::InstancePlugin& instance_plugin,
    DevicePlugin&           device_plugin
) -> CommandPlugin;

}   // namespace kiln::gfx::renderer
