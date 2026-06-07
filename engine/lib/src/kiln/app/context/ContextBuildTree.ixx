module;

#include <algorithm>
#include <cstdint>
#include <deque>
#include <format>
#include <memory_resource>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "kiln/util/contract_macros.hpp"

export module kiln.app.context.ContextBuildTree;

import kiln.app.config.Config;
import kiln.app.context.context_builder_c;
import kiln.app.context.context_c;
import kiln.app.context.Contexts;
import kiln.app.context.ErasedContextBuilder;
import kiln.app.context.strip_dependency_t;
import kiln.app.memory.MemoryArena;
import kiln.util.any_of;
import kiln.util.contracts;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.type_list_all_of;
import kiln.util.containers.MoveOnlyFunction;
import kiln.util.containers.OptionalRef;
import kiln.util.for_each;
import kiln.util.reflection;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.result_of;
import kiln.util.TypeList;

namespace kiln::app {

export class ContextBuildTree {
public:
    // required for interfacing with the standard
    using allocator_type =   // NOLINT(*-identifier-naming)
        std::pmr::polymorphic_allocator<>;


    explicit ContextBuildTree(MemoryArena&& memory_arena, const Config& config);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;


    template <context_c Context_T>
    auto register_context(
        std::pmr::memory_resource& transient_memory_resource
        = *std::pmr::get_default_resource()
    ) -> void;

    [[nodiscard]]
    auto build(std::pmr::memory_resource& transient_memory_resource) && -> Contexts;

private:
    class Injector {
    public:
        explicit Injector(ContextBuildTree& context_build_tree);

        template <typename Builder_T>
        auto insert_builder(
            Builder_T&&,
            std::pmr::memory_resource& transient_memory_resource
        ) -> void;

        template <typename Builder_T>
        [[nodiscard]]
        auto builder_at() -> Builder_T&;
        template <typename Builder_T>
        [[nodiscard]]
        auto find_builder() -> util::OptionalRef<Builder_T>;

    private:
        ContextBuildTree& m_build_tree;
    };

    using ErasedInjection
        = util::MoveOnlyFunction<auto(Injector&, std::pmr::memory_resource&) &&->void>;

    struct DependencyChainNode {
        const DependencyChainNode* previous{};
        uint64_t                   hash;
        std::string_view           name;

        [[nodiscard]]
        auto contains(uint64_t hash) const noexcept -> bool;

        [[nodiscard]]
        auto format(const std::pmr::string::allocator_type& allocator = {}) const
            -> std::pmr::string;

    private:
        auto format(std::pmr::string& out, std::size_t capacity) const -> void;
    };

    class DependencyDescriptor {
    public:
        using allocator_type = allocator_type;

        DependencyDescriptor(const DependencyDescriptor&, const allocator_type&);
        DependencyDescriptor(DependencyDescriptor&&, const allocator_type&);

        explicit DependencyDescriptor(
            std::allocator_arg_t,
            const allocator_type& allocator,
            uint64_t              hash,
            std::pair<std::pmr::deque<uint64_t>, uint32_t>&&
                all_missing_and_resolved_dependency_hashes
        );

        [[nodiscard]]
        auto get_allocator() const noexcept -> allocator_type;

        [[nodiscard]]
        auto hash() const noexcept -> uint64_t;
        [[nodiscard]]
        auto misses_dependency(uint64_t hash) const noexcept -> bool;
        [[nodiscard]]
        auto resolved_dependencies() const noexcept -> std::span<const uint64_t>;

        auto resolve_missing_dependency(uint64_t hash) -> void;

    private:
        uint64_t                   m_hash;
        std::pmr::vector<uint64_t> m_all_missing_and_resolved_dependency_hashes;
        std::uint32_t              m_number_of_missing_dependencies;
    };

    Contexts                               m_contexts;
    std::pmr::vector<ErasedContextBuilder> m_builders;
    std::pmr::vector<uint64_t>             m_builder_hashes;
    std::pmr::vector<DependencyDescriptor> m_builder_dependencies;
    std::pmr::vector<ErasedInjection>      m_injections;
    std::pmr::vector<uint64_t>             m_injection_hashes;
    std::pmr::vector<DependencyDescriptor> m_injection_dependencies;
    std::pmr::vector<uint64_t>             m_contained_hashes;


