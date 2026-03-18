module;

#include <algorithm>
#include <cstdint>
#include <deque>
#include <format>
#include <list>
#include <memory>
#include <memory_resource>
#include <ranges>
#include <span>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "kiln/util/contract_macros.hpp"

export module kiln.app.plugin.PluginTree;

import kiln.app.App;
import kiln.app.memory.Arena;
import kiln.app.plugin.ErasedPlugin;
import kiln.app.plugin.hash_plugin;
import kiln.app.plugin.meta_plugin_c;
import kiln.app.plugin.meta_plugin_injection_c;
import kiln.app.plugin.plugin_c;
import kiln.app.plugin.plugin_injection_c;
import kiln.app.plugin.PluginStack;
import kiln.app.plugin.strip_plugin_dependency_t;
import kiln.util.Any;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.type_list_all_of;
import kiln.util.contracts;
import kiln.util.for_each;
import kiln.util.Function;
import kiln.util.GenericStack;
import kiln.util.OptionalRef;
import kiln.util.reflection;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.forward_like;
import kiln.util.type_traits.result_of;
import kiln.util.TypeList;

namespace kiln::app {

namespace internal {

template <typename T>
concept represents_optional_dependency_c = requires {
    requires util::specialization_of_c<T, util::OptionalRef>;
};

class ErasedPluginInjection {
public:
    // required for interfacing with the standard
    using allocator_type =   // NOLINT(*-identifier-naming)
        std::pmr::polymorphic_allocator<>;

    using Function = util::MoveOnlyFunction<
        void(PluginStack&, std::span<const uint64_t> resolved_dependency_hash_set) &&,
        0>;


    template <typename PluginInjection_T>
    explicit ErasedPluginInjection(
        PluginInjection_T&& plugin_injection,
        std::pair<std::pmr::vector<uint64_t>, uint32_t>
            unresolved_and_resolved_dependency_hashes
    );
    template <typename PluginInjection_T>
    explicit ErasedPluginInjection(
        std::allocator_arg_t,
        const allocator_type&,
        PluginInjection_T&& plugin_injection,
        std::pair<std::pmr::vector<uint64_t>, uint32_t>
            unresolved_and_resolved_dependency_hashes
    );
    ErasedPluginInjection(ErasedPluginInjection&&, const allocator_type&);


    // required for interfacing with the standard
    [[nodiscard]]
    auto get_allocator() const -> allocator_type;


    [[nodiscard]]
    auto hash() const noexcept -> uint64_t;
    [[nodiscard]]
    auto name() const noexcept -> std::string_view;
    [[nodiscard]]
    auto dependency_hashes() const noexcept -> std::span<const uint64_t>;
    [[nodiscard]]
    auto resolved_dependency_hash_set() const noexcept -> std::span<const uint64_t>;
    [[nodiscard]]
    auto unresolved_dependency_hash_set() const noexcept -> std::span<const uint64_t>;

    template <typename Self_T>
    auto underlying_function(this Self_T&&) -> util::forward_like_t<Function, Self_T>;

    auto resolve_dependency(uint64_t new_plugin_hash) -> void;

    auto operator()(PluginStack& plugin_stack) && -> void;

private:
    Function                   m_function;
    uint64_t                   m_hash;
    std::string_view           m_name;
    std::pmr::vector<uint64_t> m_unresolved_and_resolved_dependency_hashes;
    uint32_t                   m_unresolved_dependency_hash_count;
};

}   // namespace internal

export class PluginTree {
public:
    // required for interfacing with the standard
    using allocator_type =   // NOLINT(*-identifier-naming)
        std::pmr::polymorphic_allocator<>;

    explicit PluginTree(const allocator_type& allocator);

    [[nodiscard]]
    auto contains(uint64_t plugin_hash) const noexcept -> bool;

    template <decays_to_plugin_injection_c PluginInjection_T>
    auto plug_in(
        PluginInjection_T&&        plugin_injection,
        std::pmr::memory_resource& transitive_memory_resource =
            *std::pmr::get_default_resource()
    ) -> std::decay_t<PluginInjection_T>&;

