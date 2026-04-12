module;

#include <cstdint>

export module kiln.gfx.renderer.command.QueueBase;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandBufferBase;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.renderer.device.QueueType;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class QueueBase {
public:
    explicit QueueBase(
        QueueType                type,
        vulkan::QueueFamilyIndex family_index,
        vk::QueueFlags           flags,
        uint32_t                 index,
        vk::raii::Queue&&        queue
    );

    [[nodiscard]]
    auto type() const noexcept -> QueueType;
    [[nodiscard]]
    auto family_index() const noexcept -> vulkan::QueueFamilyIndex;
    [[nodiscard]]
    auto flags() const noexcept -> vk::QueueFlags;
    [[nodiscard]]
    auto index() const noexcept -> uint32_t;
    [[nodiscard]]
    auto get() noexcept -> const vk::raii::Queue&;

protected:
    auto submit(const CommandBufferBase& command_buffer, const SubmitInfo& info) -> void;

private:
    QueueType                m_type;
    vulkan::QueueFamilyIndex m_family_index;
    vk::QueueFlags           m_flags;
    uint32_t                 m_index;
    vk::raii::Queue          m_queue;
};

}   // namespace kiln::gfx::renderer
