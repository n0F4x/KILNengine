export module kiln.app.registry.RepresentsEntryDependencyConcept;

import kiln.app.registry.represents_entry_dependency_c;

namespace kiln::app {

export template <typename T>
struct RepresentsEntryDependencyConcept {
    constexpr static bool value = represents_entry_dependency_c<T>;
};

}   // namespace kiln::app
