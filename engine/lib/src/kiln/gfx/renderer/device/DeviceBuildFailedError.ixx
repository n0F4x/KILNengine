export module kiln.gfx.renderer.device.DeviceBuildFailedError;

import kiln.app.registry.EntryBuilderFailedError;

namespace kiln::gfx::renderer {

export class DeviceBuildFailedError : public app::EntryBuilderFailedError {
public:
    using EntryBuilderFailedError::EntryBuilderFailedError;
};

}   // namespace kiln::gfx::renderer