    [[nodiscard]]
    auto contains(uint64_t context_hash) const noexcept -> bool;
    [[nodiscard]]
    auto context_builder(uint64_t hash) const -> const ErasedContextBuilder&;
    [[nodiscard]]
    auto find_context_builder(uint64_t hash) -> util::OptionalRef<ErasedContextBuilder>;
    [[nodiscard]]
    auto find_context_builder(uint64_t hash) const
        -> util::OptionalRef<const ErasedContextBuilder>;

    template <typename ContextBuilder_T>
    auto unsafe_register_context_builder(
        ContextBuilder_T&&         context_builder,
        std::pmr::memory_resource& transient_memory_resource
        = *std::pmr::get_default_resource()
    ) -> void;

    template <typename Context_T>
    [[nodiscard]]
    auto check_for_cyclic_dependency(
        std::pmr::memory_resource& transient_memory_resource
    ) const -> bool;
    template <typename Context_T>
    [[nodiscard]]
    auto check_for_cyclic_dependency(
        const DependencyChainNode&  dependency_chain,
        std::pmr::vector<uint64_t>& hash_cache,
        std::pmr::memory_resource&  transient_memory_resource
    ) const -> bool;
    template <typename Builder_T>
    [[nodiscard]]
    auto check_for_cyclic_dependency_via_builder(
        const DependencyChainNode&  dependency_chain,
        std::pmr::vector<uint64_t>& hash_cache,
        std::pmr::memory_resource&  transient_memory_resource
    ) const -> bool;
    template <typename Builder_T>
    [[nodiscard]]
    auto check_for_cyclic_dependency_via_create(
        const DependencyChainNode&  dependency_chain,
        std::pmr::vector<uint64_t>& hash_cache,
        std::pmr::memory_resource&  transient_memory_resource
    ) const -> bool;
    template <typename Builder_T>
    [[nodiscard]]
    auto check_for_cyclic_dependency_via_build(
        const DependencyChainNode&  dependency_chain,
        std::pmr::vector<uint64_t>& hash_cache,
        std::pmr::memory_resource&  transient_memory_resource
    ) const -> bool;


    template <typename Context_T, typename... Args_T>
    auto emplace_context(Args_T&&... args) -> void;


    template <typename Builder_T>
    auto register_builder(std::pmr::memory_resource& transient_memory_resource) -> void;
    template <typename Builder_T>
    auto resolve_build_dependencies(std::pmr::memory_resource& transient_memory_resource)
        -> void;
    template <typename Builder_T, typename... Args_T>
    auto emplace_builder(
        std::pmr::memory_resource& transient_memory_resource,
        Args_T&&... args
    ) -> void;
    template <typename Builder_T>
    auto collect_all_missing_build_dependency_hashes(std::pmr::deque<uint64_t>& out)
        -> void;
    template <typename Builder_T>
    [[nodiscard]]
    auto collect_all_missing_and_resolved_build_dependency_hashes(
        const std::pmr::polymorphic_allocator<uint64_t>& allocator
    ) -> std::pair<std::pmr::deque<uint64_t>, uint32_t>;
    auto fix_order_of_builders(
        uint64_t                   latest_hash,
        std::span<const uint64_t>  resolved_builder_dependencies,
        std::pmr::memory_resource& transient_memory_resource
    ) -> void;
    template <typename Builder_T>
    [[nodiscard]]
    auto collect_all_resolved_build_dependency_hashes(
        const std::pmr::polymorphic_allocator<uint64_t>& allocator
    ) -> std::pmr::deque<uint64_t>;
    template <typename Builder_T>
    auto collect_all_resolved_build_dependency_hashes(std::pmr::deque<uint64_t>& out)
        -> void;
    auto mark_builder_as_resolved(uint64_t hash) -> void;


