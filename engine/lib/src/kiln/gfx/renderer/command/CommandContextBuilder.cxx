module kiln.gfx.renderer.command.CommandContextBuilder;

import vulkan_hpp;

namespace kiln::gfx::renderer {

auto CommandContextBuilder::create(
    vulkan::InstanceBuilder& instance_builder,
    DeviceBuilder&           device_builder
) -> CommandContextBuilder
{
    instance_builder.target_api_version(vk::ApiVersion13);

    device_builder.require_minimum_version(vk::ApiVersion13);
    device_builder.enable_features(
        vk::PhysicalDeviceVulkan13Features{ .synchronization2 = vk::True }
    );

    return CommandContextBuilder{};
}

auto CommandContextBuilder::build() -> internal::CommandContext
{
    return internal::CommandContext{};
}

}   // namespace kiln::gfx::renderer
