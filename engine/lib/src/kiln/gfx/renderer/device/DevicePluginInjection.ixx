export module kiln.gfx.renderer.device.DevicePluginInjection;

import kiln.app.memory.MemoryPlugin;
import kiln.gfx.renderer.device.DevicePlugin;

namespace kiln::gfx::renderer {

export struct DevicePluginInjection {
    [[nodiscard]]
    static auto operator()(const app::MemoryPlugin& memory_plugin) -> DevicePlugin
    {
        return DevicePlugin{ &memory_plugin.builder_local_arena().pool_resource() };
    }
};

}   // namespace kiln::gfx::renderer
