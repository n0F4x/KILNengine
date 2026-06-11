module kiln.gfx.renderer.pipeline.PipelineContextBuilder;

import vulkan_hpp;

import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.pipeline.PipelineContext;

namespace kiln::gfx::renderer {

auto PipelineContextBuilder::create(DeviceBuilder& device_builder)
    -> PipelineContextBuilder
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

auto PipelineContextBuilder::build() -> PipelineContext
{
    return PipelineContext{};
}

}   // namespace kiln::gfx::renderer
