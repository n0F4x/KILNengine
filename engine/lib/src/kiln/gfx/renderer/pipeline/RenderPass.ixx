module;

#include <cstdint>
#include <span>

export module kiln.gfx.renderer.pipeline.RenderPass;

import vulkan_hpp;

namespace kiln::gfx::renderer {

export class RenderPass {
public:
    explicit RenderPass(
        vk::Rect2D                                   render_area,
        std::span<const vk::RenderingAttachmentInfo> color_attachments = {},
        const vk::RenderingAttachmentInfo*           depth_attachment  = nullptr
    );

    [[nodiscard]]
    auto get() const noexcept -> const vk::RenderingInfo&;

private:
    vk::RenderingInfo m_rendering_info;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

RenderPass::RenderPass(
    const vk::Rect2D                             render_area,
    std::span<const vk::RenderingAttachmentInfo> color_attachments,
    const vk::RenderingAttachmentInfo*           depth_attachment
)
    : m_rendering_info{
          .renderArea           = render_area,
          .layerCount           = 1,
          .colorAttachmentCount = static_cast<uint32_t>(color_attachments.size()),
          .pColorAttachments    = color_attachments.data(),
          .pDepthAttachment     = depth_attachment,
      }
{
}

auto RenderPass::get() const noexcept -> const vk::RenderingInfo&
{
    return m_rendering_info;
}

}   // namespace kiln::gfx::renderer
