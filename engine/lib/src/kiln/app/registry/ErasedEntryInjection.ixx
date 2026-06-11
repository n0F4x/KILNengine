module;

#include <concepts>

export module kiln.app.registry.ErasedEntryInjection;

import kiln.app.registry.ConfigurationEntry;
import kiln.app.registry.EntryBuilderBase;
import kiln.app.registry.EntryBuilderContainer;
import kiln.app.registry.Registry;
import kiln.app.registry.strip_dependency_t;
import kiln.util.concepts.specialization_of;
import kiln.util.containers.MoveOnlyFunction;
import kiln.util.containers.OptionalRef;
import kiln.util.type_traits.arguments_of;
import kiln.util.TypeList;

namespace kiln::app {

export using ErasedEntryInjection
    = util::MoveOnlyFunction<auto(EntryBuilderContainer&, Registry&) &&->void, 0>;

template <typename Dependency_T>
// ReSharper disable once CppNotAllPathsReturnValue
auto fetch_dependency(EntryBuilderContainer& builders, Registry& registry) -> Dependency_T
{
    using StrippedDependency = strip_dependency_t<Dependency_T>;

    if constexpr (std::derived_from<StrippedDependency, EntryBuilderBase>)
    {
        if constexpr (util::specialization_of_c<Dependency_T, util::OptionalRef>)
        {
            return builders.find<StrippedDependency>();
        }
        else
        {
            return builders.at<StrippedDependency>();
        }
    }
    else if constexpr (std::derived_from<StrippedDependency, ConfigurationEntry>)
    {
        if constexpr (util::specialization_of_c<Dependency_T, util::OptionalRef>)
        {
            return registry.find<StrippedDependency>();
        }
        else
        {
            return registry.at<StrippedDependency>();
        }
    }
    else
    {
        static_assert(false, "invalid dependency");
    }
}

export template <typename Builder_T>
struct ErasedEntryInjectionLambda {
    static auto operator()(EntryBuilderContainer& builders, Registry& registry) -> void
    {
        [&builders,
         &registry]<typename... Dependencies_T>(util::TypeList<Dependencies_T...>) -> void
        {
            builders.insert(
                Builder_T::create(fetch_dependency<Dependencies_T>(builders, registry)...)
            );
        }(util::arguments_of_t<decltype(Builder_T::create)>{});
    }
};

export template <typename Builder_T>
[[nodiscard]]
auto make_erased_entry_injection() -> ErasedEntryInjectionLambda<Builder_T>
{
    return ErasedEntryInjectionLambda<Builder_T>{};
}

}   // namespace kiln::app
