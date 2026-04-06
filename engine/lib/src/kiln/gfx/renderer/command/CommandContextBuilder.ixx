export module kiln.gfx.renderer.command.CommandContextBuilder;

import kiln.app.context.ContextBuilderInterface;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.vulkan.InstanceBuilder;

namespace kiln::gfx::renderer {

namespace internal {

export class CommandContext;

}   // namespace internal

export class CommandContextBuilder : public app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto
        create(vulkan::InstanceBuilder& instance_builder, DeviceBuilder& device_builder)
            -> CommandContextBuilder;

    [[nodiscard]]
    static auto build() -> internal::CommandContext;
};

namespace internal {

export class CommandContext {
public:
    using Builder = CommandContextBuilder;
};

}   // namespace internal

}   // namespace kiln::gfx::renderer
