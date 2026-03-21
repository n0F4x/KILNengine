export module kiln.gfx.renderer.command.CommandBufferBase;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandBufferUsageFlags;
import kiln.gfx.renderer.command.DependencyInfo;
import kiln.gfx.renderer.memory.Buffer;
import kiln.util.EnumMask;

namespace kiln::gfx::renderer {

export class CommandBufferBase {
public:
    CommandBufferBase(
        vk::raii::CommandBuffer&&               command_buffer,
        util::EnumMask<CommandBufferUsageFlags> usage_flags
    );

    [[nodiscard]]
    auto get() const noexcept -> const vk::raii::CommandBuffer&;

    auto begin() -> void;
    auto end() -> void;

    auto barrier(const DependencyInfo& dependency_info) -> void;

private:
    vk::raii::CommandBuffer                 m_command_buffer;
    util::EnumMask<CommandBufferUsageFlags> m_usage_flags;
};

}   // namespace kiln::gfx::renderer
