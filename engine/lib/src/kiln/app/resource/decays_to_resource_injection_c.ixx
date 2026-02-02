module;

#include <type_traits>

export module kiln.app.resource.decays_to_resource_injection_c;

import kiln.app.resource.resource_injection_c;

namespace kiln::app {

export template <typename T>
concept decays_to_resource_injection_c = resource_injection_c<std::decay_t<T>>;

}   // namespace kiln::app
