module;

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <format>
#include <functional>
#include <memory_resource>
#include <vector>

#include "kiln/util/contract_macros.hpp"

export module kiln.app.registry.EntryBuildDirector;

import kiln.app.registry.BuildableEntryBase;
import kiln.app.registry.ConfigurationEntry;
import kiln.app.registry.CyclicDependencyDetected;
import kiln.app.registry.DependencyChainNode;
import kiln.app.registry.entry_builder_c;
import kiln.app.registry.entry_c;
import kiln.app.registry.EntryBase;
import kiln.app.registry.EntryBuilderBase;
import kiln.app.registry.EntryBuilderContainer;
import kiln.app.registry.EntryInjectionContainer;
import kiln.app.registry.ErasedEntryInjection;
import kiln.app.registry.RepresentsEntryDependencyConcept;
import kiln.app.registry.Registry;
import kiln.app.registry.strip_dependency_t;
import kiln.util.any_of;
import kiln.util.concepts.function_reference;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.type_list_all_of;
import kiln.util.containers.OptionalRef;
import kiln.util.contracts;
import kiln.util.for_each;
import kiln.util.reflection;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.result_of;
import kiln.util.TypeList;

namespace kiln::app {

template <typename T, typename Entry_T>
concept builds_entry_c
    = entry_builder_c<T> && std::same_as<util::result_of_t<decltype(&T::build)>, Entry_T>;

template <typename T, typename Entry_T>
concept entry_maker_function_reference_c
    = util::function_reference_c<T>
   && util::type_list_all_of_c<util::arguments_of_t<T>, RepresentsEntryDependencyConcept>
   && std::same_as<util::result_of_t<T>, Entry_T>;

export template <typename Entry_T>
class EntryBuildDirector {
public:
    explicit EntryBuildDirector(
        EntryInjectionContainer&   injection_container,
        EntryBuilderContainer&     builder_container,
        Registry&                  registry,
        bool                       check_dependencies,
        std::pmr::memory_resource& transient_memory_resource
    );
    explicit EntryBuildDirector(
        const DependencyChainNode&  dependency_chain,
        std::pmr::vector<uint64_t>& hash_cache,
        std::pmr::memory_resource&  transient_memory_resource
    );

    template <builds_entry_c<Entry_T> Builder_T>
    auto use_builder() -> void;

    template <entry_maker_function_reference_c<Entry_T> func_T>
    auto use_function() -> void;

private:
    template <typename>
    friend class EntryBuildDirector;

    EntryInjectionContainer* m_injection_container{};
    EntryBuilderContainer*   m_builder_container{};
    Registry*                m_registry{};
    bool                     m_check_dependencies;

    const DependencyChainNode*                        m_dependency_chain{};
    std::pmr::vector<uint64_t>*                       m_hash_cache{};
    std::reference_wrapper<std::pmr::memory_resource> m_transient_memory_resource;


    template <typename Builder_T>
    auto build_builder() -> void;

    template <typename Builder_T>
    auto resolve_dependencies() -> void;

    template <typename Builder_T>
    auto resolve_create_dependencies() -> void;
    template <typename Builder_T>
    auto resolve_build_dependencies() const -> void;

    template <typename UEntry_T>
    auto build_entry() const -> void;

    template <typename Builder_T>
    [[nodiscard]]
    auto check_for_cyclic_dependency() -> bool;

    template <typename Builder_T>
    [[nodiscard]]
    auto check_for_cyclic_dependency_via_builder() const -> bool;
    template <typename UEntry_T>
    [[nodiscard]]
    static auto check_for_cyclic_dependency_via_builder_through_entry(
        const DependencyChainNode&  dependency_chain,
        std::pmr::vector<uint64_t>& hash_cache,
        std::pmr::memory_resource&  transient_memory_resource
    ) -> bool;
    template <typename UEntry_T>
    [[nodiscard]]
    static auto check_for_cyclic_dependency_via_entry(
        const DependencyChainNode&  dependency_chain,
        std::pmr::vector<uint64_t>& hash_cache,
        std::pmr::memory_resource&  transient_memory_resource
    ) -> bool;

