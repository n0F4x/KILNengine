module;

#include <concepts>
#include <functional>

export module kiln.app.registry.BuildableEntry;

import kiln.app.registry.BuildableEntryBase;
import kiln.app.registry.EntryBuildDirector;

namespace kiln::app {

template <typename T, typename Entry_T>
concept build_description_c = std::invocable<T, EntryBuildDirector<Entry_T>&>;

export template <typename Entry_T, auto describe_build_T>
class BuildableEntry : public internal::BuildableEntryBase {
    static_assert(build_description_c<decltype(describe_build_T), Entry_T>);

    friend auto describe_build(EntryBuildDirector<Entry_T>& build_director) -> void
    {
        static_assert(std::derived_from<Entry_T, BuildableEntry>);
        std::invoke(describe_build_T, build_director);
    }
};

}   // namespace kiln::app
