export module kiln.gfx.renderer.command.QueueProviderBuilder;

import kiln.app.registry.EntryBuilderBase;
import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.vulkan.InstanceBuilder;

namespace kiln::gfx::renderer {

export class QueueProviderBuilder : public app::EntryBuilderBase {
public:
    [[nodiscard]]
    static auto create(
        vulkan::InstanceBuilder& instance_builder,
        DeviceBuilder&           device_builder
    ) -> QueueProviderBuilder;

    [[nodiscard]]
    static auto build(const Device& device) -> QueueProvider;
};

}   // namespace kiln::gfx::renderer
