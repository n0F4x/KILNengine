export module kiln.gfx.renderer.presentation.PresentationContextBuilder;

import kiln.app.registry.BuildableEntryBuilder;
import kiln.app.registry.BuildDirector;
import kiln.gfx.renderer.presentation.PresentationContext;

namespace kiln::gfx::renderer {

export class PresentationContextBuilder;

auto describe_build(app::BuildDirector<PresentationContextBuilder>& build_director)
    -> void;

export class PresentationContextBuilder
    : public app::BuildableEntryBuilder<PresentationContextBuilder, describe_build> {
public:
    [[nodiscard]]
    static auto build() -> PresentationContext;
};

}   // namespace kiln::gfx::renderer
