module;

#include <concepts>
#include <type_traits>

export module kiln.app.registry.entry_builder_c;

import kiln.app.registry.BuildableEntryBuilder;
import kiln.app.registry.configuration_entry_c;
import kiln.app.registry.entry_c;
import kiln.app.registry.EntryBase;
import kiln.app.registry.EntryBuilderBase;
import kiln.app.registry.represents_entry_dependency_c;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.storable;
import kiln.util.containers.OptionalRef;
import kiln.util.type_traits.result_of;

namespace kiln::app {

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

template <typename T>
concept represents_builder_dependency_c
    = (std::is_lvalue_reference_v<T>
       && (std::derived_from<std::remove_cvref_t<T>, EntryBuilderBase>
           || configuration_entry_c<std::remove_cvref_t<T>>))
   || (util::specialization_of_c<T, util::OptionalRef>
       && (std::derived_from<std::remove_cvref_t<typename T::ValueType>, EntryBuilderBase>
           || configuration_entry_c<std::remove_cvref_t<typename T::ValueType>>));

template <typename Builder_T, typename... Dependencies_T>
consteval auto is_injection(auto (*)(Dependencies_T...)->Builder_T) -> bool
{
    return (represents_builder_dependency_c<Dependencies_T> && ...);
}

export template <typename T>
concept entry_builder_c = util::storable_c<T>
                       && std::derived_from<T, EntryBuilderBase>
                       && requires { requires is_entry_builder(&T::build); }   //
                       && ((std::derived_from<T, BuildableEntryBuilder>
                            && requires { requires is_injection<T>(T::create); })
                           || std::default_initializable<T>)
                       && !std::derived_from<T, EntryBase>;

export template <typename T>
concept decays_to_entry_builder_c = entry_builder_c<std::decay_t<T>>;

}   // namespace kiln::app
