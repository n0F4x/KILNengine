export module kiln.app.context.context_variable_c;

import kiln.util.containers.GenericStack;

namespace kiln::app {

export template <typename T>
concept context_variable_c = util::generic_stack_item_c<T>;

}   // namespace kiln::app
