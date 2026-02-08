module;

#include <memory>
#include <memory_resource>

export module kiln.app.memory.Arena;

namespace kiln::app {

export class Arena {
public:
    [[nodiscard]]
    auto monotonic_resource() -> std::pmr::memory_resource&
    {
        return *m_monotonic_resource;
    }

    [[nodiscard]]
    auto pool_resource() -> std::pmr::memory_resource&
    {
        return *m_pool_resource;
    }

private:
    // TODO: use std::indirect
    std::unique_ptr<std::pmr::memory_resource> m_monotonic_resource{
        std::make_unique<std::pmr::monotonic_buffer_resource>()
    };
    std::unique_ptr<std::pmr::memory_resource> m_pool_resource{
        std::make_unique<std::pmr::unsynchronized_pool_resource>()
    };
};

}   // namespace kiln::app
