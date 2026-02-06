export module kiln.app.memory.MemoryPluginInjection;

import kiln.app.memory.MemoryPlugin;

namespace kiln::app {

export class MemoryPluginInjection {
public:
    [[nodiscard]]
    static auto operator()() -> MemoryPlugin
    {
        return MemoryPlugin{};
    }
};

}   // namespace kiln::app
