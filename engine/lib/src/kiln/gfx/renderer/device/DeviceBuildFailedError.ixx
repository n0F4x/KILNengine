export module kiln.gfx.renderer.device.DeviceBuildFailedError;

import kiln.app.context.ContextBuilderFailedError;

namespace kiln::gfx::renderer {

export class DeviceBuildFailedError : public app::ContextBuilderFailedError {
public:
    using ContextBuilderFailedError::ContextBuilderFailedError;
};

}   // namespace kiln::gfx::renderer
