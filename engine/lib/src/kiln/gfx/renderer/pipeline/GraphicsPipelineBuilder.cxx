module;

#include <array>
#include <cstdint>
#include <span>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.pipeline.GraphicsPipelineBuilder;

import vulkan;

import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.pipeline.ShaderModule;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.contracts;

namespace kiln::gfx::renderer {

GraphicsPipelineBuilder::GraphicsPipelineBuilder(
    const vk::raii::PipelineLayout& layout,
    const ShaderModule&             vertex_shader_module,
    const ShaderModule&             fragment_shader_module
)
    : m_layout{ layout },
      m_vertex_shader_module{ vertex_shader_module },
      m_fragment_shader_module{ fragment_shader_module }
{
}

auto GraphicsPipelineBuilder::build(const Device& device) const -> GraphicsPipeline
{
    PRECOND(device.capabilities().contains_features(
        vk::PhysicalDeviceMaintenance5Features{ .maintenance5 = vk::True }
    ));
    PRECOND(device.capabilities().contains_features(
        vk::PhysicalDeviceDynamicRenderingFeatures{ .dynamicRendering = vk::True }
    ));

    const vk::ShaderModuleCreateInfo vertex_shader_module_create_info{
        .codeSize = m_vertex_shader_module.get().code().size_bytes(),
        .pCode    = m_vertex_shader_module.get().code().data(),
    };
    const vk::ShaderModuleCreateInfo fragment_shader_module_create_info{
        .codeSize = m_fragment_shader_module.get().code().size_bytes(),
        .pCode    = m_fragment_shader_module.get().code().data(),
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

    static const vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info{
        .cullMode  = m_cull_mode,
        .lineWidth = 1.f,
    };

    constexpr static vk::PipelineMultisampleStateCreateInfo multisample_state_create_info{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
    };

    const bool depth_enabled{ vk::hasDepthComponent(m_depth_format) };
    const vk::PipelineDepthStencilStateCreateInfo depth_stencil_state_create_info{
        .depthTestEnable       = depth_enabled,
        .depthWriteEnable      = depth_enabled,
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

    const vk::PipelineRenderingCreateInfo pipeline_rendering_create_info{
        .colorAttachmentCount    = static_cast<uint32_t>(m_color_formats.size()),
        .pColorAttachmentFormats = m_color_formats.data(),
        .depthAttachmentFormat   = m_depth_format,
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
        .layout              = m_layout.get()
    };

    return GraphicsPipeline{
        vulkan::check_result(
            device.logical_device().createGraphicsPipeline(nullptr, create_info)
        ),
    };
}

}   // namespace kiln::gfx::renderer
