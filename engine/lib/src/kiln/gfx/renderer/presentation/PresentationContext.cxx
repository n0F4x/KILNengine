module kiln.gfx.renderer.presentation.PresentationContext;

import kiln.app.registry.BuildDirector;
import kiln.gfx.renderer.presentation.PresentationContextBuilder;

namespace kiln::gfx::renderer {

auto describe_build(app::BuildDirector<PresentationContext>& build_director)
    -> void
{
    build_director.use_builder<PresentationContextBuilder>();
}

}   // namespace kiln::gfx::renderer
