module;

#include <concepts>
#include <functional>
#include <span>
#include <vector>

export module kiln.app.context.ContextBuilderInterface;

import kiln.app.context.context_c;
import kiln.app.context.Contexts;
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

template <typename ContextBuilder_T>
struct IsContextDependencyRef {
    template <typename T>
    struct type {
        constexpr static bool value{
            std::is_lvalue_reference_v<T>   //
            && context_c<std::remove_cvref_t<T>>
            && !std::same_as<
                std::remove_cvref_t<T>,
                util::result_of_t<decltype(&ContextBuilder_T::build)>>   //
        };
    };
};

template <typename T, typename ContextBuilder_T>
concept configures_c = util::naked_c<ContextBuilder_T>   //
                    && requires {
                           requires std::same_as<
                               util::type_list_front_t<util::arguments_of_t<T>>,
                               ContextBuilder_T&>;
                           requires util::type_list_all_of_c<
                               util::type_list_drop_front_t<util::arguments_of_t<T>>,
                               IsContextDependencyRef<ContextBuilder_T>::template type>;
                       };


export class ContextBuilderInterface;

using ErasedConfiguration
    = util::MoveOnlyFunction<auto(ContextBuilderInterface&, Contexts&) &&->void>;

export [[nodiscard]]
auto configuration_dependency_hash_set(const ContextBuilderInterface& builder) noexcept
    -> std::span<const uint64_t>;

export [[nodiscard]]
auto dependency_hashes(const ContextBuilderInterface& builder) noexcept
    -> std::span<const uint64_t>;

export auto set_resolved_dependency_hashes(
    ContextBuilderInterface&  builder,
    std::span<const uint64_t> resolved_dependency_hashes
) -> void;

export template <util::naked_c ContextBuilder_T>
auto configure_and_build(ContextBuilder_T&&, Contexts& contexts) -> void;

export class ContextBuilderInterface {
public:
    template <util::naked_c Self_T, configures_c<Self_T> Configuration_T>
    auto register_configuration(this Self_T& self, Configuration_T&& configuration)
        -> void;

private:
    friend auto configuration_dependency_hash_set(
        const ContextBuilderInterface& builder
    ) noexcept -> std::span<const uint64_t>;
    friend auto dependency_hashes(const ContextBuilderInterface& builder) noexcept
        -> std::span<const uint64_t>;

    friend auto set_resolved_dependency_hashes(
        ContextBuilderInterface&  builder,
        std::span<const uint64_t> resolved_dependency_hashes
    ) -> void;

    template <util::naked_c Self_T>
    friend auto configure_and_build(Self_T&&, Contexts& contexts) -> void;


    std::pmr::vector<uint64_t>            m_configuration_dependency_hash_set;
    std::pmr::vector<uint64_t>            m_dependency_hashes;
    std::pmr::vector<ErasedConfiguration> m_configurators;


    auto insert_configuration_dependency_hash(uint64_t dependency_hash) -> void;
};

}   // namespace kiln::app

namespace kiln::app {

namespace internal {

template <util::specialization_of_c<util::OptionalRef> PotentiallyOptionalContextRef_T>
auto fetch_dependency(Contexts& contexts) -> PotentiallyOptionalContextRef_T
{
    return contexts
        .find<std::remove_cvref_t<typename PotentiallyOptionalContextRef_T::ValueType>>();
}

template <typename PotentiallyOptionalContextRef_T>
    requires(std::is_lvalue_reference_v<PotentiallyOptionalContextRef_T>)
auto fetch_dependency(Contexts& contexts) -> PotentiallyOptionalContextRef_T
{
    return contexts.at<std::remove_cvref_t<PotentiallyOptionalContextRef_T>>();
}

template <typename ContextBuilder_T>
auto invoke(ContextBuilder_T&& builder, Contexts& contexts) -> void
{
    [&builder, &contexts]<typename... PotentiallyOptionalContextRefs_T>(
        util::TypeList<PotentiallyOptionalContextRefs_T...>
    ) -> void
    {
        contexts.insert(
            std::forward<ContextBuilder_T>(builder)
                .build(fetch_dependency<PotentiallyOptionalContextRefs_T>(contexts)...)
        );
    }(util::arguments_of_t<decltype(&ContextBuilder_T::build)>{});
}

}   // namespace internal

template <util::naked_c ContextBuilder_T>
auto configure_and_build(ContextBuilder_T&& builder, Contexts& contexts) -> void
{
    for (ErasedConfiguration& configurator :
         builder.ContextBuilderInterface::m_configurators)
    {
        std::move(configurator)(builder, contexts);
    }

    if constexpr (requires { &ContextBuilder_T::build; })
    {
        internal::invoke(std::forward<ContextBuilder_T>(builder), contexts);
    }
}

template <util::naked_c Self_T, configures_c<Self_T> Configuration_T>
auto ContextBuilderInterface::register_configuration(
    this Self_T&      self,
    Configuration_T&& configuration
) -> void
{
    [&self,
     &configuration]<typename... ContextRefs_T>(util::TypeList<ContextRefs_T...>) -> void
    {
        (self.ContextBuilderInterface::insert_configuration_dependency_hash(
             util::hash_u64<std::remove_cvref_t<ContextRefs_T>>()
         ),
         ...);

        self.ContextBuilderInterface::m_configurators.emplace_back(
            [x_configuration = std::forward<Configuration_T>(configuration)](
                ContextBuilderInterface& future_self,
                Contexts&                contexts   //
            ) mutable -> void
            {
                std::invoke(
                    std::move(x_configuration),
                    static_cast<Self_T&>(future_self),
                    contexts.at<std::remove_cvref_t<ContextRefs_T>>()...
                );
            }
        );
    }(util::type_list_drop_front_t<util::arguments_of_t<Configuration_T>>{});
}

}   // namespace kiln::app
