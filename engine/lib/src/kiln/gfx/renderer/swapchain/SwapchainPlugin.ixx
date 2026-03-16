export module kiln.gfx.renderer.swapchain.SwapchainPlugin;

import kiln.app.App;

namespace kiln::gfx::renderer {

export class SwapchainPlugin {
public:
    static auto operator()(app::App&) -> void
    {
        // meta plugin
    }
};

}   // namespace kiln::gfx::renderer
