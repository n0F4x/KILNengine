export module demo;

import vulkan_hpp;

import kiln;

export struct Demo {
    kiln::gfx::renderer::TransferCommandPool   immediate_transfer_command_pool;
    kiln::wsi::Window                          window;
    vk::raii::SurfaceKHR                       surface;
    kiln::gfx::renderer::Swapchain             swapchain;
    vk::raii::PipelineLayout                   pipeline_layout;
    kiln::gfx::renderer::ShaderModule          shader_module;
    kiln::gfx::renderer::GraphicsPipeline      pipeline;
    kiln::gfx::renderer::GraphicsCommandPool   graphics_command_pool;
    kiln::gfx::renderer::GraphicsCommandBuffer graphics_command_buffer;

    Demo(
        const kiln::config::Config&        config,
        const vk::raii::Instance&          vulkan_instance,
        const kiln::wsi::Context&          wsi_context,
        const kiln::gfx::renderer::Device& render_device
    );

    auto render() -> void;
};

export struct DemoPlugin {
    static auto operator()(kiln::app::App& app) -> void;
};

export [[nodiscard]]
auto demo_plugin_injection(
    const kiln::config::Plugin&,
    kiln::gfx::vulkan::InstancePlugin&,
    const kiln::wsi::Plugin&,
    kiln::gfx::renderer::DevicePlugin& device_plugin,
    const kiln::gfx::renderer::PipelinePlugin&
) -> DemoPlugin;
