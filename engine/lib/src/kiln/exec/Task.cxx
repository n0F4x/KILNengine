module;

#include <span>
#include <utility>

module kiln.exec.Task;

namespace kiln::exec {

Task::Task(Task&& other, const allocator_type& allocator)
    : m_accessed_types{ std::move(other.m_accessed_types), allocator },
      m_access_patterns{ std::move(other.m_access_patterns), allocator },
      m_invoke{ std::move(other.m_invoke), allocator }
{
}

auto Task::get_allocator() const noexcept -> allocator_type
{
    return m_accessed_types.get_allocator();
}

auto Task::accessed_types() const noexcept -> std::span<const uint64_t>
{
    return m_accessed_types;
}

auto Task::access_patterns() const noexcept -> std::span<const AccessPattern>
{
    return m_access_patterns;
}

auto Task::operator()() -> void
{
    m_invoke();
}

}   // namespace kiln::exec
