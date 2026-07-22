export module kiln.gfx.renderer.pipeline.PipelineContext;

import kiln.reg.BuildDirector;
import kiln.reg.EntryTraits;

namespace kiln::gfx::renderer {

export struct PipelineContext {};

}   // namespace kiln::gfx::renderer

template <>
struct kiln::reg::EntryTraits<kiln::gfx::renderer::PipelineContext> {
    static auto describe_build(
        BuildDirector<gfx::renderer::PipelineContext>& build_director
    ) -> void;
};
