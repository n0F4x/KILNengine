export module kiln.gfx.renderer.presentation.PresentationContext;

import kiln.reg.BuildableEntry;
import kiln.reg.BuildDirector;

namespace kiln::gfx::renderer {

export struct PresentationContext;

auto describe_build(reg::BuildDirector<PresentationContext>& build_director) -> void;

struct PresentationContext : reg::BuildableEntry<PresentationContext, describe_build> {};

}   // namespace kiln::gfx::renderer
