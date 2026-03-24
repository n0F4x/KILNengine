module;

#include <memory_resource>

export module kiln.app.memory.Arena;

import kiln.util.containers.Indirect;

namespace kiln::app {

export class Arena {
public:
    [[nodiscard]]
    auto pool_allocator() -> std::pmr::polymorphic_allocator<>
    {
        return &*m_pool_resource;
    }

    [[nodiscard]]
    auto make_transient_resource() -> std::pmr::monotonic_buffer_resource
    {
        return std::pmr::monotonic_buffer_resource{ &*m_transient_memory_resource };
    }

private:
    util::Indirect<std::pmr::monotonic_buffer_resource> m_monotonic_resource{
        std::in_place
    };
    util::Indirect<std::pmr::unsynchronized_pool_resource> m_pool_resource{
        std::allocator_arg,
        &*m_monotonic_resource,
        &*m_monotonic_resource
    };
    util::Indirect<std::pmr::unsynchronized_pool_resource> m_transient_memory_resource{
        std::allocator_arg,
        &*m_monotonic_resource,
        &*m_monotonic_resource
    };
};

}   // namespace kiln::app
