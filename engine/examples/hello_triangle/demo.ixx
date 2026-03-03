export module demo;

import vulkan_hpp;

import kiln;

export struct Demo {
    kiln::wsi::VulkanWindow         window;
    vk::raii::PipelineLayout        pipeline_layout;
    kiln::gfx::vulkan::ShaderModule shader_module;
    vk::raii::Pipeline              pipeline;
    kiln::gfx::renderer::Buffer     index_buffer;

    Demo(
        const kiln::config::Config&           config,
        const vk::raii::Instance&             vulkan_instance,
        const kiln::wsi::Context&             wsi_context,
        const kiln::gfx::renderer::Device&    render_device,
        const kiln::gfx::renderer::Allocator& render_allocator,
        kiln::gfx::renderer::CommandPool&     render_command_pool
    );
};

export struct DemoPlugin {
    static auto operator()(kiln::app::App& app) -> void;
};

export [[nodiscard]]
auto demo_plugin_injection(
    const kiln::config::Plugin&,
    const kiln::gfx::vulkan::InstancePlugin&,
    const kiln::wsi::Plugin&,
    kiln::gfx::renderer::DevicePlugin& device_plugin,
    kiln::gfx::renderer::AllocatorPlugin&,
    kiln::gfx::renderer::CommandPoolPlugin&
) -> DemoPlugin;
