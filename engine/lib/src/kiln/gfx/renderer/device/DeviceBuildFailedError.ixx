export module kiln.gfx.renderer.device.DeviceBuildFailedError;

import kiln.reg.EntryBuilderFailedError;

namespace kiln::gfx::renderer {

export class DeviceBuildFailedError : public reg::EntryBuilderFailedError {
public:
    using EntryBuilderFailedError::EntryBuilderFailedError;
};

}   // namespace kiln::gfx::renderer
