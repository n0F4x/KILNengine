export module kiln.gfx.renderer.command.QueueProviderBuilder;

import kiln.app.registry.BuildableEntryBuilder;
import kiln.app.registry.BuildDirector;
import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.device.Device;

namespace kiln::gfx::renderer {

export class QueueProviderBuilder;

auto describe_build(app::BuildDirector<QueueProviderBuilder>& build_director) -> void;

export class QueueProviderBuilder
    : public app::BuildableEntryBuilder<QueueProviderBuilder, describe_build> {
public:
    [[nodiscard]]
    static auto build(const Device& device) -> QueueProvider;
};

}   // namespace kiln::gfx::renderer
