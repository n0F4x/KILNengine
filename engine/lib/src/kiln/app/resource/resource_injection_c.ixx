export module kiln.app.resource.resource_injection_c;

import kiln.util.GenericStack;

namespace kiln::app {

export template <typename T>
concept resource_injection_c = util::generic_stack_item_injection_c<T>;

}   // namespace kiln::app
