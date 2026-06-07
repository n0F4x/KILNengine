module;

#include <utility>

module kiln.gfx.renderer.command.QueueRefBase;

namespace kiln::gfx::renderer {

auto QueueRefBase::family_index() const noexcept -> vulkan::QueueFamilyIndex
{
    return m_queue_ref.get().family_index();
}

auto QueueRefBase::flags() const noexcept -> vk::QueueFlags
{
    return m_queue_ref.get().flags();
}

auto QueueRefBase::index() const noexcept -> uint32_t
{
    return m_queue_ref.get().index();
}

auto QueueRefBase::get() const noexcept -> Queue&
{
    return m_queue_ref.get();
}

QueueRefBase::QueueRefBase(Queue& queue) : m_queue_ref{ queue } {}

auto QueueRefBase::submit(
    const CommandBufferBase& command_buffer,
    const SubmitInfo&        info
) const -> void
{
    m_queue_ref.get().submit(command_buffer, info);
}

}   // namespace kiln::gfx::renderer
