export module kiln.gfx.renderer.memory.AllocatorBuilder;

import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.vulkan.Instance;
import kiln.reg.BuildableEntryBuilder;
import kiln.reg.BuildDirector;

namespace kiln::gfx::renderer {

export class AllocatorBuilder;

auto describe_build(reg::BuildDirector<AllocatorBuilder>& build_director) -> void;

export class AllocatorBuilder
    : public reg::BuildableEntryBuilder<Allocator, AllocatorBuilder, describe_build> {
public:
    [[nodiscard]]
    static auto build(const vulkan::Instance& instance, const Device& device)
        -> Allocator;
};

}   // namespace kiln::gfx::renderer
