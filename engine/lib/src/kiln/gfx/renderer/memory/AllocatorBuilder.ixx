export module kiln.gfx.renderer.memory.AllocatorBuilder;

import kiln.app.registry.BuildableEntryBuilder;
import kiln.app.registry.BuildDirector;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.vulkan.Instance;

namespace kiln::gfx::renderer {

export class AllocatorBuilder;

auto describe_build(app::BuildDirector<AllocatorBuilder>& build_director) -> void;

export class AllocatorBuilder
    : public app::BuildableEntryBuilder<AllocatorBuilder, describe_build> {
public:
    [[nodiscard]]
    static auto build(const vulkan::Instance& instance, const Device& device)
        -> Allocator;
};

}   // namespace kiln::gfx::renderer
