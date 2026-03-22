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
    auto pool_resource() -> std::pmr::memory_resource&
    {
        return *m_pool_resource;
    }

    [[nodiscard]]
    // ReSharper disable once CppMemberFunctionMayBeConst
    auto transitive_resource() -> std::pmr::memory_resource&
    {
        return *m_pool_resource;
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
    std::unique_ptr<std::pmr::memory_resource, util::Deleter> m_transitive_memory_pool{
        std::pmr::polymorphic_allocator{ m_monotonic_resource.get() }
            .new_object<std::pmr::unsynchronized_pool_resource>(m_monotonic_resource.get()),
        util::Deleter{ m_monotonic_resource.get() }
    };
};

}   // namespace kiln::app
