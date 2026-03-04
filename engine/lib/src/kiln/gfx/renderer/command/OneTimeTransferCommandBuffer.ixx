export module kiln.gfx.renderer.command.OneTimeTransferCommandBuffer;

import vulkan_hpp;

import kiln.gfx.renderer.memory.Buffer;
import kiln.util.Strong;

namespace kiln::gfx::renderer {

export class OneTimeTransferCommandBuffer {
public:
    explicit OneTimeTransferCommandBuffer(vk::raii::CommandBuffer&& command_buffer);

    [[nodiscard]]
    auto get() const noexcept -> const vk::raii::CommandBuffer&;

    auto begin() -> void;
    auto end() -> void;

    auto enqueue_buffer_copy(const Buffer& source, const Buffer& destination) -> void;

private:
    vk::raii::CommandBuffer m_command_buffer;
};

}   // namespace kiln::gfx::renderer
