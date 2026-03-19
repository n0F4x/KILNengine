export module kiln.gfx.renderer.memory.AllocatorPlugin;

import vulkan_hpp;

import kiln.app.plugin.PluginInterface;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.device.Device;

namespace kiln::gfx::renderer {

export class AllocatorPlugin : public app::PluginInterface {
public:
    [[nodiscard]]
    static auto operator()(const vk::raii::Instance& instance, const Device& device)
        -> Allocator
    {
        return Allocator{ instance, device };
    }
};

}   // namespace kiln::gfx::renderer