    template <decays_to_meta_plugin_injection_c PluginInjection_T>
    auto plug_in_meta(
        PluginInjection_T&&        plugin_injection,
        std::pmr::memory_resource& transitive_memory_resource =
            *std::pmr::get_default_resource()
    ) -> std::decay_t<PluginInjection_T>&;

    auto build_plugins(
        App&                       app,
        std::pmr::memory_resource& transitive_memory_resource =
            *std::pmr::get_default_resource()
    ) && -> void;

private:
    struct PluginNameChainNode {
        const PluginNameChainNode* previous{};
        std::string_view           plugin_name;

        [[nodiscard]]
        auto format() const -> std::string;

    private:
        auto format(std::string& out_string, std::size_t capacity) const -> void;
    };

    std::pmr::polymorphic_allocator<>                m_resource_allocator;
    std::pmr::monotonic_buffer_resource              m_injections_resource;
    std::pmr::deque<internal::ErasedPluginInjection> m_plugin_injections;
    std::pmr::synchronized_pool_resource             m_unresolved_plugin_hashes_resource;
    std::pmr::deque<uint64_t> m_unresolved_optional_dependency_hashes;


    template <typename PluginInjection_T>
    auto plug_in_maybe_meta(
        PluginInjection_T&&        plugin_injection,
        std::pmr::memory_resource& transitive_memory_resource
    ) -> std::decay_t<PluginInjection_T>&;

    template <typename Plugin_T>
    auto check_for_duplicated_plugin() const -> void;
    template <typename PluginInjection_T>
    auto check_required_injection_dependencies() const -> void;
    template <typename Plugin_T>
    auto check_required_build_dependencies() const -> void;
    template <typename PluginInjection_T>
    auto check_required_dependencies() const -> void;
    template <typename PluginInjection_T>
    auto check_for_cyclic_dependencies(
        std::pmr::memory_resource& transitive_memory_resource
    ) const -> void;
    auto check_for_new_cyclic_dependency(
        uint64_t                   new_plugin_hash,
        std::string_view           new_plugin_name,
        uint64_t                   dependency_hash,
        const PluginNameChainNode& visited_plugin_names
    ) const -> void;


    template <typename PluginInjection_T>
    [[nodiscard]]
    auto collect_unresolved_and_resolved_dependency_plugin_hashes(
        const std::pmr::polymorphic_allocator<>& allocator
    ) const -> std::pair<std::pmr::vector<uint64_t>, uint32_t>;
    template <typename PluginInjection_T>
    auto collect_unresolved_dependency_hashes(std::pmr::vector<uint64_t>& out) const
        -> void;
    template <class PluginInjection_T>
    static auto collect_rest_of_dependency_hashes(std::pmr::vector<uint64_t>& out)
        -> void;


    auto collect_all_resolved_dependency_hashes(
        const internal::ErasedPluginInjection& plugin_injection,
        std::pmr::deque<uint64_t>&             out
    ) const -> void;


    template <typename PluginInjection_T>
    auto push_back(PluginInjection_T&& plugin_injection)
        -> internal::ErasedPluginInjection&;
    auto reestablish_internal_ordering_of_plugins(
        const internal::ErasedPluginInjection& new_plugin,
        std::pmr::memory_resource&             transitive_memory_resource =
            *std::pmr::get_default_resource()
    ) -> void;
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
                return plugin_stack.find<strip_plugin_dependency_t<Dependencies_T>>();
            }
            else
            {
                return plugin_stack.at<strip_plugin_dependency_t<Dependencies_T>>();
            }
        }()...
    );
}

template <typename PluginInjection_T>
struct PluginInjectionLambda {
    PluginInjection_T plugin_injection;

    auto operator()(
        PluginStack&                    plugin_stack,
        const std::span<const uint64_t> resolved_dependency_hash_set
    ) && -> void
    {
        auto& plugin = plugin_stack.insert(
            std::apply(
                std::move(plugin_injection),
                collect_dependencies(
                    util::arguments_of_t<PluginInjection_T>{}, plugin_stack
                )
            )
        );
        plugin.set_resolved_dependency_hash_set(resolved_dependency_hash_set);
    }
};

