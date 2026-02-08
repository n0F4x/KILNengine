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
    auto app_local_arena() const noexcept -> Arena&
    {
        return m_app_arena_ref;
    }

    [[nodiscard]]
    auto builder_local_arena() const noexcept -> Arena&
    {
        return m_builder_arena_ref;
    }

    // This function is required to meet the plugin concept
    static auto operator()(App&) noexcept -> void {}

private:
    std::reference_wrapper<Arena> m_app_arena_ref;
    std::reference_wrapper<Arena> m_builder_arena_ref;
};

}   // namespace kiln::app
