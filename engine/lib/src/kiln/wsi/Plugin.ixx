export module kiln.wsi.Plugin;

import kiln.app.App;
import kiln.wsi.Context;

namespace kiln::wsi {

export class Plugin {
public:
    static auto operator()(app::App& app) -> void
    {
        app.resources().emplace<Context>();
    }
};

}   // namespace kiln::wsi
