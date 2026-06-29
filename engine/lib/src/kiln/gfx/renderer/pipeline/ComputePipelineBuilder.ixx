module;

#include <functional>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.pipeline.ComputePipelineBuilder;

import vulkan;

import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.pipeline.ComputePipeline;
import kiln.gfx.renderer.pipeline.ShaderModule;

namespace kiln::gfx::renderer {

export class ComputePipelineBuilder {
public:
    explicit ComputePipelineBuilder(
        [[kiln_lifetimebound]] const vk::raii::PipelineLayout& layout,
        [[kiln_lifetimebound]] const ShaderModule&             shader_module
    );


    [[nodiscard]]
    auto build(const Device& device) const -> ComputePipeline;

private:
    std::reference_wrapper<const vk::raii::PipelineLayout> m_layout;
    std::reference_wrapper<const ShaderModule>             m_shader_module;
};

}   // namespace kiln::gfx::renderer
