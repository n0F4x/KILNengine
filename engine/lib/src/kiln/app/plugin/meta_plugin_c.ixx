module;

#include <concepts>
#include <type_traits>

export module kiln.app.plugin.meta_plugin_c;

import kiln.app.plugin.PluginInterface;
import kiln.app.plugin.strip_plugin_dependency_t;
import kiln.util.concepts.naked;
import kiln.util.concepts.type_list_all_of;
import kiln.util.containers.OptionalRef;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.result_of;

namespace kiln::app {

template <typename T>
struct RepresentsPluginDependency {
    constexpr static bool value =
        std::derived_from<strip_plugin_dependency_t<T>, PluginInterface>;
};

export template <typename T>
concept meta_plugin_c =
    util::naked_c<T>                           //
    && std::derived_from<T, PluginInterface>   //
    && requires {
    requires std::is_same_v<util::result_of_t<decltype(T::create_plugin)>, T>
                 && util::type_list_all_of_c<
                     util::arguments_of_t<decltype(T::create_plugin)>,
                     RepresentsPluginDependency>;
    };

export template <typename T>
concept decays_to_meta_plugin_c = meta_plugin_c<std::remove_cvref_t<T>>;

}   // namespace kiln::app
