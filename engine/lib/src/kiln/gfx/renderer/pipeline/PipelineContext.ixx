export module kiln.gfx.renderer.pipeline.PipelineContext;

import kiln.reg.BuildableEntry;
import kiln.reg.BuildDirector;

namespace kiln::gfx::renderer {

export struct PipelineContext;

auto describe_build(reg::BuildDirector<PipelineContext>& build_director) -> void;

struct PipelineContext : reg::BuildableEntry<PipelineContext, describe_build> {};

}   // namespace kiln::gfx::renderer
