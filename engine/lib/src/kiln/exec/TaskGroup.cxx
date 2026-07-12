module;

#include <memory_resource>
#include <utility>

module kiln.exec.TaskGroup;

namespace kiln::exec {

TaskGroup::TaskGroup(TaskGroup&& other, const allocator_type& allocator)
    : m_tasks{ std::move(other.m_tasks), allocator }
{
}

TaskGroup::TaskGroup(std::allocator_arg_t, const allocator_type& allocator)
    : m_tasks{ allocator }
{
}

auto TaskGroup::get_allocator() const noexcept -> allocator_type
{
    return m_tasks.get_allocator();
}

auto TaskGroup::size() const noexcept -> size_t
{
    return m_tasks.size();
}

auto TaskGroup::reserve(const size_t size) -> void
{
    m_tasks.reserve(size);
}

auto TaskGroup::push(Task<void>&& task) -> void
{
    m_tasks.push_back(std::move(task));
}

}   // namespace kiln::exec
