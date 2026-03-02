module;

#include <array>
#include <cstdint>
#include <filesystem>
#include <source_location>

#include "kiln/util/contract_macros.hpp"

module demo;

[[nodiscard]]
auto create_window(
    const kiln::config::Config&        config,
    const vk::raii::Instance&          vulkan_instance,
    const kiln::wsi::Context&          wsi_context,
    const kiln::gfx::renderer::Device& render_device
) -> kiln::wsi::VulkanWindow
{
    constexpr static kiln::wsi::WindowedWindowSettings screen_settings{
        .content_size{ .width = 640, .height = 480 }
    };
    const kiln::wsi::Window::CreateInfo window_info{
        .title    = config.app_name(),
        .settings = screen_settings,
    };

    return render_device.create_window(vulkan_instance, wsi_context, window_info);
}

[[nodiscard]]
auto create_graphics_pipeline(
    const kiln::gfx::renderer::Device&     device,
    const vk::raii::PipelineLayout&        layout,
    const kiln::gfx::vulkan::ShaderModule& shader_module
) -> vk::raii::Pipeline
{
    PRECOND(device.capabilities().contains_features(
        vk::PhysicalDeviceMaintenance5Features{ .maintenance5 = vk::True }
    ));

    const vk::ShaderModuleCreateInfo vertex_shader_module_create_info{
        .codeSize = shader_module.code().size_bytes(),
        .pCode    = shader_module.code().data(),
    };
    const vk::ShaderModuleCreateInfo fragment_shader_module_create_info{
        .codeSize = shader_module.code().size_bytes(),
        .pCode    = shader_module.code().data(),
    };

    const std::array shader_stage_create_infos{
        vk::PipelineShaderStageCreateInfo{
                                          .pNext = &vertex_shader_module_create_info,
                                          .stage = vk::ShaderStageFlagBits::eVertex,
                                          .pName = "main",
                                          },
        vk::PipelineShaderStageCreateInfo{
                                          .pNext = &fragment_shader_module_create_info,
                                          .stage = vk::ShaderStageFlagBits::eFragment,
                                          .pName = "main",
                                          },
    };

    constexpr static vk::PipelineVertexInputStateCreateInfo
        vertex_input_state_create_info{};

    constexpr static vk::PipelineInputAssemblyStateCreateInfo
        input_assembly_state_create_info{
            .topology = vk::PrimitiveTopology::eTriangleList,
        };

    constexpr static vk::PipelineViewportStateCreateInfo viewport_state_create_info{
        .viewportCount = 1,
        .scissorCount  = 1,
    };

    constexpr static vk::PipelineRasterizationStateCreateInfo
        rasterization_state_create_info{
            .lineWidth = 1.f,
        };

    constexpr static vk::PipelineMultisampleStateCreateInfo multisample_state_create_info{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
    };

    constexpr static vk::PipelineDepthStencilStateCreateInfo
        depth_stencil_state_create_info{
            .depthTestEnable       = vk::True,
            .depthWriteEnable      = vk::True,
            .depthCompareOp        = vk::CompareOp::eLess,
            .depthBoundsTestEnable = vk::False,
            .stencilTestEnable     = vk::False,
        };

    using enum vk::ColorComponentFlagBits;
    constexpr static vk::PipelineColorBlendAttachmentState color_blend_attachment_state{
        .colorWriteMask = eR | eG | eB | eA,
    };
    constexpr static vk::PipelineColorBlendStateCreateInfo color_blend_state_create_info{
        .attachmentCount = 1,
        .pAttachments    = &color_blend_attachment_state,
    };

    constexpr static std::array dynamic_states{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };
    constexpr static vk::PipelineDynamicStateCreateInfo dynamic_state{
        .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates    = dynamic_states.data()
    };

    const vk::GraphicsPipelineCreateInfo create_info{
        .stageCount        = static_cast<std::uint32_t>(shader_stage_create_infos.size()),
        .pStages           = shader_stage_create_infos.data(),
        .pVertexInputState = &vertex_input_state_create_info,
        .pInputAssemblyState = &input_assembly_state_create_info,
        .pViewportState      = &viewport_state_create_info,
        .pRasterizationState = &rasterization_state_create_info,
        .pMultisampleState   = &multisample_state_create_info,
        .pDepthStencilState  = &depth_stencil_state_create_info,
        .pColorBlendState    = &color_blend_state_create_info,
        .pDynamicState       = &dynamic_state,
        .layout              = layout
    };

    return kiln::gfx::vulkan::check_result(
        device.logical_device().createGraphicsPipeline(nullptr, create_info)
    );
}

Demo::Demo(
    const kiln::config::Config&        config,
    const vk::raii::Instance&          vulkan_instance,
    const kiln::wsi::Context&          wsi_context,
    const kiln::gfx::renderer::Device& render_device
)
    : window{ create_window(config, vulkan_instance, wsi_context, render_device) },
      pipeline_layout{
          kiln::gfx::vulkan::check_result(
              render_device.logical_device().createPipelineLayout({})
          )   //
      },
      shader_module{
          *kiln::gfx::vulkan::ShaderModule::load_from_file(
              std::filesystem::path{ std::source_location::current().file_name() }
                  .parent_path()
              / "shaders" / "triangle.spv"
          )   //
      },
      pipeline{ create_graphics_pipeline(render_device, pipeline_layout, shader_module) }
{
}

auto DemoPlugin::operator()(kiln::app::App& app) -> void
{
    app.resources().insert(
        Demo{
            app.resources().at<kiln::config::Config>(),
            app.resources().at<vk::raii::Instance>(),
            app.resources().at<kiln::wsi::Context>(),
            app.resources().at<kiln::gfx::renderer::Device>(),
        }
    );
}

auto demo_plugin_injection(
    const kiln::config::Plugin&,
    const kiln::gfx::vulkan::InstancePlugin&,
    const kiln::wsi::Plugin&,
    kiln::gfx::renderer::DevicePlugin& device_plugin
) -> DemoPlugin
{
    device_plugin->enable_features(
        vk::PhysicalDeviceMaintenance5Features{ .maintenance5 = vk::True }
    );

    return DemoPlugin{};
}
