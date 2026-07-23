module;

#include <type_traits>

export module kiln.reg.entry_builder_c;

import kiln.reg.BuildableEntryBuilderBase;
import kiln.reg.entry_c;
import kiln.reg.EntryBuilderBase;
import kiln.reg.represents_entry_dependency_c;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.storable;
import kiln.util.containers.OptionalRef;
import kiln.util.type_traits.result_of;

namespace kiln::reg {

template <typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (*)(Dependencies_T...)->Entry_T) -> bool
{
    return (represents_entry_dependency_c<Dependencies_T> && ...) && entry_c<Entry_T>;
}

template <typename Builder_T, typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (Builder_T::*)(Dependencies_T...)->Entry_T) -> bool
{
    return (represents_entry_dependency_c<Dependencies_T> && ...) && entry_c<Entry_T>;
}

template <typename Builder_T, typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (Builder_T::*)(Dependencies_T...) const->Entry_T)
    -> bool
{
    return (represents_entry_dependency_c<Dependencies_T> && ...) && entry_c<Entry_T>;
}

template <typename Builder_T, typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (Builder_T::*)(Dependencies_T...) &->Entry_T)
    -> bool = delete;

template <typename Builder_T, typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (Builder_T::*)(Dependencies_T...) const&->Entry_T)
    -> bool
{
    return (represents_entry_dependency_c<Dependencies_T> && ...) && entry_c<Entry_T>;
}

template <typename Builder_T, typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (Builder_T::*)(Dependencies_T...) &&->Entry_T)
    -> bool
{
    return (represents_entry_dependency_c<Dependencies_T> && ...) && entry_c<Entry_T>;
}

template <typename Builder_T, typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (Builder_T::*)(Dependencies_T...) const&&->Entry_T)
    -> bool
{
    return (represents_entry_dependency_c<Dependencies_T> && ...) && entry_c<Entry_T>;
}

export template <typename T>
concept entry_builder_c = util::storable_c<T>
                       && std::is_base_of_v<internal::EntryBuilderBase, T>
                       && requires { requires is_entry_builder(&T::build); };

export template <typename T>
concept decays_to_entry_builder_c = entry_builder_c<std::decay_t<T>>;

}   // namespace kiln::reg
