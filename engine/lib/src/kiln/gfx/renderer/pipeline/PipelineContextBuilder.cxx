module kiln.gfx.renderer.pipeline.PipelineContextBuilder;

import vulkan;

import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.pipeline.PipelineContext;

namespace kiln::gfx::renderer {

[[nodiscard]]
auto make_pipeline_context_builder(DeviceBuilder& device_builder)
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

auto describe_build(app::BuildDirector<PipelineContextBuilder>& build_director) -> void
{
    build_director.use_function<make_pipeline_context_builder>();
}

auto PipelineContextBuilder::build() -> PipelineContext
{
    return PipelineContext{};
}

}   // namespace kiln::gfx::renderer
