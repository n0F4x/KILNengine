export module kiln.gfx.renderer.pipeline.PipelineContextBuilder;

import kiln.app.registry.BuildDirector;
import kiln.app.registry.BuildableEntryBuilder;
import kiln.gfx.renderer.pipeline.PipelineContext;

namespace kiln::gfx::renderer {

export class PipelineContextBuilder;

auto describe_build(app::BuildDirector<PipelineContextBuilder>& build_director) -> void;

export class PipelineContextBuilder
    : public app::BuildableEntryBuilder<PipelineContextBuilder, describe_build> {
public:
    [[nodiscard]]
    static auto build() -> PipelineContext;
};

}   // namespace kiln::gfx::renderer