    template <typename Builder_T>
    auto register_injection(std::pmr::memory_resource& transient_memory_resource) -> void;
    template <typename Builder_T>
    auto resolve_injection_dependencies(
        std::pmr::memory_resource& transient_memory_resource
    ) -> void;
    template <typename... Dependencies_T>
    [[nodiscard]]
    static auto collect_creation_dependencies(
        util::TypeList<Dependencies_T...>,
        Injector& injector
    ) -> std::tuple<Dependencies_T...>;
    template <typename Builder_T>
    auto emplace_injection(std::pmr::memory_resource& transient_memory_resource) -> void;
    template <typename Builder_T>
    [[nodiscard]]
    auto collect_all_missing_injection_dependency_hashes(
        const std::pmr::polymorphic_allocator<uint64_t>& allocator
    ) -> std::pair<std::pmr::deque<uint64_t>, uint32_t>;
    auto fix_order_of_injections(
        uint64_t                   latest_hash,
        std::span<const uint64_t>  resolved_injection_dependencies,
        std::pmr::memory_resource& transient_memory_resource
    ) -> void;
    template <typename Builder_T>
    [[nodiscard]]
    auto collect_all_resolved_injection_dependency_hashes(
        const std::pmr::polymorphic_allocator<uint64_t>& allocator
    ) -> std::pmr::deque<uint64_t>;
    auto mark_injection_as_resolved(uint64_t hash) -> void;

    [[nodiscard]]
    auto check_for_configuration_dependencies() const -> bool;
    [[nodiscard]]
    auto check_for_configuration_dependencies(
        const ErasedContextBuilder& erased_builder
    ) const -> bool;
    [[nodiscard]]
    auto check_for_cyclic_configuration_dependencies() const -> bool;
    [[nodiscard]]
    auto check_for_cyclic_configuration_dependencies(
        const ErasedContextBuilder& erased_builder
    ) const -> bool;
    [[nodiscard]]
    auto check_for_cyclic_configuration_dependency(
        uint64_t                   hash,
        std::string_view           original_builder_name,
        std::string_view           original_dependency_name,
        uint64_t                   dependency_hash,
        const DependencyChainNode& visited_dependency_names
    ) const -> bool;
    [[nodiscard]]
    auto collect_all_resolved_build_dependency_hashes(
        uint64_t                                         hash,
        const std::pmr::polymorphic_allocator<uint64_t>& allocator
    ) -> std::pmr::deque<uint64_t>;
    auto collect_all_resolved_build_dependency_hashes(
        const ErasedContextBuilder& builder,
        std::pmr::deque<uint64_t>&  out
    ) -> void;
    auto fix_build_order(std::pmr::memory_resource& transient_memory_resource) -> void;
};

}   // namespace kiln::app

