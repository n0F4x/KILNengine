module;

#include <type_traits>

export module kiln.app.plugin.plugin_c;

import kiln.app.plugin.ErasedPlugin;
import kiln.util.containers.GenericStack;

namespace kiln::app {

export template <typename T>
concept plugin_c = util::basic_generic_stack_item_c<T, ErasedPlugin>;

export template <typename T>
concept decays_to_plugin_c = plugin_c<std::decay_t<T>>;

}   // namespace kiln::app
