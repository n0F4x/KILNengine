module;

#include <type_traits>

export module kiln.app.ResourceStack;

import kiln.util.GenericStack;

namespace kiln::app {

export using ResourceStack = util::GenericStack;

export template <typename T>
concept resource_c = util::generic_stack_item_c<T>;

export template <typename T>
concept decays_to_resource_c = resource_c<std::decay_t<T>>;

export template <typename T>
concept resource_injection_c = util::generic_stack_item_injection_c<T>;

export template <typename T>
concept decays_to_resource_injection_c = resource_injection_c<std::decay_t<T>>;

}   // namespace kiln::app
