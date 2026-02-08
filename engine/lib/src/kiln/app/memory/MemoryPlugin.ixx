module;

#include <functional>

export module kiln.app.memory.MemoryPlugin;

import kiln.app.App;
import kiln.app.memory.Arena;

namespace kiln::app {

export class MemoryPlugin {
public:
    MemoryPlugin(Arena& app_arena, Arena& builder_arena) noexcept
        : m_app_arena_ref{ app_arena },
          m_builder_arena_ref{ builder_arena }
    {
    }

    [[nodiscard]]
    auto app_local_monotonic_resource() const -> std::pmr::memory_resource&
    {
        return m_app_arena_ref.get().monotonic_resource();
    }

    [[nodiscard]]
    auto app_local_pool_resource() const -> std::pmr::memory_resource&
    {
        return m_app_arena_ref.get().pool_resource();
    }

    [[nodiscard]]
    auto builder_local_monotonic_resource() const -> std::pmr::memory_resource&
    {
        return m_builder_arena_ref.get().monotonic_resource();
    }

    [[nodiscard]]
    auto builder_local_pool_resource() const -> std::pmr::memory_resource&
    {
        return m_builder_arena_ref.get().pool_resource();
    }

    static auto operator()(App&) noexcept -> void {}

private:
    std::reference_wrapper<Arena> m_app_arena_ref;
    std::reference_wrapper<Arena> m_builder_arena_ref;
};

}   // namespace kiln::app
