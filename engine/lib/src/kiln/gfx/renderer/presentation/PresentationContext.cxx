module kiln.gfx.renderer.presentation.PresentationContext;

import kiln.gfx.renderer.presentation.PresentationContextBuilder;
import kiln.reg.BuildDirector;

auto kiln::reg::EntryTraits<kiln::gfx::renderer::PresentationContext>::describe_build(
    BuildDirector<gfx::renderer::PresentationContext>& build_director
) -> void
{
    build_director.use_builder<gfx::renderer::PresentationContextBuilder>();
}
