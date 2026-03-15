export module kiln.gfx.renderer.pipeline.PipelinePluginInjection;

import vulkan_hpp;

import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.renderer.pipeline.PipelinePlugin;

namespace kiln::gfx::renderer {

export class PipelinePluginInjection {
public:
    [[nodiscard]]
    static auto operator()(DevicePlugin& device_plugin) -> PipelinePlugin
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
};

}   // namespace kiln::gfx::renderer
