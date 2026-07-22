export module kiln.gfx.renderer.presentation.PresentationContext;

import kiln.reg.BuildDirector;
import kiln.reg.EntryTraits;

namespace kiln::gfx::renderer {

export struct PresentationContext {};

}   // namespace kiln::gfx::renderer

template <>
struct kiln::reg::EntryTraits<kiln::gfx::renderer::PresentationContext> {
    static auto describe_build(
        BuildDirector<gfx::renderer::PresentationContext>& build_director
    ) -> void;
};
