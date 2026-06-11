module;

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <functional>
#include <memory_resource>
#include <utility>
#include <vector>

#include "kiln/util/contract_macros.hpp"

export module kiln.app.registry.EntryInjectionContainer;

import kiln.app.registry.ConfigurationEntry;
import kiln.app.registry.EntryBuilderBase;
import kiln.app.registry.ErasedEntryInjection;
import kiln.app.registry.strip_dependency_t;
import kiln.util.concepts.function;
import kiln.util.concepts.function_pointer;
import kiln.util.contracts;
import kiln.util.reflection;
import kiln.util.type_traits.forward_like;
import kiln.util.type_traits.result_of;

namespace kiln::app {

export class EntryInjectionContainer {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    EntryInjectionContainer(EntryInjectionContainer&&, const allocator_type&);

    explicit EntryInjectionContainer() = default;
    explicit EntryInjectionContainer(const allocator_type&);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    template <util::function_c Injection_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;


    template <typename Builder_T, typename... Dependencies_T>
    auto push_back(auto (*create)(Dependencies_T...)->Builder_T) -> void;

    auto sort(std::pmr::memory_resource& transient_memory_resource) -> void;

    template <
        typename Self_T,
        std::invocable<util::forward_like_t<ErasedEntryInjection, Self_T>> F>
    auto for_each(this Self_T&&, F&& func) -> F;

private:
    std::pmr::vector<ErasedEntryInjection>       m_injections;
    std::pmr::vector<uint64_t>                   m_entry_hashes;
    std::pmr::vector<std::pmr::vector<uint64_t>> m_dependency_entry_hashes;


    auto bubble_up_dependencies_of(
        uint64_t                   hash,
        std::pmr::memory_resource& transient_memory_resource
    ) -> void;
    auto collect_dependencies_of(uint64_t hash, std::pmr::vector<uint64_t>& result)
        -> void;
};

}   // namespace kiln::app

namespace kiln::app {

template <util::function_c Injection_T>
auto EntryInjectionContainer::contains() const noexcept -> bool
{
    return std::ranges::contains(
        m_entry_hashes,
        util::hash_u64<
            util::result_of_t<decltype(&util::result_of_t<Injection_T>::build)>>()
    );
}

template <typename Dependency_T>
// ReSharper disable once CppNotAllPathsReturnValue
consteval auto hash_dependency() -> uint64_t
{
    using StrippedDependency = strip_dependency_t<Dependency_T>;

    if constexpr (std::derived_from<StrippedDependency, EntryBuilderBase>)
    {
        return util::hash_u64<util::result_of_t<decltype(&StrippedDependency::build)>>();
    }
    else if constexpr (std::derived_from<StrippedDependency, ConfigurationEntry>)
    {
        return util::hash_u64<StrippedDependency>();
    }
    else
    {
        static_assert(false, "invalid dependency");
    }
}

template <typename Builder_T, typename... Dependencies_T>
auto EntryInjectionContainer::push_back(auto (*create)(Dependencies_T...)->Builder_T)
    -> void
{
    PRECOND(!contains<std::remove_pointer_t<decltype(create)>>());

    m_injections.emplace_back(make_erased_entry_injection<Builder_T>());
    m_entry_hashes.push_back(
        util::hash_u64<util::result_of_t<
            decltype(&util::result_of_t<std::remove_pointer_t<decltype(create)>>::build)>>()
    );
    m_dependency_entry_hashes.emplace_back(
        std::initializer_list{ hash_dependency<Dependencies_T>()... }
    );
}

template <
    typename Self_T,
    std::invocable<util::forward_like_t<ErasedEntryInjection, Self_T>> F>
auto EntryInjectionContainer::for_each(this Self_T&& self, F&& func) -> F
{
    for (auto&& injection : self.m_injections)
    {
        std::invoke(func, std::forward_like<Self_T>(injection));
    }

    return std::forward<F>(func);
}

}   // namespace kiln::app