    template <typename Builder_T>
    [[nodiscard]]
    static auto check_for_cyclic_dependency_via_create(
        const DependencyChainNode&  dependency_chain,
        std::pmr::vector<uint64_t>& hash_cache,
        std::pmr::memory_resource&  transient_memory_resource
    ) -> bool;
    template <typename Builder_T>
    [[nodiscard]]
    static auto check_for_cyclic_dependency_via_build(
        const DependencyChainNode&  dependency_chain,
        std::pmr::vector<uint64_t>& hash_cache,
        std::pmr::memory_resource&  transient_memory_resource
    ) -> bool;
};

}   // namespace kiln::app

namespace kiln::app {

template <typename T>
concept represents_optional_dependency_c
    = util::specialization_of_c<T, util::OptionalRef>;

template <typename Entry_T>
concept buildable_entry_c = std::derived_from<Entry_T, internal::BuildableEntryBase>;

template <typename Entry_T>
EntryBuildDirector<Entry_T>::EntryBuildDirector(
    EntryInjectionContainer&   injection_container,
    EntryBuilderContainer&     builder_container,
    Registry&                  registry,
    const bool                 check_dependencies,
    std::pmr::memory_resource& transient_memory_resource
)
    : m_injection_container{ &injection_container },
      m_builder_container{ &builder_container },
      m_registry{ &registry },
      m_check_dependencies{ check_dependencies },
      m_transient_memory_resource{ transient_memory_resource }
{
}

template <typename Entry_T>
EntryBuildDirector<Entry_T>::EntryBuildDirector(
    const DependencyChainNode&  dependency_chain,
    std::pmr::vector<uint64_t>& hash_cache,
    std::pmr::memory_resource&  transient_memory_resource
)
    : m_check_dependencies{ true },
      m_dependency_chain{ &dependency_chain },
      m_hash_cache{ &hash_cache },
      m_transient_memory_resource{ transient_memory_resource }
{
}

template <typename Builder_T>
concept injectable_builder_c = requires {
    requires std::same_as<util::result_of_t<decltype(Builder_T::create)>, Builder_T>;
};

template <typename Entry_T>
template <builds_entry_c<Entry_T> Builder_T>
auto EntryBuildDirector<Entry_T>::use_builder() -> void
{
    if (m_dependency_chain == nullptr)
    {
        PRECOND(
            m_injection_container != nullptr
                && m_builder_container != nullptr
                && m_registry != nullptr,
            std::format("{} can only be used once", util::name_of<EntryBuildDirector>())
        );

        build_builder<Builder_T>();

        m_injection_container = nullptr;
        m_builder_container   = nullptr;
        m_registry            = nullptr;
    }
    else
    {
        PRECOND(!check_for_cyclic_dependency_via_builder<Builder_T>());
    }
}

template <auto func_T>
struct DummyBuilder;

template <
    typename Entry_T,
    typename... Dependencies_T,
    auto (&func_T)(Dependencies_T...)->Entry_T>
struct DummyBuilder<func_T> {
    [[nodiscard]]
    static auto build(Dependencies_T... dependencies) -> Entry_T
    {
        return func_T(std::forward<Dependencies_T>(dependencies)...);
    }
};

template <typename Entry_T>
template <entry_maker_function_reference_c<Entry_T> func_T>
auto EntryBuildDirector<Entry_T>::use_function() -> void
{
    use_builder<func_T>();
}

template <typename Entry_T>
template <typename Builder_T>
auto EntryBuildDirector<Entry_T>::build_builder() -> void
{
    if constexpr (injectable_builder_c<Builder_T>)
    {
        if (m_injection_container->contains<decltype(Builder_T::create)>())
        {
            return;
        }

        PRECOND(!check_for_cyclic_dependency<Builder_T>());

        resolve_dependencies<Builder_T>();

        m_injection_container->push_back(Builder_T::create);
    }
    else
    {
        if (m_builder_container->contains<Builder_T>())
        {
            return;
        }

        PRECOND(!check_for_cyclic_dependency<Builder_T>());

        resolve_dependencies<Builder_T>();

        m_builder_container->emplace_back<Builder_T>();
    }
}

template <typename Entry_T>
template <typename Builder_T>
auto EntryBuildDirector<Entry_T>::resolve_dependencies() -> void
{
    if constexpr (injectable_builder_c<Builder_T>)
    {
        resolve_create_dependencies<Builder_T>();
    }
    resolve_build_dependencies<Builder_T>();
}

template <typename Entry_T>
template <typename Builder_T>
auto EntryBuildDirector<Entry_T>::resolve_create_dependencies() -> void
{
    PRECOND(m_injection_container != nullptr);
    PRECOND(m_builder_container != nullptr);
    PRECOND(m_registry != nullptr);

    util::for_each(
        util::arguments_of_t<decltype(Builder_T::create)>{},
        [&]<typename Dependency_T> -> void
        {
            if constexpr (!represents_optional_dependency_c<Dependency_T>)
            {
                using StrippedDependency = std::remove_cvref_t<Dependency_T>;

                if constexpr (std::derived_from<StrippedDependency, EntryBuilderBase>)
                {
                    /*
                     * Use `build_entry` instead of `build_builder`
                     *  to restrict infinite template instantiation recursion
                     */
                    build_entry<util::result_of_t<decltype(&StrippedDependency::build)>>();
                }
                else
                {
                    static_assert(
                        std::derived_from<StrippedDependency, ConfigurationEntry>
                    );
                }
            }
        }
    );
}

template <typename Entry_T>
template <typename Builder_T>
auto EntryBuildDirector<Entry_T>::resolve_build_dependencies() const -> void
{
    PRECOND(m_injection_container != nullptr);
    PRECOND(m_builder_container != nullptr);
    PRECOND(m_registry != nullptr);

    util::for_each(
        util::arguments_of_t<decltype(&Builder_T::build)>{},
        [&]<typename Dependency_T> -> void
        {
            if constexpr (!represents_optional_dependency_c<Dependency_T>)
            {
                using StrippedDependency = std::remove_cvref_t<Dependency_T>;

                // TODO: resolve builder dependencies as well
                if constexpr (std::derived_from<StrippedDependency, EntryBase>)
                {
                    build_entry<StrippedDependency>();
                }
            }
        }
    );
}

template <typename Entry_T>
template <typename UEntry_T>
auto EntryBuildDirector<Entry_T>::build_entry() const -> void
{
    if constexpr (buildable_entry_c<UEntry_T>)
    {
        /*
         * This can end up in an infinite recursion in case of a cyclic
         * dependency. Cyclic dependencies must be checked before calling this
         * function.
         */

        EntryBuildDirector<UEntry_T> build_director{
            *m_injection_container,
            *m_builder_container,
            *m_registry,
            false,   //
            m_transient_memory_resource,
        };

        describe_build(std::type_identity<UEntry_T>{}, build_director);
    }
    else
    {
        m_registry->try_emplace<UEntry_T>();
    }
}

template <typename Entry_T>
template <typename Builder_T>
auto EntryBuildDirector<Entry_T>::check_for_cyclic_dependency() -> bool
{
    if (!m_check_dependencies)
    {
        return false;
    }
    m_check_dependencies = false;

    constexpr static uint64_t hash{ util::hash_u64<Builder_T>() };

    std::pmr::vector<uint64_t> hash_cache{ &m_transient_memory_resource.get() };
    hash_cache.push_back(hash);

    const DependencyChainNode dependency_chain_node{
        .hash{ hash },
        .name{ util::name_of<Builder_T>() },
    };

    if constexpr (injectable_builder_c<Builder_T>)
    {
        if (check_for_cyclic_dependency_via_create<Builder_T>(
                dependency_chain_node,
                hash_cache,
                m_transient_memory_resource
            ))
        {
            return true;
        }
    }
    return check_for_cyclic_dependency_via_build<Builder_T>(
        dependency_chain_node,
        hash_cache,
        m_transient_memory_resource
    );
}

template <typename Entry_T>
template <typename Builder_T>
auto EntryBuildDirector<Entry_T>::check_for_cyclic_dependency_via_builder() const -> bool
{
    PRECOND(m_dependency_chain != nullptr);
    PRECOND(m_hash_cache != nullptr);

    constexpr static uint64_t         hash{ util::hash_u64<Builder_T>() };
    constexpr static std::string_view name{ util::name_of<Builder_T>() };

    if (m_dependency_chain->contains(hash))
    {
        PRECOND_AS(
            false,
            CyclicDependencyDetected,
            std::format(
                "Cyclic dependency detected"   //
                " - entry builder of type `{}` depends on itself ({} -> {})",
                name,
                m_dependency_chain->format(&m_transient_memory_resource.get()),
                name
            )
        );
        return true;
    }

    if (std::ranges::contains(*m_hash_cache, hash))
    {
        return false;
    }
    m_hash_cache->insert(std::ranges::upper_bound(*m_hash_cache, hash), hash);

    const DependencyChainNode dependency_chain_node{
        .previous = m_dependency_chain,
        .hash     = hash,
        .name     = name,
    };

    if constexpr (injectable_builder_c<Builder_T>)
    {
        if (check_for_cyclic_dependency_via_create<Builder_T>(
                dependency_chain_node,
                *m_hash_cache,
                m_transient_memory_resource
            ))
        {
            return true;
        }
    }
    return check_for_cyclic_dependency_via_build<Builder_T>(
        dependency_chain_node,
        *m_hash_cache,
        m_transient_memory_resource
    );
}

template <typename Entry_T>
template <typename UEntry_T>
auto EntryBuildDirector<Entry_T>::check_for_cyclic_dependency_via_builder_through_entry(
    const DependencyChainNode&  dependency_chain,
    std::pmr::vector<uint64_t>& hash_cache,
    std::pmr::memory_resource&  transient_memory_resource
) -> bool
{
    static_assert(buildable_entry_c<UEntry_T>);

    EntryBuildDirector<UEntry_T> build_director{
        dependency_chain,
        hash_cache,
        transient_memory_resource,
    };

    describe_build(std::type_identity<UEntry_T>{}, build_director);

    return false;
}

template <typename Entry_T>
template <typename UEntry_T>
auto EntryBuildDirector<Entry_T>::check_for_cyclic_dependency_via_entry(
    const DependencyChainNode&  dependency_chain,
    std::pmr::vector<uint64_t>& hash_cache,
    std::pmr::memory_resource&  transient_memory_resource
) -> bool
{
    constexpr static uint64_t         hash{ util::hash_u64<UEntry_T>() };
    constexpr static std::string_view name{ util::name_of<UEntry_T>() };

    if (dependency_chain.contains(hash))
    {
        PRECOND_AS(
            false,
            CyclicDependencyDetected,
            std::format(
                "Cyclic dependency detected - entry of type `{}` depends on itself "   //
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

    if constexpr (buildable_entry_c<UEntry_T>)
    {
        const DependencyChainNode dependency_chain_node{
            .previous = &dependency_chain,
            .hash     = hash,
            .name     = name,
        };

        EntryBuildDirector<UEntry_T> build_director{
            dependency_chain_node,
            hash_cache,
            transient_memory_resource,
        };

        describe_build(std::type_identity<UEntry_T>{}, build_director);
    }

    return false;
}

template <typename Entry_T>
template <typename Builder_T>
auto EntryBuildDirector<Entry_T>::check_for_cyclic_dependency_via_create(
    const DependencyChainNode&  dependency_chain,
    std::pmr::vector<uint64_t>& hash_cache,
    std::pmr::memory_resource&  transient_memory_resource
) -> bool
{
    return util::any_of(
        util::arguments_of_t<decltype(Builder_T::create)>{},
        [&]<typename Dependency_T> -> bool
        {
            using StrippedDependency = strip_dependency_t<Dependency_T>;

            if constexpr (std::derived_from<StrippedDependency, EntryBuilderBase>)
            {
                /*
                 * Hop through entry instead of going directly to builder
                 *  to restrict infinite template instantiation recursion
                 */
                return check_for_cyclic_dependency_via_builder_through_entry<
                    util::result_of_t<decltype(&StrippedDependency::build)>>(
                    dependency_chain,
                    hash_cache,
                    transient_memory_resource
                );
            }
            else
            {
                static_assert(
                    std::derived_from<StrippedDependency, ConfigurationEntry>,
                    "invalid dependency"
                );
                return false;
            }
        }
    );
}

template <typename Entry_T>
template <typename Builder_T>
auto EntryBuildDirector<Entry_T>::check_for_cyclic_dependency_via_build(
    const DependencyChainNode&  dependency_chain,
    std::pmr::vector<uint64_t>& hash_cache,
    std::pmr::memory_resource&  transient_memory_resource
) -> bool
{
    return util::any_of(
        util::arguments_of_t<decltype(&Builder_T::build)>{},
        [&]<typename Dependency_T> -> bool
        {
            using StrippedDependency = strip_dependency_t<Dependency_T>;

            if constexpr (std::derived_from<StrippedDependency, EntryBuilderBase>)
            {
                // TODO
                return false;
            }
            else
            {
                static_assert(std::derived_from<StrippedDependency, EntryBase>);

                return check_for_cyclic_dependency_via_entry<StrippedDependency>(
                    dependency_chain,
                    hash_cache,
                    transient_memory_resource
                );
            }
        }
    );
}

}   // namespace kiln::app
