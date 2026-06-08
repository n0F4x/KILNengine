export module kiln.gfx.renderer.presentation.PresentationContextBuilder;

import kiln.app.registry.EntryBase;
import kiln.app.registry.EntryBuilderInterface;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.vulkan.InstanceBuilder;
import kiln.wsi.ContextBuilder;

namespace kiln::gfx::renderer {

namespace internal {

export class PresentationContext;

}   // namespace internal

export class PresentationContextBuilder : public app::EntryBuilderInterface {
public:
    [[nodiscard]]
    static auto create(
        const wsi::ContextBuilder& wsi_context_builder,
        vulkan::InstanceBuilder&   instance_builder,
        DeviceBuilder&             device_builder
    ) -> PresentationContextBuilder;

    [[nodiscard]]
    static auto build() -> internal::PresentationContext;
};

namespace internal {

export class PresentationContext : public app::EntryBase {
public:
    using Builder = PresentationContextBuilder;
};

}   // namespace internal

}   // namespace kiln::gfx::renderer
