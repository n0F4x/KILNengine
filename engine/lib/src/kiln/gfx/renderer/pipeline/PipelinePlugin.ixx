export module kiln.gfx.renderer.pipeline.PipelinePlugin;

import vulkan_hpp;

import kiln.app.plugin.PluginInterface;
import kiln.gfx.renderer.device.DevicePlugin;

namespace kiln::gfx::renderer {

export class PipelinePlugin : public app::PluginInterface {};

export [[nodiscard]]
auto make_pipeline_plugin(DevicePlugin& device_plugin) -> PipelinePlugin
{
    device_plugin
        ->enable_features(
            vk::PhysicalDeviceVulkan11Features{ .shaderDrawParameters = vk::True }
        )
        .enable_features(
            vk::PhysicalDeviceMaintenance5Features{ .maintenance5 = vk::True }
        )
        .enable_features(
            vk::PhysicalDeviceDynamicRenderingFeatures{ .dynamicRendering = vk::True }
        );

    return PipelinePlugin{};
}

}   // namespace kiln::gfx::renderer
