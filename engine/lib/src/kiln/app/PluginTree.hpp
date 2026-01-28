#pragma once

#include <algorithm>
#include <cstdint>
#include <format>
#include <span>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "kiln/app/App.hpp"
#include "kiln/util/concepts/specialization_of.hpp"
#include "kiln/util/concepts/type_list_all_of.hpp"
#include "kiln/util/contract_macros.hpp"
#include "kiln/util/for_each.hpp"
#include "kiln/util/Function.hpp"
#include "kiln/util/GenericStack.hpp"
#include "kiln/util/OptionalRef.hpp"
#include "kiln/util/reflection.hpp"
#include "kiln/util/type_traits/arguments_of.hpp"
#include "kiln/util/type_traits/forward_like.hpp"
#include "kiln/util/type_traits/result_of.hpp"
#include "kiln/util/TypeList.hpp"

namespace kiln::app {

namespace internal {

using ErasedPlugin = util::MoveOnlyFunction<void(App&) &&, 0>;

template <typename T>
concept plugin_c = util::basic_generic_stack_item_c<T, ErasedPlugin>;

using PluginStack = util::BasicGenericStack<ErasedPlugin>;

template <typename T>
concept represents_optional_dependency_c = requires {
    requires util::specialization_of_c<T, util::OptionalRef>
                 && plugin_c<std::remove_const_t<typename T::ValueType>>;
};

template <typename T>
struct RepresentsPluginDependency {
    constexpr static bool value = plugin_c<std::remove_cvref_t<T>>
                               || represents_optional_dependency_c<T>;
};

template <typename T>
concept plugin_injection_c =
    plugin_c<util::result_of_t<T&&>>
    && util::type_list_all_of_c<util::arguments_of_t<T>, RepresentsPluginDependency>;

class ErasedPluginInjection {
public:
    using Function = util::MoveOnlyFunction<void(PluginStack&) &&, 0>;

    template <typename PluginInjection_T>
    explicit ErasedPluginInjection(
        PluginInjection_T&&     plugin_injection,
        std::vector<uint64_t>&& unresolved_plugin_dependency_hashes
    );

    [[nodiscard]]
    auto plugin_type_hash() const noexcept -> uint64_t;
    [[nodiscard]]
    auto plugin_name() const noexcept -> std::string_view;
    [[nodiscard]]
    auto resolved_plugin_dependency_hashes() const noexcept -> std::span<const uint64_t>;
    [[nodiscard]]
    auto unresolved_plugin_dependency_hashes() const noexcept
        -> std::span<const uint64_t>;

    template <typename Self_T>
    auto underlying_function(this Self_T&&) -> util::forward_like_t<Function, Self_T>;

    auto resolve_dependency(uint64_t new_plugin_hash) -> void;

    auto operator()(PluginStack& plugin_stack) && -> void;

private:
    Function              m_function;
    uint64_t              m_plugin_type_hash;
    std::string_view      m_plugin_name;
    std::vector<uint64_t> m_resolved_plugin_dependency_hashes;
    std::vector<uint64_t> m_unresolved_plugin_dependency_hashes;
};

}   // namespace internal

template <typename T>
concept plugin_c = internal::plugin_c<T>;

template <typename T>
concept decays_to_plugin_c = plugin_c<std::decay_t<T>>;

template <typename T>
concept plugin_injection_c = internal::plugin_injection_c<T>;

template <typename T>
concept decays_to_plugin_injection_c = plugin_injection_c<std::decay_t<T>>;

class PluginTree {
public:
    template <decays_to_plugin_injection_c... PluginInjections_T>
    explicit PluginTree(PluginInjections_T&&... default_plugin_injections);

    template <plugin_c Plugin_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

    template <decays_to_plugin_injection_c PluginInjection_T>
    auto plug_in(PluginInjection_T&& plugin_injection)
        -> std::decay_t<PluginInjection_T>&;

    auto invoke_plugins(App& app) && -> void;

private:
    struct PluginNameChainNode {
        const PluginNameChainNode* previous{};
        std::string_view           plugin_name;

        [[nodiscard]]
        auto format() const -> std::string;

    private:
        auto format(std::string& out_string, std::size_t capacity) const -> void;
    };

    std::vector<internal::ErasedPluginInjection> m_plugin_injections;
    std::vector<uint64_t> m_unresolved_optional_dependency_plugin_types;

    template <typename Plugin_T>
    auto check_for_duplicated_plugin() const -> void;
    template <typename PluginInjection_T>
    auto check_required_dependencies() const -> void;
    template <typename PluginInjection_T>
    auto check_for_cyclic_dependencies() const -> void;
    auto check_for_new_cyclic_dependency(
        uint64_t                   new_plugin_hash,
        std::string_view           new_plugin_name,
        uint64_t                   dependency_plugin_hash,
        const PluginNameChainNode& visited_plugin_names
    ) const -> void;


