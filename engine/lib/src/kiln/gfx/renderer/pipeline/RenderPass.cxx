module;

#include <array>
#include <optional>
#include <span>
#include <vector>

module kiln.gfx.renderer.pipeline.RenderPass;

import vulkan_hpp;

import kiln.gfx.renderer.pipeline.ColorAttachment;
import kiln.gfx.renderer.pipeline.DepthAttachment;

namespace kiln::gfx::renderer {

[[nodiscard]]
auto transform_color_attachments(const std::span<const ColorAttachment> color_attachments)
    -> std::vector<vk::RenderingAttachmentInfo>
{
    std::vector<vk::RenderingAttachmentInfo> result;
    result.reserve(color_attachments.size());

    for (const ColorAttachment& attachment : color_attachments)
    {
        vk::RenderingAttachmentInfo transformed_attachment{
            // .imageView   = attachment.image_view(),
            .imageLayout = vk::ImageLayout::eAttachmentOptimal,
        };

        if (const auto clear_value{ attachment.clear_value() }; clear_value.has_value())
        {
            transformed_attachment.loadOp = vk::AttachmentLoadOp::eClear;
            transformed_attachment.clearValue.color.float32 = std::array{
                clear_value->operator[](0),
                clear_value->operator[](1),
                clear_value->operator[](2),
                clear_value->operator[](3),
            };
        }

        result.push_back(transformed_attachment);
    }

    return result;
}

[[nodiscard]]
auto transform_depth_attachment(const DepthAttachment* depth_attachment)
    -> std::optional<vk::RenderingAttachmentInfo>
{
    std::optional<vk::RenderingAttachmentInfo> result{};

    if (depth_attachment != nullptr)
    {
        result = vk::RenderingAttachmentInfo{
            // .imageView   = attachment.image_view(),
            .imageLayout = vk::ImageLayout::eAttachmentOptimal,
        };

        if (const auto clear_value{ depth_attachment->clear_value() };
            clear_value.has_value())
        {
            result->loadOp                        = vk::AttachmentLoadOp::eClear;
            result->clearValue.depthStencil.depth = *clear_value;
        }
    }

    return result;
}

RenderPass::RenderPass(
    const vk::Rect2D                       render_area,
    const std::span<const ColorAttachment> color_attachments,
    const DepthAttachment*                 depth_attachment
)
    : m_color_attachments{ transform_color_attachments(color_attachments) },
      m_depth_attachment{ transform_depth_attachment(depth_attachment) },
      m_rendering_info{
          .renderArea           = render_area,
          .layerCount           = 1,
          .colorAttachmentCount = static_cast<uint32_t>(color_attachments.size()),
          .pColorAttachments    = m_color_attachments.data(),
          .pDepthAttachment     = m_depth_attachment
                                  .transform(
                                      [](const vk::RenderingAttachmentInfo& value)
                                          -> const vk::RenderingAttachmentInfo*
                                      { return &value; }
                                  )
                                  .value_or(nullptr),
      }
{
}

auto RenderPass::get() const noexcept -> const vk::RenderingInfo&
{
    return m_rendering_info;
}

}   // namespace kiln::gfx::renderer
