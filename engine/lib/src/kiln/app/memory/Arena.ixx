module;

#include <memory>
#include <memory_resource>

export module kiln.app.memory.Arena;

import kiln.util.Deleter;

namespace kiln::app {

export class Arena {
public:
    [[nodiscard]]
    auto pool_resource() -> std::pmr::memory_resource&
    {
        return *m_pool_resource;
    }

    [[nodiscard]]
    auto transitive_resource() -> std::pmr::memory_resource&
    {
        return *m_pool_resource;
    }

private:
    // TODO: use std::indirect
    std::unique_ptr<std::pmr::memory_resource> m_monotonic_resource{
        std::make_unique<std::pmr::monotonic_buffer_resource>()
    };
    std::pmr::polymorphic_allocator<> m_monotonic_allocator{ m_monotonic_resource.get() };
    std::unique_ptr<std::pmr::memory_resource, util::Deleter> m_pool_resource{
        m_monotonic_allocator.new_object<std::pmr::unsynchronized_pool_resource>(
            m_monotonic_resource.get()
        ),
        util::Deleter{ m_monotonic_allocator }
    };
    std::unique_ptr<std::pmr::memory_resource, util::Deleter> m_transitive_memory_pool{
        m_monotonic_allocator.new_object<std::pmr::unsynchronized_pool_resource>(
            m_monotonic_resource.get()
        ),
        util::Deleter{ m_monotonic_allocator }
    };
};

}   // namespace kiln::app
