module;

#include <memory_resource>
#include <vector>

export module kiln.gfx.renderer.command.SubmitInfo;

import vulkan;

import kiln.util.containers.OptionalRef;

namespace kiln::gfx::renderer {

export class SubmitInfo {
public:
    using allocator_type           = std::pmr::polymorphic_allocator<>;
    using WaitSemaphoreContainer   = std::pmr::vector<vk::SemaphoreSubmitInfo>;
    using SignalSemaphoreContainer = std::pmr::vector<vk::SemaphoreSubmitInfo>;


    SubmitInfo(const SubmitInfo&, const allocator_type&);
    SubmitInfo(SubmitInfo&&, const allocator_type&);

    explicit SubmitInfo() = default;
    explicit SubmitInfo(const allocator_type& allocator);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto wait_semaphores() noexcept -> WaitSemaphoreContainer&;
    [[nodiscard]]
    auto wait_semaphores() const noexcept -> const WaitSemaphoreContainer&;
    [[nodiscard]]
    auto signal_semaphores() noexcept -> SignalSemaphoreContainer&;
    [[nodiscard]]
    auto signal_semaphores() const noexcept -> const SignalSemaphoreContainer&;
    [[nodiscard]]
    auto fence() -> util::OptionalRef<const vk::Fence>&;
    [[nodiscard]]
    auto fence() const -> const util::OptionalRef<const vk::Fence>&;

private:
    WaitSemaphoreContainer             m_wait_semaphores;
    SignalSemaphoreContainer           m_signal_semaphores;
    util::OptionalRef<const vk::Fence> m_fence;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

SubmitInfo::SubmitInfo(const SubmitInfo& other, const allocator_type& allocator)
    : m_wait_semaphores{ other.m_wait_semaphores, allocator },
      m_signal_semaphores{ other.m_signal_semaphores, allocator },
      m_fence{ other.m_fence }
{
}

SubmitInfo::SubmitInfo(SubmitInfo&& other, const allocator_type& allocator)
    : m_wait_semaphores{ std::move(other.m_wait_semaphores), allocator },
      m_signal_semaphores{ std::move(other.m_signal_semaphores), allocator },
      m_fence{ other.m_fence }
{
}

SubmitInfo::SubmitInfo(const allocator_type& allocator)
    : m_wait_semaphores{ allocator },
      m_signal_semaphores{ allocator }
{
}

auto SubmitInfo::get_allocator() const noexcept -> allocator_type
{
    return m_wait_semaphores.get_allocator();
}

auto SubmitInfo::wait_semaphores() noexcept -> WaitSemaphoreContainer&
{
    return m_wait_semaphores;
}

auto SubmitInfo::wait_semaphores() const noexcept -> const WaitSemaphoreContainer&
{
    return m_wait_semaphores;
}

auto SubmitInfo::signal_semaphores() noexcept -> SignalSemaphoreContainer&
{
    return m_signal_semaphores;
}

auto SubmitInfo::signal_semaphores() const noexcept -> const SignalSemaphoreContainer&
{
    return m_signal_semaphores;
}

auto SubmitInfo::fence() -> util::OptionalRef<const vk::Fence>&
{
    return m_fence;
}

auto SubmitInfo::fence() const -> const util::OptionalRef<const vk::Fence>&
{
    return m_fence;
}

}   // namespace kiln::gfx::renderer
