module;

#include <functional>

export module kiln.app.memory.MemoryPluginInjection;

import kiln.app.memory.Arena;
import kiln.app.memory.MemoryPlugin;

namespace kiln::app {

export class MemoryPluginInjection {
public:
    MemoryPluginInjection(Arena& app_arena, Arena& builder_arena) noexcept
        : m_app_arena_ref{ app_arena },
          m_builder_arena_ref{ builder_arena }
    {
    }

    [[nodiscard]]
    auto operator()() const -> MemoryPlugin
    {
        return MemoryPlugin{ m_app_arena_ref, m_builder_arena_ref };
    }

private:
    std::reference_wrapper<Arena> m_app_arena_ref;
    std::reference_wrapper<Arena> m_builder_arena_ref;
};

}   // namespace kiln::app
