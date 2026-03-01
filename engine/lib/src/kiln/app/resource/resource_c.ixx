export module kiln.app.resource.resource_c;

import kiln.util.containers.GenericStack;

namespace kiln::app {

export template <typename T>
concept resource_c = util::generic_stack_item_c<T>;

}   // namespace kiln::app
