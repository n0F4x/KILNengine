module;

#include <utility>

export module kiln.gfx.renderer.pipeline.GraphicsPipeline;

import vulkan_hpp;

namespace kiln::gfx::renderer {

export class GraphicsPipeline {
public:
    explicit GraphicsPipeline(vk::raii::Pipeline&& pipeline)
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
