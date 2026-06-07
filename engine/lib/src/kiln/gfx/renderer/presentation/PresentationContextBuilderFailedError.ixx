export module kiln.gfx.renderer.presentation.PresentationContextBuilderFailedError;

import kiln.app.context.ContextBuilderFailedError;

namespace kiln::gfx::renderer {

export class PresentationContextBuilderFailedError
    : public app::ContextBuilderFailedError   //
{
public:
    using ContextBuilderFailedError::ContextBuilderFailedError;
};

}   // namespace kiln::gfx::renderer
