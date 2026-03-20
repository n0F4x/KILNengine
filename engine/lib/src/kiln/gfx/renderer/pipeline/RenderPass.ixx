module;

#include <optional>
#include <span>
#include <vector>

export module kiln.gfx.renderer.pipeline.RenderPass;

import vulkan_hpp;

import kiln.gfx.renderer.pipeline.ColorAttachment;
import kiln.gfx.renderer.pipeline.DepthAttachment;

namespace kiln::gfx::renderer {

export class RenderPass {
public:
    explicit RenderPass(
        vk::Rect2D                       render_area,
        std::span<const ColorAttachment> color_attachments = {},
        const DepthAttachment*           depth_attachment  = nullptr
    );

    [[nodiscard]]
    auto get() const noexcept -> const vk::RenderingInfo&;

private:
    // TODO: use std::inplace_vector
    std::vector<vk::RenderingAttachmentInfo>   m_color_attachments;
    std::optional<vk::RenderingAttachmentInfo> m_depth_attachment;
    vk::RenderingInfo                          m_rendering_info;
};

}   // namespace kiln::gfx::renderer
