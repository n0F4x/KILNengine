module;

#include <type_traits>

export module kiln.app.plugin.strip_plugin_dependency_t;

import kiln.util.concepts.specialization_of;
import kiln.util.containers.OptionalRef;

namespace kiln::app {

template <typename>
struct StripPluginDependency;

template <util::specialization_of_c<util::OptionalRef> T>
struct StripPluginDependency<T> {
    using type = std::remove_cvref_t<typename T::ValueType>;
};

template <typename T>
    requires(!util::specialization_of_c<T, util::OptionalRef>)
struct StripPluginDependency<T> {
    using type = std::remove_cvref_t<T>;
};

export template <typename T>
using strip_plugin_dependency_t = StripPluginDependency<T>::type;

}   // namespace kiln::app
