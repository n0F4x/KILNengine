export module kiln.event.Plugin;

import kiln.app.plugin.PluginInterface;
import kiln.event.EventSystem;

namespace kiln::event {

export class Plugin : public app::PluginInterface {
public:
    static auto build() -> EventSystem
    {
        return EventSystem{};
    }
};

}   // namespace kiln::event
