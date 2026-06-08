module;

#include <concepts>
#include <functional>
#include <span>
#include <vector>

export module kiln.app.registry.EntryBuilderInterface;

import kiln.app.registry.entry_c;
import kiln.app.registry.Registry;
import kiln.util.concepts.naked;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.type_list_all_of;
import kiln.util.containers.MoveOnlyFunction;
import kiln.util.containers.OptionalRef;
import kiln.util.reflection;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.result_of;
import kiln.util.type_traits.type_list_drop_front;
import kiln.util.type_traits.type_list_front;
import kiln.util.TypeList;

namespace kiln::app {

template <typename EntryBuilder_T>
struct IsEntryDependencyRef {
    template <typename T>
    struct type {
        constexpr static bool value{
            std::is_lvalue_reference_v<T>   //
            && entry_c<std::remove_cvref_t<T>>
            && !std::same_as<
                std::remove_cvref_t<T>,
                util::result_of_t<decltype(&EntryBuilder_T::build)>>   //
        };
    };
};

template <typename T, typename EntryBuilder_T>
concept configures_c = util::naked_c<EntryBuilder_T>   //
                    && requires {
                           requires std::same_as<
                               util::type_list_front_t<util::arguments_of_t<T>>,
                               EntryBuilder_T&>;
                           requires util::type_list_all_of_c<
                               util::type_list_drop_front_t<util::arguments_of_t<T>>,
                               IsEntryDependencyRef<EntryBuilder_T>::template type>;
                       };


export class EntryBuilderInterface;

using ErasedConfiguration
    = util::MoveOnlyFunction<auto(EntryBuilderInterface&, Registry&) &&->void>;

export [[nodiscard]]
auto configuration_dependency_hash_set(const EntryBuilderInterface& builder) noexcept
    -> std::span<const uint64_t>;

export [[nodiscard]]
auto dependency_hashes(const EntryBuilderInterface& builder) noexcept
    -> std::span<const uint64_t>;

export auto set_resolved_dependency_hashes(
    EntryBuilderInterface&  builder,
    std::span<const uint64_t> resolved_dependency_hashes
) -> void;

export template <util::naked_c EntryBuilder_T>
auto configure_and_build(EntryBuilder_T&&, Registry& registry) -> void;

export class EntryBuilderInterface {
public:
    template <util::naked_c Self_T, configures_c<Self_T> Configuration_T>
    auto register_configuration(this Self_T& self, Configuration_T&& configuration)
        -> void;

private:
    friend auto configuration_dependency_hash_set(
        const EntryBuilderInterface& builder
    ) noexcept -> std::span<const uint64_t>;
    friend auto dependency_hashes(const EntryBuilderInterface& builder) noexcept
        -> std::span<const uint64_t>;

    friend auto set_resolved_dependency_hashes(
        EntryBuilderInterface&  builder,
        std::span<const uint64_t> resolved_dependency_hashes
    ) -> void;

    template <util::naked_c Self_T>
    friend auto configure_and_build(Self_T&&, Registry& registry) -> void;


    std::pmr::vector<uint64_t>            m_configuration_dependency_hash_set;
    std::pmr::vector<uint64_t>            m_dependency_hashes;
    std::pmr::vector<ErasedConfiguration> m_configurators;


    auto insert_configuration_dependency_hash(uint64_t dependency_hash) -> void;
};

}   // namespace kiln::app

namespace kiln::app {

namespace internal {

template <util::specialization_of_c<util::OptionalRef> PotentiallyOptionalEntryRef_T>
auto fetch_dependency(Registry& registry) -> PotentiallyOptionalEntryRef_T
{
    return registry
        .find<std::remove_cvref_t<typename PotentiallyOptionalEntryRef_T::ValueType>>();
}

template <typename PotentiallyOptionalEntryRef_T>
    requires(std::is_lvalue_reference_v<PotentiallyOptionalEntryRef_T>)
auto fetch_dependency(Registry& registry) -> PotentiallyOptionalEntryRef_T
{
    return registry.at<std::remove_cvref_t<PotentiallyOptionalEntryRef_T>>();
}

template <typename EntryBuilder_T>
auto invoke(EntryBuilder_T&& builder, Registry& registry) -> void
{
    [&builder, &registry]<typename... PotentiallyOptionalEntryRefs_T>(
        util::TypeList<PotentiallyOptionalEntryRefs_T...>
    ) -> void
    {
        registry.insert(
            std::forward<EntryBuilder_T>(builder)
                .build(fetch_dependency<PotentiallyOptionalEntryRefs_T>(registry)...)
        );
    }(util::arguments_of_t<decltype(&EntryBuilder_T::build)>{});
}

}   // namespace internal

template <util::naked_c EntryBuilder_T>
auto configure_and_build(EntryBuilder_T&& builder, Registry& registry) -> void
{
    for (ErasedConfiguration& configurator :
         builder.EntryBuilderInterface::m_configurators)
    {
        std::move(configurator)(builder, registry);
    }

    if constexpr (requires { &EntryBuilder_T::build; })
    {
        internal::invoke(std::forward<EntryBuilder_T>(builder), registry);
    }
}

template <util::naked_c Self_T, configures_c<Self_T> Configuration_T>
auto EntryBuilderInterface::register_configuration(
    this Self_T&      self,
    Configuration_T&& configuration
) -> void
{
    [&self,
     &configuration]<typename... EntryRefs_T>(util::TypeList<EntryRefs_T...>) -> void
    {
        (self.EntryBuilderInterface::insert_configuration_dependency_hash(
             util::hash_u64<std::remove_cvref_t<EntryRefs_T>>()
         ),
         ...);

        self.EntryBuilderInterface::m_configurators.emplace_back(
            [x_configuration = std::forward<Configuration_T>(configuration)](
                EntryBuilderInterface& future_self,
                Registry&                registry   //
            ) mutable -> void
            {
                std::invoke(
                    std::move(x_configuration),
                    static_cast<Self_T&>(future_self),
                    registry.at<std::remove_cvref_t<EntryRefs_T>>()...
                );
            }
        );
    }(util::type_list_drop_front_t<util::arguments_of_t<Configuration_T>>{});
}

}   // namespace kiln::app
