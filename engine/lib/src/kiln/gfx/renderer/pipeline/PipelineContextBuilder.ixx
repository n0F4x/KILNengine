export module kiln.gfx.renderer.pipeline.PipelineContextBuilder;

import kiln.app.registry.BuildableEntryBuilder;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.pipeline.PipelineContext;

namespace kiln::gfx::renderer {

export class PipelineContextBuilder : public app::BuildableEntryBuilder {
public:
    [[nodiscard]]
    static auto create(DeviceBuilder& device_builder) -> PipelineContextBuilder;

    [[nodiscard]]
    static auto build() -> PipelineContext;
};

}   // namespace kiln::gfx::renderer
