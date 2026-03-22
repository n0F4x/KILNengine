module;

#include <memory>
#include <memory_resource>

export module kiln.app.memory.Arena;

import kiln.util.Deleter;

namespace kiln::app {

export class Arena {
public:
    [[nodiscard]]
    // ReSharper disable once CppMemberFunctionMayBeConst
    auto pool_allocator() -> std::pmr::polymorphic_allocator<>
    {
        return m_pool_resource.get();
    }

    [[nodiscard]]
    // ReSharper disable once CppMemberFunctionMayBeConst
    auto make_transient_resource() -> std::pmr::monotonic_buffer_resource
    {
        return std::pmr::monotonic_buffer_resource{ m_transient_memory_resource.get() };
    }

private:
    // TODO: use std::polymorphic
    std::unique_ptr<std::pmr::memory_resource, util::Deleter> m_monotonic_resource{
        std::pmr::polymorphic_allocator{ std::pmr::get_default_resource() }
            .new_object<std::pmr::monotonic_buffer_resource>(),
        util::Deleter{ std::pmr::get_default_resource() }
    };
    std::unique_ptr<std::pmr::memory_resource, util::Deleter> m_pool_resource{
        std::pmr::polymorphic_allocator{ m_monotonic_resource.get() }
            .new_object<std::pmr::unsynchronized_pool_resource>(m_monotonic_resource.get()),
        util::Deleter{ m_monotonic_resource.get() }
    };
    std::unique_ptr<std::pmr::memory_resource, util::Deleter> m_transient_memory_resource{
        std::pmr::polymorphic_allocator{ m_monotonic_resource.get() }
            .new_object<std::pmr::monotonic_buffer_resource>(m_monotonic_resource.get()),
        util::Deleter{ m_monotonic_resource.get() }
    };
};

}   // namespace kiln::app
