export module kiln.gfx.renderer.presentation.PresentationContextBuilderFailedError;

import kiln.app.registry.EntryBuilderFailedError;

namespace kiln::gfx::renderer {

export class PresentationContextBuilderFailedError
    : public app::EntryBuilderFailedError   //
{
public:
    using EntryBuilderFailedError::EntryBuilderFailedError;
};

}   // namespace kiln::gfx::renderer
