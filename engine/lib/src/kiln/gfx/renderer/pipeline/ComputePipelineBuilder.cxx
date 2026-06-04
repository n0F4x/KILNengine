module;

#include <functional>

module kiln.gfx.renderer.pipeline.ComputePipelineBuilder;

import vulkan_hpp;

import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.pipeline.ComputePipeline;
import kiln.gfx.renderer.pipeline.ShaderModule;
import kiln.gfx.vulkan.result.check_result;

namespace kiln::gfx::renderer {

ComputePipelineBuilder::ComputePipelineBuilder(
    const vk::raii::PipelineLayout& layout,
    const ShaderModule&             shader_module
)
    : m_layout{ layout },
      m_shader_module{ shader_module }
{
}

auto ComputePipelineBuilder::build(const Device& device) const -> ComputePipeline
{
    const vk::ShaderModuleCreateInfo shader_module_create_info{
        .codeSize = m_shader_module.get().code().size_bytes(),
        .pCode    = m_shader_module.get().code().data(),
    };
    const vk::PipelineShaderStageCreateInfo shader_stage_create_info{
        .pNext = &shader_module_create_info,
        .stage = vk::ShaderStageFlagBits::eCompute,
        .pName = "main",
    };

    const vk::ComputePipelineCreateInfo create_info{
        .stage  = shader_stage_create_info,
        .layout = m_layout.get(),
    };

    return ComputePipeline{
        vulkan::check_result(
            device.logical_device().createComputePipeline(nullptr, create_info)
        ),
    };
}

}   // namespace kiln::gfx::renderer
