module;

#include <functional>

export module kiln.gfx.renderer.command.TransferQueueRef;

import vulkan_hpp;

import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class TransferQueueRef {
public:
    explicit TransferQueueRef(
        vulkan::QueueFamilyIndex queue_family_index,
        const vk::raii::Queue&   queue_ref
    );

    [[nodiscard]]
    auto family_index() const noexcept -> vulkan::QueueFamilyIndex;

    auto submit(
        const TransferCommandBuffer& one_time_transfer_command_buffer,
        const SubmitInfo&            info = {}
    ) const -> void;

protected:
    [[nodiscard]]
    auto get() const noexcept -> const vk::raii::Queue&;

private:
    vulkan::QueueFamilyIndex                      m_family_index;
    std::reference_wrapper<const vk::raii::Queue> m_queue_ref;
};

}   // namespace kiln::gfx::renderer
