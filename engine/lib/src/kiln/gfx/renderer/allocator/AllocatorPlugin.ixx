export module kiln.gfx.renderer.allocator.AllocatorPlugin;

import vulkan_hpp;

import kiln.app.App;
import kiln.gfx.renderer.allocator.Allocator;
import kiln.gfx.renderer.device.Device;

namespace kiln::gfx::renderer {

export class AllocatorPlugin {
public:
    static auto operator()(app::App& app) -> void
    {
        app.resources().insert(
            Allocator{
                app.resources().at<vk::raii::Instance>(),
                app.resources().at<Device>(),
            }
        );
    }
};

}   // namespace kiln::gfx::renderer
