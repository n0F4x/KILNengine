module;

#include <concepts>
#include <type_traits>

export module kiln.app.plugin.meta_plugin_c;

import kiln.app.plugin.plugin_c;
import kiln.app.plugin.PluginInterface;
import kiln.util.concepts.naked;

namespace kiln::app {

export template <typename T>
concept meta_plugin_c = util::naked_c<T>                        //
                     && std::derived_from<T, PluginInterface>   //
                     && not requires { &T::build; };

export template <typename T>
concept decays_to_meta_plugin_c = meta_plugin_c<std::remove_cvref_t<T>>;

}   // namespace kiln::app
