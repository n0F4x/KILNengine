export module kiln.gfx.renderer.memory.AllocatorBuilder;

import kiln.app.registry.BuildableEntryBuilder;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.InstanceBuilder;

namespace kiln::gfx::renderer {

export class AllocatorBuilder : public app::BuildableEntryBuilder {
public:
    [[nodiscard]]
    static auto create(
        vulkan::InstanceBuilder& instance_builder,
        DeviceBuilder&           device_builder
    ) -> AllocatorBuilder;

    [[nodiscard]]
    static auto build(const vulkan::Instance& instance, const Device& device)
        -> Allocator;
};

}   // namespace kiln::gfx::renderer
