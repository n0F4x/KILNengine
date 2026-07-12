module;

#include <utility>

module kiln.exec.Task;

namespace kiln::exec {

Task::Task(Task&& other, const allocator_type& allocator)
    : m_accesses{ std::move(other.m_accesses), allocator },
      m_invoke{ std::move(other.m_invoke), allocator }
{
}

auto Task::get_allocator() const noexcept -> allocator_type
{
    return m_accesses.keys().get_allocator();
}

auto Task::accesses() const noexcept -> const Accesses&
{
    return m_accesses;
}

auto Task::operator()() -> void
{
    m_invoke();
}

}   // namespace kiln::exec
