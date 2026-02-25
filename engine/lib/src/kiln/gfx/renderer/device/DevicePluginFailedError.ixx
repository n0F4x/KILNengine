export module kiln.gfx.renderer.device.DevicePluginFailedError;

import kiln.app.plugin.PluginFailedError;

namespace kiln::gfx::renderer {

export class DevicePluginFailedError : public app::PluginFailedError {
public:
    using PluginFailedError::PluginFailedError;
};

}   // namespace kiln::gfx::renderer
