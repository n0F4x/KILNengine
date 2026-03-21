export module kiln.gfx.renderer.command.QueueProviderPluginInjection;

import kiln.gfx.renderer.command.CommandPlugin;
import kiln.gfx.renderer.command.QueueProviderPlugin;
import kiln.gfx.renderer.device.DevicePlugin;

namespace kiln::gfx::renderer {

export class QueueProviderPluginInjection {
public:
    [[nodiscard]]
    static auto operator()(DevicePlugin& device_plugin, const CommandPlugin&)
        -> QueueProviderPlugin
    {
        return QueueProviderPlugin{ device_plugin };
    }
};

}   // namespace kiln::gfx::renderer