template <typename PluginInjection_T>
ErasedPluginInjection::ErasedPluginInjection(
    PluginInjection_T&& plugin_injection,
    std::pair<std::pmr::vector<uint64_t>, uint32_t> unresolved_and_resolved_dependency_hashes
)
    : m_function{
          std::allocator_arg,
          unresolved_and_resolved_dependency_hashes.first.get_allocator(),
          PluginInjectionLambda<std::decay_t<PluginInjection_T>>{
              std::forward<PluginInjection_T>(plugin_injection)   //
          }   //
      },
      m_hash{ hash_plugin<util::result_of_t<PluginInjection_T>>() },
      m_name{ util::name_of<util::result_of_t<PluginInjection_T>>() },
      m_unresolved_and_resolved_dependency_hashes{
          std::move(unresolved_and_resolved_dependency_hashes.first)
      },
      m_unresolved_dependency_hash_count{ unresolved_and_resolved_dependency_hashes.second }
{
}

template <typename PluginInjection_T>
ErasedPluginInjection::ErasedPluginInjection(
    std::allocator_arg_t,
    const allocator_type& allocator,
    PluginInjection_T&&   plugin_injection,
    std::pair<std::pmr::vector<uint64_t>, uint32_t> unresolved_and_resolved_dependency_hashes
)
    : ErasedPluginInjection{
          std::forward<PluginInjection_T>(plugin_injection),
          std::pair{
                    std::pmr::vector<uint64_t>{
                  std::move(unresolved_and_resolved_dependency_hashes.first),
                  allocator,
              },                                                  //
              unresolved_and_resolved_dependency_hashes.second,   //
          }
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

auto PluginTree::contains(const uint64_t plugin_hash) const noexcept -> bool
{
    return std::ranges::contains(
        m_plugin_injections, plugin_hash, &internal::ErasedPluginInjection::hash
    );
}

template <decays_to_plugin_injection_c PluginInjection_T>
auto PluginTree::plug_in(
    PluginInjection_T&&        plugin_injection,
    std::pmr::memory_resource& transitive_memory_resource
) -> std::decay_t<PluginInjection_T>&
{
    return plug_in_maybe_meta(
        std::forward<PluginInjection_T>(plugin_injection), transitive_memory_resource
    );
}

template <decays_to_meta_plugin_injection_c PluginInjection_T>
auto PluginTree::plug_in_meta(
    PluginInjection_T&&        plugin_injection,
    std::pmr::memory_resource& transitive_memory_resource
) -> std::decay_t<PluginInjection_T>&
{
    return plug_in_maybe_meta(
        std::forward<PluginInjection_T>(plugin_injection), transitive_memory_resource
    );
}

template <typename PluginInjection_T>
auto PluginTree::plug_in_maybe_meta(
    PluginInjection_T&&        plugin_injection,
    std::pmr::memory_resource& transitive_memory_resource
) -> std::decay_t<PluginInjection_T>&
{
    using Plugin = util::result_of_t<PluginInjection_T>;
    constexpr static uint64_t plugin_hash{ hash_plugin<Plugin>() };

    check_for_duplicated_plugin<Plugin>();
    check_required_dependencies<PluginInjection_T>();
    check_for_cyclic_dependencies<PluginInjection_T>(transitive_memory_resource);

    internal::ErasedPluginInjection& erased_result{
        push_back(std::forward<PluginInjection_T>(plugin_injection))
    };
    std::decay_t<PluginInjection_T>& result{
        util::any_cast<internal::PluginInjectionLambda<std::decay_t<PluginInjection_T>>>(
            erased_result.underlying_function()
        )
            .plugin_injection
    };

    reestablish_internal_ordering_of_plugins(erased_result, transitive_memory_resource);
    resolve(plugin_hash);

    return result;
}

template <typename Plugin_T>
auto PluginTree::check_for_duplicated_plugin() const -> void
{
    PRECOND(
        !contains(hash_plugin<Plugin_T>()),
        std::format(
            "Attempt to inject plugin of type `{}`, but it has already been injected",
            util::name_of<Plugin_T>()
        )
    );
}

template <typename PluginInjection_T>
auto PluginTree::check_required_injection_dependencies() const -> void
{
    util::for_each(
        util::arguments_of_t<PluginInjection_T>{},
        [this]<typename Dependency_T> -> void
        {
            std::ignore = this;

            if constexpr (!internal::represents_optional_dependency_c<Dependency_T>)
            {
                PRECOND(
                    (contains(hash_plugin<std::remove_cvref_t<Dependency_T>>())),
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

template <typename Plugin_T>
auto PluginTree::check_required_build_dependencies() const -> void
{
    util::for_each(
        util::arguments_of_t<decltype(&Plugin_T::build)>{},
        [this]<typename Dependency_T> -> void
        {
            std::ignore = this;

            if constexpr (!internal::represents_optional_dependency_c<Dependency_T>)
            {
                PRECOND(
                    contains(util::hash_u64<std::remove_cvref_t<Dependency_T>>()),
                    std::format(
                        "Dependent plugin with context variable of type `{}` "   //
                        "must be injected before `{}`",
                        util::name_of<std::remove_cvref_t<Dependency_T>>(),
                        util::name_of<Plugin_T>()
                    )
                );
            }
        }
    );
}

template <typename PluginInjection_T>
auto PluginTree::check_required_dependencies() const -> void
{
    check_required_injection_dependencies<PluginInjection_T>();
    if constexpr (!meta_plugin_c<util::result_of_t<PluginInjection_T>>)
    {
        check_required_build_dependencies<util::result_of_t<PluginInjection_T>>();
    }
}

template <typename MaybeMetaPlugin_T>
[[nodiscard]]
constexpr auto count_build_dependencies() -> uint32_t
{
    if constexpr (!meta_plugin_c<MaybeMetaPlugin_T>)
    {
        return util::arguments_of_t<decltype(&MaybeMetaPlugin_T::build)>::size();
    }
    else
    {
        return 0;
    }
}

template <typename PluginInjection_T>
[[nodiscard]]
auto collect_direct_dependency_hashes(
    std::pmr::memory_resource& transitive_memory_resource
) -> std::pmr::vector<uint64_t>
{
    using Plugin = util::result_of_t<PluginInjection_T>;

    std::pmr::vector<uint64_t> result{ &transitive_memory_resource };
    result.reserve(
        util::arguments_of_t<PluginInjection_T>::size()
        + count_build_dependencies<Plugin>()
    );

    util::for_each(
        util::arguments_of_t<PluginInjection_T>{},
        [&result]<typename InjectionDependency_T> -> void
        {
            result.push_back(
                hash_plugin<strip_plugin_dependency_t<InjectionDependency_T>>()
            );
        }
    );
    if constexpr (!meta_plugin_c<Plugin>)
    {
        util::for_each(
            util::arguments_of_t<decltype(&Plugin::build)>{},
            [&result]<typename BuildDependency_T> -> void
            {
                result.push_back(
                    util::hash_u64<strip_plugin_dependency_t<BuildDependency_T>>()
                );
            }
        );
    }

    std::ranges::sort(result);
    const auto [first, last] = std::ranges::unique(result.begin(), result.end());
    result.erase(first, last);

    return result;
}

template <typename PluginInjection_T>
auto PluginTree::check_for_cyclic_dependencies(
    std::pmr::memory_resource& transitive_memory_resource
) const -> void
{
    using Plugin = util::result_of_t<PluginInjection_T>;

    if (!std::ranges::binary_search(
            m_unresolved_optional_dependency_hashes, hash_plugin<Plugin>()
        ))
    {
        return;
    }

    const std::pmr::vector<uint64_t> dependency_hashes{
        collect_direct_dependency_hashes<PluginInjection_T>(transitive_memory_resource)
    };

    const PluginNameChainNode plugin_name_chain_node{
        .plugin_name = util::name_of<Plugin>(),
    };
    for (const uint64_t dependency_hash : dependency_hashes)
    {
        check_for_new_cyclic_dependency(
            hash_plugin<Plugin>(),
            util::name_of<Plugin>(),
            dependency_hash,
            plugin_name_chain_node
        );
    }
}

template <typename PluginInjection_T>
auto PluginTree::collect_unresolved_and_resolved_dependency_plugin_hashes(
    const std::pmr::polymorphic_allocator<>& allocator
) const -> std::pair<std::pmr::vector<uint64_t>, uint32_t>
{
    std::pair<std::pmr::vector<uint64_t>, uint32_t> result{
        std::piecewise_construct,
        std::tuple{ allocator },
        std::tuple{ 0uz },
    };
    result.first.reserve(util::arguments_of_t<PluginInjection_T>::size());

    collect_unresolved_dependency_hashes<PluginInjection_T>(result.first);
    std::ranges::sort(result.first);
    result.second = static_cast<uint32_t>(result.first.size());

    collect_rest_of_dependency_hashes<PluginInjection_T>(result.first);
    std::ranges::sort(std::views::drop(result.first, result.second));

    return result;
}

auto collect_unresolved_dependency_plugin_hash(
    std::pmr::vector<uint64_t>&                             out,
    const std::pmr::deque<internal::ErasedPluginInjection>& plugin_injections,
    const uint64_t                                          dependency_hash
) -> void
{
    if (!std::ranges::contains(out, dependency_hash)
        && !std::ranges::contains(
            plugin_injections, dependency_hash, &internal::ErasedPluginInjection::hash
        ))
    {
        out.push_back(dependency_hash);
    }
}

template <typename PluginInjection_T>
auto PluginTree::collect_unresolved_dependency_hashes(
    std::pmr::vector<uint64_t>& out
) const -> void
{
    using Plugin = util::result_of_t<PluginInjection_T>;

    util::for_each(
        util::arguments_of_t<PluginInjection_T>{},
        [this, &out]<typename Dependency_T> -> void
        {
            if constexpr (internal::represents_optional_dependency_c<Dependency_T>)
            {
                using Dependency = strip_plugin_dependency_t<Dependency_T>;

                collect_unresolved_dependency_plugin_hash(
                    out, m_plugin_injections, hash_plugin<Dependency>()
                );
            }
        }
    );

    if constexpr (!meta_plugin_c<Plugin>)
    {
        util::for_each(
            util::arguments_of_t<decltype(&Plugin::build)>{},
            [this, &out]<typename Dependency_T> -> void
            {
                if constexpr (internal::represents_optional_dependency_c<Dependency_T>)
                {
                    using Dependency = strip_plugin_dependency_t<Dependency_T>;

                    collect_unresolved_dependency_plugin_hash(
                        out, m_plugin_injections, util::hash_u64<Dependency>()
                    );
                }
            }
        );
    }
}

auto collect_dependency_plugin_hash(
    std::pmr::vector<uint64_t>& out,
    const uint64_t              dependency_hash
) -> void
{
    if (!std::ranges::contains(out, dependency_hash))
    {
        out.push_back(dependency_hash);
    }
}

template <typename PluginInjection_T>
auto PluginTree::collect_rest_of_dependency_hashes(std::pmr::vector<uint64_t>& out)
    -> void
{
    using Plugin = util::result_of_t<PluginInjection_T>;

    util::for_each(
        util::arguments_of_t<PluginInjection_T>{},
        [&out]<typename Dependency_T> -> void
        {
            using Dependency = strip_plugin_dependency_t<Dependency_T>;

            collect_dependency_plugin_hash(out, hash_plugin<Dependency>());
        }
    );

    if constexpr (!meta_plugin_c<Plugin>)
    {
        util::for_each(
            util::arguments_of_t<decltype(&Plugin::build)>{},
            [&out]<typename Dependency_T> -> void
            {
                using Dependency = strip_plugin_dependency_t<Dependency_T>;

                collect_dependency_plugin_hash(out, util::hash_u64<Dependency>());
            }
        );
    }
}

template <typename PluginInjection_T>
auto PluginTree::push_back(PluginInjection_T&& plugin_injection)
    -> internal::ErasedPluginInjection&
{
    internal::ErasedPluginInjection& result = m_plugin_injections.emplace_back(
        std::forward<PluginInjection_T>(plugin_injection),
        collect_unresolved_and_resolved_dependency_plugin_hashes<PluginInjection_T>(
            m_plugin_injections.get_allocator()
        )
    );

    m_unresolved_optional_dependency_hashes.append_range(
        result.unresolved_dependency_hash_set()
    );
    std::ranges::sort(m_unresolved_optional_dependency_hashes);

    return result;
}

}   // namespace kiln::app
