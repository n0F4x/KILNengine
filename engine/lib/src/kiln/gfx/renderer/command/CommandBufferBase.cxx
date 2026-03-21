module;

#include <utility>

module kiln.gfx.renderer.command.CommandBufferBase;

namespace kiln::gfx::renderer {

CommandBufferBase::CommandBufferBase(
    vk::raii::CommandBuffer&&                     command_buffer,
    const util::EnumMask<CommandBufferUsageFlags> usage_flags
)
    : m_command_buffer{ std::move(command_buffer) },
      m_usage_flags{ usage_flags }
{
}

auto CommandBufferBase::get() const noexcept -> const vk::raii::CommandBuffer&
{
    return m_command_buffer;
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto CommandBufferBase::begin() -> void
{
    vk::CommandBufferUsageFlags flags;
    if (!(m_usage_flags & CommandBufferUsageFlags::eReusable))
    {
        flags |= vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    }

    m_command_buffer.begin(
        vk::CommandBufferBeginInfo{
            .flags = flags,
        }
    );
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto CommandBufferBase::end() -> void
{
    m_command_buffer.end();
}

}   // namespace kiln::gfx::renderer
