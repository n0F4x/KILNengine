export module kiln.gfx.renderer.presentation.PresentationPluginFailedError;

import kiln.app.plugin.PluginFailedError;

namespace kiln::gfx::renderer {

export class PresentationPluginFailedError : public app::PluginFailedError {
public:
    using PluginFailedError::PluginFailedError;
};

}   // namespace kiln::gfx::renderer
