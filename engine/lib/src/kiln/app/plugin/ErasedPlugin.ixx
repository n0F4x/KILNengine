export module kiln.app.plugin.ErasedPlugin;

import kiln.app.App;
import kiln.util.containers.MoveOnlyFunction;

namespace kiln::app {

export using ErasedPlugin = util::MoveOnlyFunction<void(App&) &&, 0>;

}   // namespace kiln::app
