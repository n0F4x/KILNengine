module;

#include <functional>

export module kiln.gfx.renderer.command.QueueRefBase;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandBufferBase;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class QueueRefBase {
public:
    explicit QueueRefBase(
        vulkan::QueueFamilyIndex queue_family_index,
        const vk::raii::Queue&   queue_ref
    );

    [[nodiscard]]
    auto get() const noexcept -> const vk::raii::Queue&;

    [[nodiscard]]
    auto family_index() const noexcept -> vulkan::QueueFamilyIndex;

protected:
    auto submit(const CommandBufferBase& command_buffer, const SubmitInfo& info) const
        -> void;

private:
    vulkan::QueueFamilyIndex                      m_family_index;
    std::reference_wrapper<const vk::raii::Queue> m_queue_ref;
};

}   // namespace kiln::gfx::renderer
