module;

#include <concepts>
#include <format>
#include <memory_resource>
#include <type_traits>
#include <utility>

#include "kiln/util/contract_macros.hpp"

export module kiln.app.registry.EntryBuildDirector;

import kiln.app.registry.BuildableEntryBase;
import kiln.app.registry.ConfigurationEntry;
import kiln.app.registry.entry_builder_c;
import kiln.app.registry.EntryBase;
import kiln.app.registry.EntryBuilderBase;
import kiln.app.registry.EntryBuilderContainer;
import kiln.app.registry.EntryInjectionContainer;
import kiln.app.registry.RepresentsEntryDependencyConcept;
import kiln.app.registry.Registry;
import kiln.util.concepts.function_reference;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.type_list_all_of;
import kiln.util.containers.OptionalRef;
import kiln.util.contracts;
import kiln.util.for_each;
import kiln.util.reflection;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.result_of;

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
        EntryInjectionContainer& injection_container,
        EntryBuilderContainer&   builder_container,
        Registry&                registry
    );

    template <builds_entry_c<Entry_T> Builder_T>
    auto use_builder() -> void;

    template <entry_maker_function_reference_c<Entry_T> auto func_T>
    auto use_function() -> void;

private:
    template <typename>
    friend class EntryBuildDirector;

    EntryInjectionContainer* m_injection_container{};
    EntryBuilderContainer*   m_builder_container{};
    Registry*                m_registry{};


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
};

}   // namespace kiln::app

namespace kiln::app {

template <typename T>
concept represents_optional_dependency_c
    = util::specialization_of_c<T, util::OptionalRef>;

template <typename Entry_T>
EntryBuildDirector<Entry_T>::EntryBuildDirector(
    EntryInjectionContainer& injection_container,
    EntryBuilderContainer&   builder_container,
    Registry&                registry
)
    : m_injection_container{ &injection_container },
      m_builder_container{ &builder_container },
      m_registry{ &registry }
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
template <entry_maker_function_reference_c<Entry_T> auto func_T>
auto EntryBuildDirector<Entry_T>::use_function() -> void
{
    use_builder<DummyBuilder<func_T>>();
}

template <typename Entry_T>
template <typename Builder_T>
auto EntryBuildDirector<Entry_T>::build_builder() -> void
{
    if constexpr (injectable_builder_c<Builder_T>)
    {
        if (const bool success = m_injection_container->try_insert(Builder_T::create);
            !success)
        {
            return;
        }
    }
    else
    {
        if (const bool success = m_builder_container->try_emplace<Builder_T>(); !success)
        {
            return;
        }
    }

    /*
     * Dependencies must be resolved after builder to avoid recursion
     *  in case of a cyclic dependency
     */
    resolve_dependencies<Builder_T>();
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

                    build_entry<StrippedDependency>();
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

                if constexpr (std::derived_from<StrippedDependency, EntryBuilderBase>)
                {
                    /*
                     * Use `build_entry` instead of `build_builder`
                     *  to restrict infinite template instantiation recursion
                     */
                    build_entry<util::result_of_t<decltype(&StrippedDependency::build)>>();
                }
                else if constexpr (std::derived_from<StrippedDependency, EntryBase>)
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
    if constexpr (std::derived_from<UEntry_T, internal::BuildableEntryBase>)
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
        };

        describe_build(build_director);
    }
    else
    {
        m_registry->try_emplace<UEntry_T>();
    }
}

}   // namespace kiln::app
