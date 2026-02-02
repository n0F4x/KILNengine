module;

#include <type_traits>

export module kiln.app.resource.decays_to_resource_c;

import kiln.app.resource.resource_c;

namespace kiln::app {

export template <typename T>
concept decays_to_resource_c = resource_c<std::decay_t<T>>;

}   // namespace kiln::app
