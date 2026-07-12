module kiln.gfx.renderer.pipeline.PipelineContext;

import kiln.reg.BuildDirector;
import kiln.gfx.renderer.pipeline.PipelineContextBuilder;

namespace kiln::gfx::renderer {

auto describe_build(reg::BuildDirector<PipelineContext>& build_director) -> void
{
    build_director.use_builder<PipelineContextBuilder>();
}

}   // namespace kiln::gfx::renderer
