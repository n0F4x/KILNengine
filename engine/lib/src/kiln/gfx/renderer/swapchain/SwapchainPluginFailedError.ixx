export module kiln.gfx.renderer.swapchain.SwapchainPluginFailedError;

import kiln.app.plugin.PluginFailedError;

namespace kiln::gfx::renderer {

export class SwapchainPluginFailedError : public app::PluginFailedError {
public:
    using PluginFailedError::PluginFailedError;
};

}   // namespace kiln::gfx::renderer
