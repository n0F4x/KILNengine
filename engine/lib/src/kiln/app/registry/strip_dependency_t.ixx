module;

#include <type_traits>

export module kiln.app.registry.strip_dependency_t;

import kiln.util.concepts.specialization_of;
import kiln.util.containers.OptionalRef;

namespace kiln::app {

template <typename>
struct StripDependency;

template <util::specialization_of_c<util::OptionalRef> T>
struct StripDependency<T> {
    using type = std::remove_cvref_t<typename T::ValueType>;
};

template <typename T>
    requires(!util::specialization_of_c<T, util::OptionalRef>)
struct StripDependency<T> {
    using type = std::remove_cvref_t<T>;
};

export template <typename T>
using strip_dependency_t = StripDependency<T>::type;

}   // namespace kiln::app
