module;

#include <type_traits>

export module kiln.app.registry.strip_dependency_t;

import kiln.util.concepts.specialization_of;
import kiln.util.containers.OptionalRef;

namespace kiln::app {

template <typename T>
struct StripDependency {
    using type = std::remove_cvref_t<T>;
};

template <typename T>
struct StripDependency<util::OptionalRef<T>> {
    using type = std::remove_const_t<T>;
};

export template <typename T>
using strip_dependency_t = StripDependency<T>::type;

}   // namespace kiln::app
