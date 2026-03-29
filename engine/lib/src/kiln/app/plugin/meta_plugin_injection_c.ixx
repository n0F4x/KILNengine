module;

#include <concepts>

export module kiln.app.plugin.meta_plugin_injection_c;

import kiln.app.plugin.meta_plugin_c;
import kiln.app.plugin.plugin_c;
import kiln.app.plugin.strip_plugin_dependency_t;
import kiln.util.concepts.function_reference;
import kiln.util.concepts.type_list_all_of;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.result_of;

namespace kiln::app {

template <typename T>
struct RepresentsPluginDependency {
    constexpr static bool value = plugin_c<strip_plugin_dependency_t<T>>
                               || meta_plugin_c<strip_plugin_dependency_t<T>>;
};

export template <typename T>
concept meta_plugin_injection_c =
    util::function_reference_c<T>   //
    && meta_plugin_c<util::result_of_t<T&&>>
    && util::type_list_all_of_c<util::arguments_of_t<T>, RepresentsPluginDependency>;

}   // namespace kiln::app
