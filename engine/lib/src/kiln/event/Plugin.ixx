export module kiln.event.Plugin;

import kiln.app.App;
import kiln.event.EventSystem;

namespace kiln::event {

export class Plugin {
public:
    static auto operator()(app::App& app) -> void
    {
        app.context().emplace<EventSystem>();
    }
};

}   // namespace kiln::event
