export module kiln.gfx.renderer.pipeline.PipelinePlugin;

import kiln.app.App;

namespace kiln::gfx::renderer {

export struct PipelinePlugin {
    static auto operator()(app::App&) -> void
    {
        // meta plugin
    }
};

}   // namespace kiln::gfx::renderer
