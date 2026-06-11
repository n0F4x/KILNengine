module;

#include <concepts>

export module kiln.app.registry.ErasedEntryBuilder;

import kiln.app.registry.EntryBase;
import kiln.app.registry.Registry;
import kiln.app.registry.strip_dependency_t;
import kiln.util.concepts.specialization_of;
import kiln.util.containers.MoveOnlyFunction;
import kiln.util.containers.OptionalRef;
import kiln.util.type_traits.arguments_of;
import kiln.util.TypeList;

namespace kiln::app {

export using ErasedEntryBuilder = util::MoveOnlyFunction<auto(Registry&) &&->void, 0>;

template <typename Dependency_T>
auto fetch_dependency(Registry& registry) -> Dependency_T
{
    using StrippedDependency = strip_dependency_t<Dependency_T>;

    static_assert(std::derived_from<StrippedDependency, EntryBase>);

    if constexpr (util::specialization_of_c<Dependency_T, util::OptionalRef>)
    {
        return registry.find<StrippedDependency>();
    }
    else
    {
        return registry.at<StrippedDependency>();
    }
}

export template <typename Builder_T>
struct ErasedEntryBuilderLambda {
    Builder_T builder;

    auto operator()(Registry& registry) && -> void
    {
        [this,
         &registry]<typename... Dependencies_T>(util::TypeList<Dependencies_T...>) -> void
        {
            registry.insert(
                std::move(builder).build(fetch_dependency<Dependencies_T>(registry)...)
            );
        }(util::arguments_of_t<decltype(&Builder_T::build)>{});
    }
};

export template <typename Builder_T, typename... Args_T>
    requires std::constructible_from<Builder_T, Args_T&&...>
[[nodiscard]]
auto make_erased_entry_builder_lambda(Args_T&&... args)
    -> ErasedEntryBuilderLambda<Builder_T>
{
    return ErasedEntryBuilderLambda<Builder_T>{
        .builder{ std::forward<Args_T>(args)... },
    };
}

}   // namespace kiln::app
