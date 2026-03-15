export module demo;

import vulkan_hpp;

import kiln;

export struct Demo {
    kiln::gfx::renderer::TransferCommandPool immediate_transfer_command_pool;
    kiln::wsi::VulkanWindow                  window;
    vk::raii::PipelineLayout                 pipeline_layout;
    kiln::gfx::renderer::ShaderModule        shader_module;
    kiln::gfx::renderer::GraphicsPipeline    pipeline;
    kiln::gfx::renderer::Buffer              index_buffer;

    Demo(
        const kiln::config::Config&           config,
        const vk::raii::Instance&             vulkan_instance,
        const kiln::wsi::Context&             wsi_context,
        const kiln::gfx::renderer::Device&    render_device,
        const kiln::gfx::renderer::Allocator& render_allocator
    );
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
    const kiln::gfx::renderer::AllocatorPlugin&,
    const kiln::gfx::renderer::PipelinePlugin&
) -> DemoPlugin;
