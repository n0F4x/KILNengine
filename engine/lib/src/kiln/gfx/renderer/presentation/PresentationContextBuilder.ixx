export module kiln.gfx.renderer.presentation.PresentationContextBuilder;

import kiln.app.registry.EntryBuilderBase;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.presentation.PresentationContext;
import kiln.gfx.vulkan.InstanceBuilder;
import kiln.wsi.Context;

namespace kiln::gfx::renderer {

export class PresentationContextBuilder : public app::EntryBuilderBase {
public:
    [[nodiscard]]
    static auto create(
        const wsi::Context&      wsi_context,
        vulkan::InstanceBuilder& instance_builder,
        DeviceBuilder&           device_builder
    ) -> PresentationContextBuilder;

    [[nodiscard]]
    static auto build() -> PresentationContext;
};

}   // namespace kiln::gfx::renderer
