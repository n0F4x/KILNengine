module;

#include <cstddef>
#include <memory_resource>
#include <vector>

export module kiln.exec.TaskGroup;

import kiln.exec.Task;

namespace kiln::exec {

export class TaskGroup {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    TaskGroup(TaskGroup&&, const allocator_type&);

    explicit TaskGroup() = default;
    explicit TaskGroup(std::allocator_arg_t, const allocator_type& allocator);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto size() const noexcept -> std::size_t;


    auto reserve(std::size_t size) -> void;
    auto push(Task&& task) -> void;


    [[nodiscard]]
    auto build() && -> Task;

private:
    std::pmr::vector<Task> m_tasks;
};

}   // namespace kiln::exec
