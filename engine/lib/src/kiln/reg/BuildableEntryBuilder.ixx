module;

#include <concepts>

export module kiln.reg.BuildableEntryBuilder;

import kiln.reg.BuildableEntryBuilderBase;
import kiln.reg.BuildDirector;
import kiln.reg.entry_c;
import kiln.reg.EntryBuilderInterface;

namespace kiln::reg {

template <typename T, typename EntryBuilder_T>
concept build_description_c = std::invocable<T, BuildDirector<EntryBuilder_T>&>;

export template <entry_c Entry_T, typename EntryBuilder_T, auto describe_build_T>
class BuildableEntryBuilder : public EntryBuilderInterface<Entry_T>,
                              public internal::BuildableEntryBuilderBase   //
{
    friend auto describe_build(
        BuildableEntryBuilderBase,
        BuildDirector<EntryBuilder_T>& build_director
    ) -> void
    {
        static_assert(build_description_c<decltype(describe_build_T), EntryBuilder_T>);
        static_assert(std::derived_from<EntryBuilder_T, BuildableEntryBuilder>);
        std::invoke(describe_build_T, build_director);
    }
};

}   // namespace kiln::reg
