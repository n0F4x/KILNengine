export module kiln.asset.AssetManagerPlugin;

import kiln.app.memory.Arena;
import kiln.app.plugin.PluginInterface;
import kiln.asset.AssetManager;

namespace kiln::asset {

export class AssetManagerPlugin : public app::PluginInterface {
public:
    [[nodiscard]]
    static auto create_plugin() -> AssetManagerPlugin
    {
        return AssetManagerPlugin{};
    }

    [[nodiscard]]
    static auto operator()(app::Arena& arena) -> AssetManager
    {
        return AssetManager{ arena.pool_allocator() };
    }
};

}   // namespace kiln::asset
