export module kiln.gfx.renderer.pipeline.PipelineContext;

import kiln.app.registry.BuildableEntry;
import kiln.app.registry.BuildDirector;

namespace kiln::gfx::renderer {

export struct PipelineContext;

auto describe_build(app::BuildDirector<PipelineContext>& build_director) -> void;

struct PipelineContext : app::BuildableEntry<PipelineContext, describe_build> {};

}   // namespace kiln::gfx::renderer
