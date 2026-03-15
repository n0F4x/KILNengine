export module kiln.gfx.renderer.pipeline.RenderPass;

import vulkan_hpp;

namespace kiln::gfx::renderer {

export class RenderPass {
private:
    vk::RenderingInfo m_rendering_info;
};

}   // namespace kiln::gfx::renderer
