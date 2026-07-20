module;

#include <algorithm>
#include <cstdint>
#include <memory_resource>
#include <ranges>
#include <utility>
#include <vector>

module kiln.exec.TaskGroup;

import kiln.exec.Access;
import kiln.exec.AccessPattern;
import kiln.util.containers.MoveOnlyFunction;

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

auto TaskGroup::build(
    std::pmr::memory_resource& transient_memory_resource
) && -> Task<void>
{
    return std::move(*this)
        .build(std::pmr::get_default_resource(), transient_memory_resource);
}

[[nodiscard]]
auto make_func(
    const util::MoveOnlyFunction<void()>::allocator_type& allocator,
    std::pmr::vector<Task<void>>&&                        tasks
) -> util::MoveOnlyFunction<void()>
{
    return util::MoveOnlyFunction<void()>{
        std::allocator_arg,
        allocator,
        [x_tasks = std::move(tasks)] mutable -> void
        {
            for (Task<void>& task : x_tasks)
            {
                task();
            }
        },
    };
}

auto TaskGroup::build(
    const Task<void>::allocator_type& allocator,
    std::pmr::memory_resource&        transient_memory_resource
) && -> Task<void>
{
    std::pmr::vector<uint64_t>      accessed_resource_ids{ &transient_memory_resource };
    std::pmr::vector<AccessPattern> access_patterns{ &transient_memory_resource };

    for (const Task<void>& task : m_tasks)
    {
        for (const auto [accessed_resource_id, access_pattern] :
             std::views::zip(task.accessed_resource_ids(), task.access_patterns()))
        {
            const auto resource_id_iter{
                std::ranges::lower_bound(accessed_resource_ids, accessed_resource_id)
            };
            const auto access_pattern_iter{
                std::next(
                    access_patterns.begin(),
                    std::distance(accessed_resource_ids.begin(), resource_id_iter)
                ),
            };

            if (resource_id_iter == accessed_resource_ids.cend()
                or *resource_id_iter != accessed_resource_id)
            {
                accessed_resource_ids.insert(resource_id_iter, accessed_resource_id);
                access_patterns.insert(access_pattern_iter, access_pattern);
            }
            else
            {
                *access_pattern_iter |= access_pattern;
            }
        }
    }

    return Task<void>{
        std::sorted_unique,
        std::allocator_arg,
        allocator,
        make_func(allocator, std::move(m_tasks)),
        std::views::transform(
            // TODO: use `std::views::indices`
            std::views::iota(0uz, accessed_resource_ids.size()),
            [&](const std::size_t index) -> Access
            {
                return Access{
                    .access_pattern = access_patterns[index],
                    .resource_id    = accessed_resource_ids[index],
                };
            }
        ),
    };
}

}   // namespace kiln::exec
