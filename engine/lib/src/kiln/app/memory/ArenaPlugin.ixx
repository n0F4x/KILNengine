module;

#include <functional>

export module kiln.app.memory.ArenaPlugin;

import kiln.app.memory.Arena;
import kiln.app.plugin.PluginInterface;

namespace kiln::app {

export class ArenaPlugin : public PluginInterface {
public:
    explicit ArenaPlugin(Arena& arena) noexcept : m_arena_ref{ arena } {}

    [[nodiscard]]
    // ReSharper disable once CppMemberFunctionMayBeConst
    auto arena() noexcept -> Arena&
    {
        return m_arena_ref;
    }

private:
    std::reference_wrapper<Arena> m_arena_ref;
};

}   // namespace kiln::app
