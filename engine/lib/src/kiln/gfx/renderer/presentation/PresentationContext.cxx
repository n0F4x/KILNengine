module kiln.gfx.renderer.presentation.PresentationContext;

import kiln.gfx.renderer.presentation.PresentationContextBuilder;
import kiln.reg.BuildDirector;

namespace kiln::gfx::renderer {

auto describe_build(reg::BuildDirector<PresentationContext>& build_director) -> void
{
    build_director.use_builder<PresentationContextBuilder>();
}

}   // namespace kiln::gfx::renderer
