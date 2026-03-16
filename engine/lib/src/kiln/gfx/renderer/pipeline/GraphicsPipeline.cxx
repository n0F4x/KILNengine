module;

#include <span>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.pipeline.GraphicsPipeline;

import vulkan_hpp;

import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.pipeline.ShaderModule;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.contracts;

namespace kiln::gfx::renderer {

[[nodiscard]]
auto create_graphics_pipeline(
    const Device&                     device,
    const vk::raii::PipelineLayout&   layout,
    const ShaderModule&               vertex_shader_module,
    const ShaderModule&               fragment_shader_module,
    const std::span<const vk::Format> color_formats,
    const vk::Format                  depth_format
) -> vk::raii::Pipeline
{
    PRECOND(device.capabilities().contains_features(
        vk::PhysicalDeviceMaintenance5Features{ .maintenance5 = vk::True }
    ));
    PRECOND(device.capabilities().contains_features(
        vk::PhysicalDeviceDynamicRenderingFeatures{ .dynamicRendering = vk::True }
    ));

    const vk::PipelineRenderingCreateInfo pipeline_rendering_create_info{
        .colorAttachmentCount    = static_cast<uint32_t>(color_formats.size()),
        .pColorAttachmentFormats = color_formats.data(),
        .depthAttachmentFormat   = depth_format,
    };

    const vk::ShaderModuleCreateInfo vertex_shader_module_create_info{
        .codeSize = vertex_shader_module.code().size_bytes(),
        .pCode    = vertex_shader_module.code().data(),
    };
    const vk::ShaderModuleCreateInfo fragment_shader_module_create_info{
        .codeSize = fragment_shader_module.code().size_bytes(),
        .pCode    = fragment_shader_module.code().data(),
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
        .pNext             = &pipeline_rendering_create_info,
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

    return vulkan::check_result(
        device.logical_device().createGraphicsPipeline(nullptr, create_info)
    );
}

GraphicsPipeline::GraphicsPipeline(
    const Device&                     device,
    const vk::raii::PipelineLayout&   layout,
    const ShaderModule&               vertex_shader_module,
    const ShaderModule&               fragment_shader_module,
    const std::span<const vk::Format> color_formats,
    const vk::Format                  depth_format
)
    : m_pipeline{
          create_graphics_pipeline(
              device,
              layout,
              vertex_shader_module,
              fragment_shader_module,
              color_formats,
              depth_format
          )   //
      }
{
}

auto GraphicsPipeline::get() const noexcept -> const vk::raii::Pipeline&
{
    return m_pipeline;
}

}   // namespace kiln::gfx::renderer
