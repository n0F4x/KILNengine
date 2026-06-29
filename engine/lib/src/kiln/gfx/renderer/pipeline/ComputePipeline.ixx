module;

#include <utility>

export module kiln.gfx.renderer.pipeline.ComputePipeline;

import vulkan;

namespace kiln::gfx::renderer {

export class ComputePipeline {
public:
    explicit ComputePipeline(vk::raii::Pipeline&& pipeline)
        : m_pipeline{ std::move(pipeline) }
    {
    }

    [[nodiscard]]
    auto get() const noexcept -> const vk::raii::Pipeline&
    {
        return m_pipeline;
    }

private:
    vk::raii::Pipeline m_pipeline;
};

}   // namespace kiln::gfx::renderer
