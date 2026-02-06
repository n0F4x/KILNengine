module;

#include <memory>
#include <memory_resource>

export module kiln.app.memory.Arena;

namespace kiln::app {

export class Arena {
public:
    Arena() = default;

    Arena(
        std::unique_ptr<std::pmr::memory_resource>&& app_local_monotonic_resource,
        std::unique_ptr<std::pmr::memory_resource>&& app_local_pool_resource
    )
        : m_app_local_monotonic_resource{ std::move(app_local_monotonic_resource) },
          m_app_local_pool_resource{ std::move(app_local_pool_resource) }
    {
    }

    [[nodiscard]]
    auto app_local_monotonic_resource() -> std::pmr::memory_resource&
    {
        return *m_app_local_monotonic_resource;
    }

    [[nodiscard]]
    auto app_local_pool_resource() -> std::pmr::memory_resource&
    {
        return *m_app_local_pool_resource;
    }

private:
    // TODO: use std::indirect
    std::unique_ptr<std::pmr::memory_resource> m_app_local_monotonic_resource;
    std::unique_ptr<std::pmr::memory_resource> m_app_local_pool_resource;
};

}   // namespace kiln::app
