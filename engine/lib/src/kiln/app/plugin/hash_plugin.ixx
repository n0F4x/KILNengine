module;

#include <concepts>
#include <cstdint>

export module kiln.app.plugin.hash_plugin;

import kiln.app.memory.Arena;
import kiln.app.memory.MemoryPlugin;
import kiln.app.plugin.meta_plugin_c;
import kiln.app.plugin.plugin_c;
import kiln.util.reflection;
import kiln.util.type_traits.result_of;

namespace kiln::app {

template <typename MetaPlugin_T>
struct MetaWrap {};

export template <typename MaybeMetaPlugin_T>
[[nodiscard]]
// ReSharper disable once CppNotAllPathsReturnValue
consteval auto hash_plugin() -> uint64_t
{
    if constexpr (requires { &MaybeMetaPlugin_T::operator(); })
    {
        return util::
            hash_u64<util::result_of_t<decltype(&MaybeMetaPlugin_T::operator())>>();
    }
    else
    {
        if constexpr (std::same_as<MaybeMetaPlugin_T, MemoryPlugin>)
        {
            return util::hash_u64<Arena>();
        }
        else
        {
            return util::hash_u64<MetaWrap<MaybeMetaPlugin_T>>();
        }
    }
}

}   // namespace kiln::app
