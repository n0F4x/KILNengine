module kiln.gfx.renderer.pipeline.PipelineContext;

import kiln.app.registry.EntryBuildDirector;
import kiln.gfx.renderer.pipeline.PipelineContextBuilder;

namespace kiln::gfx::renderer {

auto describe_build(app::EntryBuildDirector<PipelineContext>& build_director) -> void
{
    build_director.use_builder<PipelineContextBuilder>();
}

}   // namespace kiln::gfx::renderer