    template <typename PluginInjection_T>
    [[nodiscard]]
    auto collect_unresolved_dependency_plugin_hashes() const -> std::vector<uint64_t>;

    auto collect_all_resolved_dependency_plugin_hashes(
        uint64_t               plugin_hash,
        std::vector<uint64_t>& out
    ) const -> void;


    template <decays_to_plugin_injection_c PluginInjection_T>
    auto push_back(PluginInjection_T&& plugin_injection)
        -> std::decay_t<PluginInjection_T>&;
    auto reestablish_internal_ordering_of_plugins(uint64_t new_plugin_hash) -> void;
    auto resolve(uint64_t new_plugin_hash) -> void;
};

}   // namespace kiln::app

namespace kiln::app {

namespace internal {

template <typename... Dependencies_T>
[[nodiscard]]
auto collect_dependencies(util::TypeList<Dependencies_T...>, PluginStack& plugin_stack)
    -> std::tuple<Dependencies_T...>
{
    return std::forward_as_tuple(
        [&plugin_stack] -> Dependencies_T
        {
            if constexpr (represents_optional_dependency_c<Dependencies_T>)
            {
                return plugin_stack
                    .find<std::remove_const_t<typename Dependencies_T::ValueType>>();
            }
            else
            {
                return plugin_stack.at<std::remove_cvref_t<Dependencies_T>>();
            }
        }()...
    );
}

template <typename PluginInjection_T>
struct PluginInjectionLambda {
    PluginInjection_T plugin_injection;

