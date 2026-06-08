export module kiln.gfx.renderer.pipeline.PipelineContextBuilder;

import vulkan_hpp;

import kiln.app.registry.EntryBase;
import kiln.app.registry.EntryBuilderInterface;
import kiln.gfx.renderer.device.DeviceBuilder;

namespace kiln::gfx::renderer {

namespace internal {

export class PipelineContext;

}   // namespace internal

export class PipelineContextBuilder : public app::EntryBuilderInterface {
public:
    [[nodiscard]]
    static auto create(DeviceBuilder& device_builder) -> PipelineContextBuilder
    {
        device_builder.enable_features(
            vk::PhysicalDeviceVulkan11Features{ .shaderDrawParameters = vk::True }
        );
        device_builder.enable_features(
            vk::PhysicalDeviceMaintenance5Features{ .maintenance5 = vk::True }
        );
        device_builder.enable_features(
            vk::PhysicalDeviceDynamicRenderingFeatures{ .dynamicRendering = vk::True }
        );

        return PipelineContextBuilder{};
    }

    [[nodiscard]]
    static auto build() -> internal::PipelineContext;
};

namespace internal {

export class PipelineContext : public app::EntryBase {
public:
    using Builder = PipelineContextBuilder;
};

}   // namespace internal

auto PipelineContextBuilder::build() -> internal::PipelineContext
{
    return internal::PipelineContext{};
}

}   // namespace kiln::gfx::renderer
