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
auto CommandBufferBase::begin_recording() -> void
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
auto CommandBufferBase::end_recording() -> void
{
    m_command_buffer.end();
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto CommandBufferBase::record_barrier(const DependencyInfo& dependency_info) -> void
{
    const vk::DependencyInfo transformed_dependency_info{
        .memoryBarrierCount =
            static_cast<uint32_t>(dependency_info.memory_barriers.size()),
        .pMemoryBarriers = dependency_info.memory_barriers.data(),
        .bufferMemoryBarrierCount =
            static_cast<uint32_t>(dependency_info.buffer_memory_barriers.size()),
        .pBufferMemoryBarriers = dependency_info.buffer_memory_barriers.data(),
        .imageMemoryBarrierCount =
            static_cast<uint32_t>(dependency_info.image_memory_barriers.size()),
        .pImageMemoryBarriers = dependency_info.image_memory_barriers.data(),
    };
    m_command_buffer.pipelineBarrier2(transformed_dependency_info);
}

}   // namespace kiln::gfx::renderer
