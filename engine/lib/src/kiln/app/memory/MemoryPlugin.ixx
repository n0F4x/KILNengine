module;

#include <memory_resource>

export module kiln.app.memory.MemoryPlugin;

import kiln.app.App;
import kiln.app.memory.Arena;

namespace kiln::app {

export class MemoryPlugin {
public:
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

    auto operator()(App& app) && -> void
    {
        app.arena() = Arena{
            std::move(m_app_local_monotonic_resource),
            std::move(m_app_local_pool_resource)   //
        };
    }

private:
    // TODO: use std::indirect
    std::unique_ptr<std::pmr::memory_resource> m_app_local_monotonic_resource{
        std::make_unique<std::pmr::monotonic_buffer_resource>()
    };
    std::unique_ptr<std::pmr::memory_resource> m_app_local_pool_resource{
        std::make_unique<std::pmr::unsynchronized_pool_resource>()
    };
};

}   // namespace kiln::app
