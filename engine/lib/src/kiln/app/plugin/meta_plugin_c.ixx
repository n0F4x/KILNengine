module;

#include <concepts>

export module kiln.app.plugin.meta_plugin_c;

import kiln.app.plugin.plugin_c;
import kiln.app.plugin.PluginInterface;
import kiln.util.concepts.naked;

namespace kiln::app {

export template <typename T>
concept meta_plugin_c = util::naked_c<T>                        //
                     && std::derived_from<T, PluginInterface>   //
                     && !plugin_c<T>;

}   // namespace kiln::app
