export module kiln.gfx.renderer.command.CommandPluginInjection;

import kiln.gfx.renderer.command.CommandPlugin;
import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.vulkan.InstancePlugin;

namespace kiln::gfx::renderer {

export class CommandPluginInjection {
public:
    static auto
        operator()(vulkan::InstancePlugin& instance_plugin, DevicePlugin& device_plugin)
            -> CommandPlugin;
};

}   // namespace kiln::gfx::renderer
