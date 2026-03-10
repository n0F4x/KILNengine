export module kiln.gfx.renderer.memory.AllocatorPlugin;

import vulkan_hpp;

import kiln.app.App;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.device.Device;

namespace kiln::gfx::renderer {

export class AllocatorPlugin {
public:
    static auto operator()(app::App& app) -> void
    {
        app.context().insert(
            Allocator{
                app.context().at<vk::raii::Instance>(),
                app.context().at<Device>(),
            }
        );
    }
};

}   // namespace kiln::gfx::renderer
