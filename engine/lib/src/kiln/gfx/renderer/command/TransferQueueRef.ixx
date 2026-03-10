module;

#include <functional>

export module kiln.gfx.renderer.command.TransferQueueRef;

import vulkan_hpp;

import kiln.gfx.renderer.command.OneTimeTransferCommandBuffer;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class TransferQueueRef {
public:
    explicit TransferQueueRef(
        const vk::raii::Queue&   queue_ref,
        vulkan::QueueFamilyIndex queue_family_index
    );

    [[nodiscard]]
    auto family_index() const noexcept -> vulkan::QueueFamilyIndex;

    auto submit(
        const OneTimeTransferCommandBuffer& one_time_transfer_command_buffer,
        const SubmitInfo&                   info = {}
    ) const -> void;

private:
    std::reference_wrapper<const vk::raii::Queue> m_queue_ref;
    vulkan::QueueFamilyIndex                      m_family_index;
};

}   // namespace kiln::gfx::renderer
