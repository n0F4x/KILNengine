module;

#include <optional>
#include <span>
#include <vector>

export module kiln.gfx.renderer.pipeline.RenderPass;

import vulkan_hpp;

import kiln.gfx.renderer.pipeline.ColorAttachment;
import kiln.gfx.renderer.pipeline.DepthAttachment;
import kiln.util.containers.OptionalRef;

namespace kiln::gfx::renderer {

export class RenderPass {
public:
    explicit RenderPass(
        vk::Rect2D                               render_area,
        std::span<const ColorAttachment>         color_attachments = {},
        util::OptionalRef<const DepthAttachment> depth_attachment  = {}
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
