module;

#include <memory>
#include <memory_resource>

export module kiln.app.memory.Arena;

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
    // TODO: use std::indirect
    std::unique_ptr<std::pmr::memory_resource> m_monotonic_resource{
        std::make_unique<std::pmr::monotonic_buffer_resource>()
    };
    std::pmr::polymorphic_allocator<> m_monotonic_allocator{ m_monotonic_resource.get() };
    std::unique_ptr<std::pmr::memory_resource> m_pool_resource{
        std::make_unique<std::pmr::unsynchronized_pool_resource>(m_monotonic_resource.get())
    };
    std::unique_ptr<std::pmr::memory_resource> m_transitive_memory_pool{
        std::make_unique<std::pmr::unsynchronized_pool_resource>(m_monotonic_resource.get())
    };
};

}   // namespace kiln::app
