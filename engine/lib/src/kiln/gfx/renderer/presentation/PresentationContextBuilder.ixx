export module kiln.gfx.renderer.presentation.PresentationContextBuilder;

import kiln.gfx.renderer.presentation.PresentationContext;
import kiln.reg.BuildableEntryBuilder;
import kiln.reg.BuildDirector;

namespace kiln::gfx::renderer {

export class PresentationContextBuilder;

auto describe_build(reg::BuildDirector<PresentationContextBuilder>& build_director)
    -> void;

export class PresentationContextBuilder : public reg::BuildableEntryBuilder<
                                              PresentationContext,
                                              PresentationContextBuilder,
                                              describe_build>   //
{
public:
    [[nodiscard]]
    static auto build() -> PresentationContext;
};

}   // namespace kiln::gfx::renderer