    auto operator()(PluginStack& plugin_stack) && -> void
    {
        plugin_stack.insert(
            std::apply(
                std::move(plugin_injection),
                collect_dependencies(
                    util::arguments_of_t<PluginInjection_T>{}, plugin_stack
                )
            )
        );
    }
};

constexpr static auto hash_plugin_dependency =
    []<typename PluginDependency_T> [[nodiscard]]
    -> uint64_t
{
    if constexpr (represents_optional_dependency_c<PluginDependency_T>)
    {
        return util::
            hash_u64<std::remove_const_t<typename PluginDependency_T::ValueType>>();
    }
    else
    {
        return util::hash_u64<std::remove_cvref_t<PluginDependency_T>>();
    }
};

template <typename PluginInjection_T>
[[nodiscard]]
auto collect_resolved_plugin_dependency_hashes(
    const std::span<const uint64_t> unresolved_dependency_hashes
) -> std::vector<uint64_t>
{
    std::vector<uint64_t> result;

    util::for_each(
        util::arguments_of_t<PluginInjection_T>{},
        [&result, unresolved_dependency_hashes](const uint64_t plugin_hash) -> void
        {
            if (!std::ranges::contains(unresolved_dependency_hashes, plugin_hash)
                && !std::ranges::contains(result, plugin_hash))
            {
                result.push_back(plugin_hash);
            }
        },
        hash_plugin_dependency
    );

    return result;
}

template <typename PluginInjection_T>
ErasedPluginInjection::ErasedPluginInjection(
    PluginInjection_T&&     plugin_injection,
    std::vector<uint64_t>&& unresolved_plugin_dependency_hashes
)
    : m_function{
          PluginInjectionLambda<std::decay_t<PluginInjection_T>>{
              std::forward<PluginInjection_T>(plugin_injection)   //
          }   //
      },
      m_plugin_type_hash{ util::hash_u64<util::result_of_t<PluginInjection_T>>() },
      m_plugin_name{ util::name_of<util::result_of_t<PluginInjection_T>>() },
      m_resolved_plugin_dependency_hashes{
          collect_resolved_plugin_dependency_hashes<PluginInjection_T>(
              unresolved_plugin_dependency_hashes
          )
      },
      m_unresolved_plugin_dependency_hashes{
          std::move(unresolved_plugin_dependency_hashes)
      }
{
}

template <typename Self_T>
auto ErasedPluginInjection::underlying_function(this Self_T&& self)
    -> util::forward_like_t<Function, Self_T>
{
    return std::forward_like<Self_T>(self.ErasedPluginInjection::m_function);
}

}   // namespace internal

template <decays_to_plugin_injection_c... PluginInjections_T>
PluginTree::PluginTree(PluginInjections_T&&... default_plugin_injections)
{
    (plug_in(std::forward<PluginInjections_T>(default_plugin_injections)), ...);
}

template <plugin_c Plugin_T>
auto PluginTree::contains() const noexcept -> bool
{
    return std::ranges::contains(
        m_plugin_injections,
        util::hash_u64<Plugin_T>(),
        &internal::ErasedPluginInjection::plugin_type_hash
    );
}

template <decays_to_plugin_injection_c PluginInjection_T>
auto PluginTree::plug_in(PluginInjection_T&& plugin_injection)
    -> std::decay_t<PluginInjection_T>&
{
    using Plugin = util::result_of_t<PluginInjection_T>;
    constexpr static uint64_t plugin_hash{ util::hash_u64<Plugin>() };

    check_for_duplicated_plugin<Plugin>();
    check_required_dependencies<PluginInjection_T>();
    check_for_cyclic_dependencies<PluginInjection_T>();

    std::decay_t<PluginInjection_T>& result{
        push_back(std::forward<PluginInjection_T>(plugin_injection))
    };

    reestablish_internal_ordering_of_plugins(plugin_hash);
    resolve(plugin_hash);

    return result;
}

template <typename Plugin_T>
auto PluginTree::check_for_duplicated_plugin() const -> void
{
    PRECOND(
        (!contains<Plugin_T>()),
        std::format(
            "Attempt to inject plugin of type `{}`, but it has already been injected",
            util::name_of<Plugin_T>()
        )
    );
}

template <typename PluginInjection_T>
auto PluginTree::check_required_dependencies() const -> void
{
    util::for_each(
        util::arguments_of_t<PluginInjection_T>{},
        [this]<typename Dependency_T> -> void
        {
            std::ignore = this;

            if constexpr (!internal::represents_optional_dependency_c<Dependency_T>)
            {
                PRECOND(
                    (contains<std::remove_cvref_t<Dependency_T>>()),
                    std::format(
                        "Dependent plugin of type `{}` must be injected before `{}`",
                        util::name_of<std::remove_cvref_t<Dependency_T>>(),
                        util::name_of<util::result_of_t<PluginInjection_T>>()
                    )
                );
            }
        }
    );
}

template <typename PluginInjection_T>
auto PluginTree::check_for_cyclic_dependencies() const -> void
{
    using Plugin = util::result_of_t<PluginInjection_T>;

    if (!std::ranges::contains(
            m_unresolved_optional_dependency_plugin_types, util::hash_u64<Plugin>()
        ))
    {
        return;
    }

    util::for_each(
        util::arguments_of_t<PluginInjection_T>{},
        [this]<typename Dependency_T> -> void
        {
            if constexpr (internal::represents_optional_dependency_c<Dependency_T>)
            {
                using DependencyPlugin =
                    std::remove_const_t<typename Dependency_T::ValueType>;

                const PluginNameChainNode plugin_name_chain_node{
                    .plugin_name = util::name_of<Plugin>(),
                };
                check_for_new_cyclic_dependency(
                    util::hash_u64<Plugin>(),
                    util::name_of<Plugin>(),
                    util::hash_u64<DependencyPlugin>(),
                    plugin_name_chain_node
                );
            }
        }
    );
}

template <typename PluginInjection_T>
auto PluginTree::collect_unresolved_dependency_plugin_hashes() const
    -> std::vector<uint64_t>
{
    std::vector<uint64_t> result;

    util::for_each(
        util::arguments_of_t<PluginInjection_T>{},
        [this, &result]<typename Dependency_T> -> void
        {
            if constexpr (internal::represents_optional_dependency_c<Dependency_T>)
            {
                using PluginDependency =
                    std::remove_const_t<typename Dependency_T::ValueType>;

                constexpr static uint64_t dependency_hash{
                    util::hash_u64<PluginDependency>()
                };

                if (!std::ranges::contains(
                        m_plugin_injections,
                        dependency_hash,
                        &internal::ErasedPluginInjection::plugin_type_hash
                    ))
                {
                    result.push_back(dependency_hash);
                }
            }
        }
    );

    return result;
}

template <decays_to_plugin_injection_c PluginInjection_T>
auto PluginTree::push_back(PluginInjection_T&& plugin_injection)
    -> std::decay_t<PluginInjection_T>&
{
    internal::ErasedPluginInjection& erased_plugin_injection =
        m_plugin_injections.emplace_back(
            std::forward<PluginInjection_T>(plugin_injection),
            collect_unresolved_dependency_plugin_hashes<PluginInjection_T>()
        );

    m_unresolved_optional_dependency_plugin_types.append_range(
        erased_plugin_injection.unresolved_plugin_dependency_hashes()
    );

    return util::any_cast<internal::PluginInjectionLambda<std::decay_t<PluginInjection_T>>>(
               erased_plugin_injection.underlying_function()
    )
        .plugin_injection;
}

}   // namespace kiln::app
