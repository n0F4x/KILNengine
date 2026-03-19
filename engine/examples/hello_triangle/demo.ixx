module;

#include <vector>

export module demo;

import vulkan_hpp;

import kiln;

export class Demo {
public:
    Demo(
        const kiln::config::Config&               config,
        const vk::raii::Instance&                 vulkan_instance,
        const kiln::wsi::Context&                 wsi_context,
        const kiln::gfx::renderer::Device&        render_device,
        const kiln::gfx::renderer::QueueProvider& render_queue_provider
    );

    [[nodiscard]]
    auto window() noexcept -> kiln::wsi::Window&;

    auto render() -> void;

private:
    kiln::wsi::Window                                       m_window;
    vk::raii::SurfaceKHR                                    m_surface;
    kiln::gfx::renderer::Swapchain                          m_swapchain;
    vk::raii::PipelineLayout                                m_pipeline_layout;
    kiln::gfx::renderer::ShaderModule                       m_shader_module;
    kiln::gfx::renderer::GraphicsPipeline                   m_pipeline;
    uint8_t                                                 m_number_of_frames{ 2 };
    uint8_t                                                 m_current_frame_index{};
    std::vector<kiln::gfx::renderer::GraphicsCommandPool>   m_graphics_command_pools;
    std::vector<kiln::gfx::renderer::GraphicsCommandBuffer> m_graphics_command_buffers;
};

export struct DemoPlugin : kiln::app::PluginInterface {
    static auto operator()(
        const kiln::config::Config&               config,
        const vk::raii::Instance&                 vulkan_instance,
        const kiln::wsi::Context&                 wsi_context,
        const kiln::gfx::renderer::Device&        render_device,
        const kiln::gfx::renderer::QueueProvider& render_queue_provider
    ) -> Demo;
};

export [[nodiscard]]
auto demo_plugin_injection(
    kiln::gfx::vulkan::InstancePlugin&        instance_plugin,
    kiln::gfx::renderer::DevicePlugin&        device_plugin,
    kiln::gfx::renderer::QueueProviderPlugin& queue_provider_plugin,
    const kiln::gfx::renderer::PipelinePlugin&
) -> DemoPlugin;
