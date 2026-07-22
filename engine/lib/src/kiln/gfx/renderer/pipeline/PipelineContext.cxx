module kiln.gfx.renderer.pipeline.PipelineContext;

import kiln.reg.BuildDirector;
import kiln.gfx.renderer.pipeline.PipelineContextBuilder;

auto kiln::reg::EntryTraits<kiln::gfx::renderer::PipelineContext>::describe_build(
    BuildDirector<gfx::renderer::PipelineContext>& build_director
) -> void
{
    build_director.use_builder<gfx::renderer::PipelineContextBuilder>();
}
