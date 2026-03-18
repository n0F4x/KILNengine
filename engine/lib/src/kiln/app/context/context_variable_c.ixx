module;

#include <type_traits>

export module kiln.app.context.context_variable_c;

import kiln.util.containers.GenericStack;

namespace kiln::app {

export template <typename T>
concept context_variable_c = util::generic_stack_item_c<T>;

export template <typename T>
concept decays_to_context_variable_c = context_variable_c<std::decay_t<T>>;

}   // namespace kiln::app
