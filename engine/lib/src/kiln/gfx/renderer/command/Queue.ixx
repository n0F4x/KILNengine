module;

#include <cstdint>

export module kiln.gfx.renderer.command.Queue;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandBufferBase;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class Queue {
public:
    explicit Queue(
        const Device&            device,
        vulkan::QueueFamilyIndex family_index,
        vk::QueueFlags           flags,
        uint32_t                 index
    );

    [[nodiscard]]
    auto family_index() const noexcept -> vulkan::QueueFamilyIndex;
    [[nodiscard]]
    auto flags() const noexcept -> vk::QueueFlags;
    [[nodiscard]]
    auto index() const noexcept -> uint32_t;
    [[nodiscard]]
    auto supports_presentation() const noexcept -> uint32_t;
    [[nodiscard]]
    auto get() noexcept -> const vk::raii::Queue&;

    auto submit(const CommandBufferBase& command_buffer, const SubmitInfo& info) -> void;

private:
    vulkan::QueueFamilyIndex m_family_index;
    vk::QueueFlags           m_flags;
    uint32_t                 m_index;
    bool                     m_supports_presentation;
    vk::raii::Queue          m_queue;
};

}   // namespace kiln::gfx::renderer
