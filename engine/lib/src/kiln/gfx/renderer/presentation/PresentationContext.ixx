export module kiln.gfx.renderer.presentation.PresentationContext;

import kiln.app.registry.BuildableEntry;
import kiln.app.registry.BuildDirector;

namespace kiln::gfx::renderer {

export struct PresentationContext;

auto describe_build(app::BuildDirector<PresentationContext>& build_director)
    -> void;

struct PresentationContext
    : app::BuildableEntry<PresentationContext, describe_build> {};

}   // namespace kiln::gfx::renderer
