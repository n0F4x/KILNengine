export module kiln.gfx.vulkan.InstancePluginInjection;

import kiln.config.Plugin;
import kiln.gfx.vulkan.InstancePlugin;

namespace kiln::gfx::vulkan {

export class InstancePluginInjection {
public:
    [[nodiscard]]
    static auto operator()(const config::Plugin& config_plugin) -> InstancePlugin
    {
        return InstancePlugin{ config_plugin.config() };
    }
};

}   // namespace kiln::gfx::vulkan
