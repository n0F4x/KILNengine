module;

#include <concepts>
#include <type_traits>

export module kiln.app.context.context_c;

import kiln.util.containers.GenericStack;

namespace kiln::app {

export template <typename T>
concept context_c =                 //
    util::generic_stack_item_c<T>   //
    && (requires { typename T::Builder; } || std::default_initializable<T>);

export template <typename T>
concept decays_to_context_c = context_c<std::decay_t<T>>;

}   // namespace kiln::app
