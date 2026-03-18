module;

#include <cstdint>

export module kiln.app.plugin.hash_plugin;

import kiln.app.plugin.meta_plugin_c;
import kiln.app.plugin.plugin_c;
import kiln.util.reflection;
import kiln.util.type_traits.result_of;

namespace kiln::app {

template <typename T>
concept maybe_meta_plugin_c = plugin_c<T> || meta_plugin_c<T>;

template <typename MetaPlugin_T>
struct MetaWrap {};

export template <maybe_meta_plugin_c MaybeMetaPlugin_T>
[[nodiscard]]
// ReSharper disable once CppNotAllPathsReturnValue
consteval auto hash_plugin() -> uint64_t
{
    if constexpr (plugin_c<MaybeMetaPlugin_T>)
    {
        return util::hash_u64<util::result_of_t<decltype(&MaybeMetaPlugin_T::build)>>();
    }
    else if (meta_plugin_c<MaybeMetaPlugin_T>)
    {
        return util::hash_u64<MetaWrap<MaybeMetaPlugin_T>>();
    }
}

}   // namespace kiln::app
