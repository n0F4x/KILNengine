module;

#include <functional>

export module kiln.gfx.renderer.device.TransferQueueRef;

import vulkan_hpp;

import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.gfx.renderer.device.SubmitInfo;
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
        const TransferCommandBuffer& one_time_transfer_command_buffer,
        const SubmitInfo&            info = {}
    ) const -> void;

protected:
    [[nodiscard]]
    auto get() const noexcept -> const vk::raii::Queue&;

private:
    std::reference_wrapper<const vk::raii::Queue> m_queue_ref;
    vulkan::QueueFamilyIndex                      m_family_index;
};

}   // namespace kiln::gfx::renderer
