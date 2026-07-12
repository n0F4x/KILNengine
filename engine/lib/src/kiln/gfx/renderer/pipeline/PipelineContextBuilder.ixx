export module kiln.gfx.renderer.pipeline.PipelineContextBuilder;

import kiln.gfx.renderer.pipeline.PipelineContext;
import kiln.reg.BuildDirector;
import kiln.reg.BuildableEntryBuilder;

namespace kiln::gfx::renderer {

export class PipelineContextBuilder;

auto describe_build(reg::BuildDirector<PipelineContextBuilder>& build_director) -> void;

export class PipelineContextBuilder
    : public reg::BuildableEntryBuilder<PipelineContextBuilder, describe_build> {
public:
    [[nodiscard]]
    static auto build() -> PipelineContext;
};

}   // namespace kiln::gfx::renderer
