export module kiln.gfx.renderer.presentation.PresentationContextBuilderFailedError;

import kiln.reg.EntryBuilderFailedError;

namespace kiln::gfx::renderer {

export class PresentationContextBuilderFailedError
    : public reg::EntryBuilderFailedError   //
{
public:
    using EntryBuilderFailedError::EntryBuilderFailedError;
};

}   // namespace kiln::gfx::renderer
