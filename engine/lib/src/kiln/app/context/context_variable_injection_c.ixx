export module kiln.app.context.context_variable_injection_c;

import kiln.util.GenericStack;

namespace kiln::app {

export template <typename T>
concept context_variable_injection_c = util::generic_stack_item_injection_c<T>;

}   // namespace kiln::app