namespace kiln::app {

template <typename Context_T>
[[nodiscard]]
consteval auto hash_context() noexcept -> uint64_t
{
    return util::hash_u64<Context_T>();
}

template <typename Builder_T>
[[nodiscard]]
consteval auto hash_builder() noexcept -> uint64_t
{
    return hash_context<util::result_of_t<decltype(&Builder_T::build)>>();
}

template <typename Injection_T>
[[nodiscard]]
consteval auto hash_injection() noexcept -> uint64_t
{
    return hash_builder<util::result_of_t<Injection_T>>();
}

template <typename Builder_T>
[[nodiscard]]
consteval auto context_name_of_builder() -> std::string_view
{
    return util::name_of<util::result_of_t<decltype(&Builder_T::build)>>();
}

template <typename Injection_T>
[[nodiscard]]
consteval auto context_name_of_injection() -> std::string_view
{
    return context_name_of_builder<util::result_of_t<Injection_T>>();
}

template <typename T>
struct RepresentsBuilderDependency {
    constexpr static bool value{ context_builder_c<strip_dependency_t<T>> };
};

export template <typename T, typename Builder_T>
concept creates_builder_c = requires {
    requires std::is_same_v<util::result_of_t<T>, Builder_T>
                 && util::type_list_all_of_c<
                     util::arguments_of_t<T>,
                     RepresentsBuilderDependency>;
};

template <typename T>
concept represents_optional_dependency_c
    = requires { requires util::specialization_of_c<T, util::OptionalRef>; };

template <context_c Context_T>
auto ContextBuildTree::register_context(
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    constexpr static uint64_t hash{ hash_context<Context_T>() };

    if (contains(hash))
    {
        return;
    }

    if constexpr (requires { typename Context_T::Builder; })
    {
        static_assert(context_builder_c<typename Context_T::Builder>);
        static_assert(
            std::is_same_v<util::result_of_t<decltype(&Context_T::Builder::build)>, Context_T>
        );

        PRECOND(!check_for_cyclic_dependency<Context_T>(transient_memory_resource));

        register_builder<typename Context_T::Builder>(transient_memory_resource);
    }
    else
    {
        emplace_context<Context_T>();
    }

    m_contained_hashes.insert(std::ranges::upper_bound(m_contained_hashes, hash), hash);
}

template <typename Builder_T>
auto ContextBuildTree::Injector::insert_builder(
    Builder_T&&                builder,
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    m_build_tree.emplace_builder<std::remove_cvref_t<Builder_T>>(
        transient_memory_resource,
        std::forward<Builder_T>(builder)
    );
}

template <typename Builder_T>
auto ContextBuildTree::Injector::builder_at() -> Builder_T&
{
    return *find_builder<Builder_T>();
}

template <typename Builder_T>
auto ContextBuildTree::Injector::find_builder() -> util::OptionalRef<Builder_T>
{
    return m_build_tree.find_context_builder(hash_builder<Builder_T>())
        .transform(
            [](ErasedContextBuilder& erased_context_builder) -> Builder_T&
            {
                return static_cast<Builder_T&>(*erased_context_builder);   //
            }
        );
}

auto ContextBuildTree::contains(const uint64_t context_hash) const noexcept -> bool
{
    return std::ranges::binary_search(m_contained_hashes, context_hash);
}

template <typename ContextBuilder_T>
auto ContextBuildTree::unsafe_register_context_builder(
    ContextBuilder_T&&         context_builder,
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    using StrippedBuilder = std::remove_cvref_t<ContextBuilder_T>;

    constexpr static uint64_t hash{ hash_builder<StrippedBuilder>() };

    PRECOND(!contains(hash));
    PRECOND(
        !check_for_cyclic_dependency<util::result_of_t<decltype(&StrippedBuilder::build)>>(
            transient_memory_resource
        )
    );

    resolve_build_dependencies<StrippedBuilder>(transient_memory_resource);
    emplace_builder<StrippedBuilder>(
        transient_memory_resource,
        std::forward<ContextBuilder_T>(context_builder)
    );

    m_contained_hashes.insert(std::ranges::upper_bound(m_contained_hashes, hash), hash);
}

template <typename Context_T>
auto ContextBuildTree::check_for_cyclic_dependency(
    std::pmr::memory_resource& transient_memory_resource
) const -> bool
{
    if constexpr (requires { typename Context_T::Builder; })
    {
        static_assert(context_builder_c<typename Context_T::Builder>);
        static_assert(
            std::is_same_v<util::result_of_t<decltype(&Context_T::Builder::build)>, Context_T>
        );

        constexpr static uint64_t hash{ hash_context<Context_T>() };

        std::pmr::vector<uint64_t> hash_cache{ &transient_memory_resource };
        hash_cache.push_back(hash);

        const DependencyChainNode dependency_chain_node{
            .hash{ hash },
            .name{ util::name_of<Context_T>() },
        };

        return check_for_cyclic_dependency_via_builder<typename Context_T::Builder>(
            dependency_chain_node,
            hash_cache,
            transient_memory_resource
        );
    }

    return false;
}

template <typename Context_T>
auto ContextBuildTree::check_for_cyclic_dependency(
    const DependencyChainNode&  dependency_chain,
    std::pmr::vector<uint64_t>& hash_cache,
    std::pmr::memory_resource&  transient_memory_resource
) const -> bool
{
    constexpr static uint64_t         hash{ hash_context<Context_T>() };
    constexpr static std::string_view name{ util::name_of<Context_T>() };

    if (dependency_chain.contains(hash))
    {
        PRECOND(
            false,
            std::format(
                "Cyclic dependency detected - context of type `{}` depends on itself "   //
                "({} -> {})",
                name,
                dependency_chain.format(&transient_memory_resource),
                name
            )
        );
        return true;
    }

    if (std::ranges::contains(hash_cache, hash))
    {
        return false;
    }
    hash_cache.insert(std::ranges::upper_bound(hash_cache, hash), hash);

    if constexpr (requires { typename Context_T::Builder; })
    {
        static_assert(context_builder_c<typename Context_T::Builder>);
        static_assert(
            std::is_same_v<util::result_of_t<decltype(&Context_T::Builder::build)>, Context_T>
        );

        const DependencyChainNode dependency_chain_node{
            .previous = &dependency_chain,
            .hash     = hash,
            .name     = name,
        };

        return check_for_cyclic_dependency_via_builder<typename Context_T::Builder>(
            dependency_chain_node,
            hash_cache,
            transient_memory_resource
        );
    }

    return false;
}

template <typename Builder_T>
auto ContextBuildTree::check_for_cyclic_dependency_via_builder(
    const DependencyChainNode&  dependency_chain,
    std::pmr::vector<uint64_t>& hash_cache,
    std::pmr::memory_resource&  transient_memory_resource
) const -> bool
{
    return check_for_cyclic_dependency_via_create<Builder_T>(
               dependency_chain,
               hash_cache,
               transient_memory_resource
           )
        || check_for_cyclic_dependency_via_build<Builder_T>(
               dependency_chain,
               hash_cache,
               transient_memory_resource
        );
}

template <typename Builder_T>
auto ContextBuildTree::check_for_cyclic_dependency_via_create(
    const DependencyChainNode&  dependency_chain,
    std::pmr::vector<uint64_t>& hash_cache,
    std::pmr::memory_resource&  transient_memory_resource
) const -> bool
{
    if constexpr (requires { Builder_T::create; })
    {
        static_assert(creates_builder_c<decltype(Builder_T::create), Builder_T>);

        return util::any_of(
            util::arguments_of_t<decltype(Builder_T::create)>{},
            [&]<typename Dependency_T> -> bool
            {
                using DependencyContext
                    = util::result_of_t<decltype(&strip_dependency_t<Dependency_T>::build)>;

                return check_for_cyclic_dependency<DependencyContext>(
                    dependency_chain,
                    hash_cache,
                    transient_memory_resource
                );
            }
        );
    }

    return false;
}

template <typename Builder_T>
auto ContextBuildTree::check_for_cyclic_dependency_via_build(
    const DependencyChainNode&  dependency_chain,
    std::pmr::vector<uint64_t>& hash_cache,
    std::pmr::memory_resource&  transient_memory_resource
) const -> bool
{
    return util::any_of(
        util::arguments_of_t<decltype(&Builder_T::build)>{},
        [&]<typename Dependency_T> -> bool
        {
            using DependencyContext = strip_dependency_t<Dependency_T>;

            return check_for_cyclic_dependency<DependencyContext>(
                dependency_chain,
                hash_cache,
                transient_memory_resource
            );
        }
    );
}

template <typename Context_T, typename... Args_T>
auto ContextBuildTree::emplace_context(Args_T&&... args) -> void
{
    m_contexts.emplace<Context_T>(std::forward<Args_T>(args)...);
}

template <typename Builder_T>
auto ContextBuildTree::register_builder(
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    resolve_build_dependencies<Builder_T>(transient_memory_resource);

    if constexpr (requires { Builder_T::create; })
    {
        static_assert(creates_builder_c<decltype(Builder_T::create), Builder_T>);

        register_injection<Builder_T>(transient_memory_resource);
    }
    else
    {
        emplace_builder<Builder_T>(transient_memory_resource);
    }
}

template <typename Builder_T>
auto ContextBuildTree::resolve_build_dependencies(
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    util::for_each(
        util::arguments_of_t<decltype(&Builder_T::build)>{},
        [&]<typename Dependency_T> -> void
        {
            if constexpr (!represents_optional_dependency_c<Dependency_T>)
            {
                /*
                 * This can end up in an infinite recursion in case of a cyclic
                 * dependency. Cyclic dependencies must be checked before calling this
                 * function.
                 */
                register_context<strip_dependency_t<Dependency_T>>(
                    transient_memory_resource
                );
            }
        }
    );
}

template <typename Builder_T>
auto ContextBuildTree::collect_all_missing_build_dependency_hashes(
    std::pmr::deque<uint64_t>& out
) -> void
{
    util::for_each(
        util::arguments_of_t<decltype(&Builder_T::build)>{},
        [this, &out]<typename Dependency_T>() -> void
        {
            using StrippedDependency = strip_dependency_t<Dependency_T>;

            constexpr static uint64_t dependency_hash{
                hash_context<StrippedDependency>()
            };

            if (const auto iter{ std::ranges::lower_bound(out, dependency_hash) };
                (iter == out.cend() || *iter != dependency_hash)
                && !std::ranges::contains(m_builder_hashes, dependency_hash))
            {
                out.insert(iter, dependency_hash);
            }

            if constexpr (requires { typename StrippedDependency::Builder; })
            {
                collect_all_missing_build_dependency_hashes<
                    typename StrippedDependency::Builder>(out);
            }
        }
    );
}

template <typename Builder_T>
auto ContextBuildTree::collect_all_missing_and_resolved_build_dependency_hashes(
    const std::pmr::polymorphic_allocator<uint64_t>& allocator
) -> std::pair<std::pmr::deque<uint64_t>, uint32_t>
{
    std::pair<std::pmr::deque<uint64_t>, uint32_t> result{ allocator, 0 };

    collect_all_missing_build_dependency_hashes<Builder_T>(result.first);

    result.second = static_cast<uint32_t>(result.first.size());

    collect_all_resolved_build_dependency_hashes<Builder_T>(result.first);

    return result;
}

template <typename Builder_T>
auto ContextBuildTree::collect_all_resolved_build_dependency_hashes(
    const std::pmr::polymorphic_allocator<uint64_t>& allocator
) -> std::pmr::deque<uint64_t>
{
    std::pmr::deque<uint64_t> result{
        std::initializer_list{ hash_builder<Builder_T>() },
        allocator,
    };

    collect_all_resolved_build_dependency_hashes<Builder_T>(result);

    return result;
}

template <typename Builder_T>
auto ContextBuildTree::collect_all_resolved_build_dependency_hashes(
    std::pmr::deque<uint64_t>& out
) -> void
{
    const uint32_t starting_index{ static_cast<uint32_t>(out.size()) };

    util::for_each(
        util::arguments_of_t<decltype(&Builder_T::build)>{},
        [&]<typename Dependency_T>(this auto x_self) -> void
        {
            using StrippedDependency = strip_dependency_t<Dependency_T>;

            constexpr static uint64_t dependency_hash{
                hash_context<StrippedDependency>()
            };

            if (const auto iter{
                    std::ranges::lower_bound(
                        std::next(out.begin(), starting_index),
                        out.end(),
                        dependency_hash
                    )   //
                };
                (iter == out.cend() || *iter != dependency_hash)
                && std::ranges::contains(m_builder_hashes, dependency_hash))
            {
                out.insert(iter, dependency_hash);
            }

            if constexpr (requires { typename StrippedDependency::Builder; })
            {
                util::for_each(
                    util::arguments_of_t<decltype(&StrippedDependency::Builder::build)>{},
                    x_self
                );
            }
        }
    );
}

template <typename Builder_T, typename... Args_T>
auto ContextBuildTree::emplace_builder(
    std::pmr::memory_resource& transient_memory_resource,
    Args_T&&... args
) -> void
{
    constexpr static uint64_t hash{ hash_builder<Builder_T>() };

    m_builders.emplace_back(std::in_place_type<Builder_T>, std::forward<Args_T>(args)...);
    m_builder_hashes.push_back(hash_builder<Builder_T>());
    const DependencyDescriptor& dependency_descriptor
        = m_builder_dependencies.emplace_back(
            hash_builder<Builder_T>(),
            collect_all_missing_and_resolved_build_dependency_hashes<Builder_T>(
                &transient_memory_resource
            )
        );

    fix_order_of_builders(
        hash,
        dependency_descriptor.resolved_dependencies(),
        transient_memory_resource
    );
    mark_builder_as_resolved(hash);
}

template <typename Builder_T>
auto ContextBuildTree::register_injection(
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    resolve_injection_dependencies<Builder_T>(transient_memory_resource);
    emplace_injection<Builder_T>(transient_memory_resource);
}

template <typename Builder_T>
auto ContextBuildTree::resolve_injection_dependencies(
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    util::for_each(
        util::arguments_of_t<decltype(Builder_T::create)>{},
        [&]<typename Dependency_T> -> void
        {
            if constexpr (!represents_optional_dependency_c<Dependency_T>)
            {
                /*
                 * This can end up in an infinite recursion in case of a cyclic
                 * dependency. Cyclic dependencies must be checked before calling this
                 * function.
                 */
                register_context<
                    util::result_of_t<decltype(&strip_dependency_t<Dependency_T>::build)>>(
                    transient_memory_resource
                );
            }
        }
    );
}

template <typename... Dependencies_T>
[[nodiscard]]
auto ContextBuildTree::collect_creation_dependencies(
    util::TypeList<Dependencies_T...>,
    Injector& injector
) -> std::tuple<Dependencies_T...>
{
    return std::forward_as_tuple(
        [&injector] -> Dependencies_T
        {
            if constexpr (represents_optional_dependency_c<Dependencies_T>)
            {
                return injector.find_builder<strip_dependency_t<Dependencies_T>>();
            }
            else
            {
                return injector.builder_at<strip_dependency_t<Dependencies_T>>();
            }
        }()...
    );
}

template <typename Builder_T>
auto ContextBuildTree::emplace_injection(
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    constexpr static uint64_t hash{ hash_builder<Builder_T>() };

    m_injections.emplace_back(
        +[](Injector&                  injector,
            std::pmr::memory_resource& u_transient_memory_resource) -> void
        {
            injector.insert_builder(
                std::apply(
                    Builder_T::create,
                    collect_creation_dependencies(
                        util::arguments_of_t<decltype(Builder_T::create)>{},
                        injector
                    )
                ),
                u_transient_memory_resource
            );
        }
    );
    m_injection_hashes.push_back(hash);
    const DependencyDescriptor& dependency_descriptor
        = m_injection_dependencies.emplace_back(
            hash,
            collect_all_missing_injection_dependency_hashes<Builder_T>(
                &transient_memory_resource
            )
        );

    fix_order_of_injections(
        hash,
        dependency_descriptor.resolved_dependencies(),
        transient_memory_resource
    );
    mark_injection_as_resolved(hash);
}

template <typename Builder_T>
auto ContextBuildTree::collect_all_missing_injection_dependency_hashes(
    const std::pmr::polymorphic_allocator<uint64_t>& allocator
) -> std::pair<std::pmr::deque<uint64_t>, uint32_t>
{
    std::pair<std::pmr::deque<uint64_t>, uint32_t> result{ allocator, 0 };

    util::for_each(
        util::arguments_of_t<decltype(Builder_T::create)>{},
        [&]<typename Dependency_T>(this auto x_self) -> void
        {
            using StrippedDependency = strip_dependency_t<Dependency_T>;

            constexpr static uint64_t dependency_hash{
                hash_builder<StrippedDependency>()
            };

            if (const auto iter{ std::ranges::lower_bound(result.first, dependency_hash) };
                (iter == result.first.cend() || *iter != dependency_hash)
                && !std::ranges::contains(m_injection_hashes, dependency_hash))
            {
                result.first.insert(iter, dependency_hash);
            }

            if constexpr (requires { StrippedDependency::create; })
            {
                util::for_each(
                    util::arguments_of_t<decltype(StrippedDependency::create)>{},
                    x_self
                );
            }
        }
    );

    result.second = static_cast<uint32_t>(result.first.size());

    return result;
}

template <typename Builder_T>
auto ContextBuildTree::collect_all_resolved_injection_dependency_hashes(
    const std::pmr::polymorphic_allocator<uint64_t>& allocator
) -> std::pmr::deque<uint64_t>
{
    std::pmr::deque<uint64_t> result{
        std::initializer_list{ hash_builder<Builder_T>() },
        allocator,
    };

    util::for_each(
        util::arguments_of_t<decltype(Builder_T::create)>{},
        [&]<typename Dependency_T>(this auto x_self) -> void
        {
            using StrippedDependency = strip_dependency_t<Dependency_T>;

            constexpr static uint64_t dependency_hash{
                hash_builder<StrippedDependency>()
            };

            if (const auto iter{ std::ranges::lower_bound(result, dependency_hash) };
                (iter == result.cend() || *iter != dependency_hash)
                && std::ranges::contains(m_injection_hashes, dependency_hash))
            {
                result.insert(iter, dependency_hash);
            }

            if constexpr (requires { StrippedDependency::create; })
            {
                util::for_each(
                    util::arguments_of_t<decltype(StrippedDependency::create)>{},
                    x_self
                );
            }
        }
    );

    return result;
}

}   // namespace kiln::app
