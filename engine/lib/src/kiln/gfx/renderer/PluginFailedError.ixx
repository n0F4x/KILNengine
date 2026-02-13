export module kiln.gfx.renderer.PluginFailedError;

import kiln.app.plugin.PluginFailedError;

namespace kiln::gfx::renderer {

export class PluginFailedError : public app::PluginFailedError {
public:
    using app::PluginFailedError::PluginFailedError;
};

}   // namespace kiln::gfx::renderer
