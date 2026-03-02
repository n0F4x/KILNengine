export module demo;

import vulkan_hpp;

import kiln;

export struct Demo {
    kiln::wsi::VulkanWindow         window;
    vk::raii::PipelineLayout        pipeline_layout;
    kiln::gfx::vulkan::ShaderModule shader_module;
    vk::raii::Pipeline              pipeline;

    Demo(
        const kiln::config::Config&        config,
        const vk::raii::Instance&          vulkan_instance,
        const kiln::wsi::Context&          wsi_context,
        const kiln::gfx::renderer::Device& render_device
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
    kiln::gfx::renderer::DevicePlugin& device_plugin
) -> DemoPlugin;
