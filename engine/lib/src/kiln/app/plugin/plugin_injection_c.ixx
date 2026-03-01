module;

#include <type_traits>

export module kiln.app.plugin.plugin_injection_c;

import kiln.app.plugin.plugin_c;
import kiln.app.plugin.strip_plugin_dependency_t;
import kiln.util.concepts.type_list_all_of;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.result_of;

namespace kiln::app {

template <typename T>
struct RepresentsPluginDependency {
    constexpr static bool value = plugin_c<strip_plugin_dependency_t<T>>;
};

export template <typename T>
concept plugin_injection_c =
    plugin_c<util::result_of_t<T&&>>
    && util::type_list_all_of_c<util::arguments_of_t<T>, RepresentsPluginDependency>;

export template <typename T>
concept decays_to_plugin_injection_c = plugin_injection_c<std::decay_t<T>>;

}   // namespace kiln::app
