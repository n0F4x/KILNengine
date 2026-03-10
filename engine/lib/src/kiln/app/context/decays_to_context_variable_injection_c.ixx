module;

#include <type_traits>

export module kiln.app.context.decays_to_context_variable_injection_c;

import kiln.app.context.context_variable_injection_c;

namespace kiln::app {

export template <typename T>
concept decays_to_context_variable_injection_c = context_variable_injection_c<std::decay_t<T>>;

}   // namespace kiln::app
