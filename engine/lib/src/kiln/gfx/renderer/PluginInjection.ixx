export module kiln.gfx.renderer.PluginInjection;

import kiln.gfx.renderer.Plugin;
import kiln.gfx.vulkan.InstancePlugin;

namespace kiln::gfx::renderer {

export class PluginInjection {
public:
    static auto operator()(vulkan::InstancePlugin& instance_plugin) -> Plugin
    {
        return Plugin{ instance_plugin };
    }
};

}   // namespace kiln::gfx::renderer
