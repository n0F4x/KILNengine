module;

#include <vector>

export module kiln.gfx.renderer.command.GraphicsCommandBuffer;

import vulkan_hpp;

import kiln.util.Strong;

namespace kiln::gfx::renderer {

export class GraphicsCommandBuffer {
public:
    GraphicsCommandBuffer(
        std::vector<vk::raii::CommandBuffer>&& command_buffers,
        uint8_t                                number_of_frames
    );

    auto begin() -> void;
    auto end() -> void;

private:
    // TODO: use std::inplace_vector
    std::vector<vk::raii::CommandBuffer> m_command_buffers;
    uint8_t                              m_frame_index{};
    uint8_t                              m_number_of_frames;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

GraphicsCommandBuffer::GraphicsCommandBuffer(
    std::vector<vk::raii::CommandBuffer>&& command_buffers,
    const uint8_t                          number_of_frames
)
    : m_command_buffers{ std::move(command_buffers) },
      m_number_of_frames{ number_of_frames }
{
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto GraphicsCommandBuffer::begin() -> void
{
    m_command_buffers[m_frame_index].begin(
        vk::CommandBufferBeginInfo{
            .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
        }
    );
}

auto GraphicsCommandBuffer::end() -> void
{
    m_command_buffers[m_frame_index].end();
    m_frame_index = (m_frame_index + 1) % m_number_of_frames;
}

}   // namespace kiln::gfx::renderer
