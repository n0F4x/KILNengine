export module kiln.gfx.renderer.command.TransferCommandBuffer;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandBufferUsageFlags;
import kiln.gfx.renderer.memory.Buffer;
import kiln.util.EnumMask;

namespace kiln::gfx::renderer {

export class TransferCommandBuffer {
public:
    TransferCommandBuffer(
        vk::raii::CommandBuffer&&               command_buffer,
        util::EnumMask<CommandBufferUsageFlags> usage_flags
    );

    [[nodiscard]]
    auto get() const noexcept -> const vk::raii::CommandBuffer&;

    auto begin() -> void;
    auto end() -> void;

    auto enqueue_buffer_copy(const Buffer& source, const Buffer& destination) -> void;

private:
    vk::raii::CommandBuffer                 m_command_buffer;
    util::EnumMask<CommandBufferUsageFlags> m_usage_flags;
};

}   // namespace kiln::gfx::renderer
