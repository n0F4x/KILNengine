module;

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <memory_resource>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "kiln/util/contract_macros.hpp"

export module kiln.app.registry.EntryBuilderContainer;

import kiln.app.registry.DependencyChainNode;
import kiln.app.registry.EntryBase;
import kiln.app.registry.EntryBuilderBase;
import kiln.app.registry.strip_dependency_t;
import kiln.app.registry.Registry;
import kiln.util.concepts.specialization_of;
import kiln.util.containers.OptionalRef;
import kiln.util.containers.MoveOnlyFunction;
import kiln.util.contracts;
import kiln.util.for_each;
import kiln.util.reflection;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;
import kiln.util.type_traits.result_of;
import kiln.util.TypeList;

namespace kiln::app {

export class EntryBuilderContainer;

using ErasedEntryBuilder
    = util::MoveOnlyFunction<auto(EntryBuilderContainer&, Registry&) &&->void, 0>;

export class EntryBuilderContainer {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    EntryBuilderContainer(EntryBuilderContainer&&, const allocator_type&);

    explicit EntryBuilderContainer() = default;
    explicit EntryBuilderContainer(const allocator_type&);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    template <typename Builder_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

    template <typename Builder_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::const_like_t<Builder_T, Self_T>>;

    template <typename Builder_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&) noexcept -> util::const_like_t<Builder_T, Self_T>&;


    template <typename Builder_T>
    auto insert(Builder_T&& builder) -> void;

    template <typename Builder_T, typename... Args_T>
        requires std::constructible_from<Builder_T, Args_T&&...>
    auto try_emplace(Args_T&&... args) -> bool;

    auto build(
        Registry&                  registry,
        std::pmr::memory_resource& transient_memory_resource
    ) && -> void;

private:
    std::pmr::vector<ErasedEntryBuilder>         m_builders;
    std::pmr::vector<uint64_t>                   m_entry_hashes;
    std::pmr::vector<std::pmr::vector<uint64_t>> m_builder_dependency_hashes;
    std::pmr::vector<std::optional<std::pmr::vector<uint64_t>>> m_dependent_builder_hashes;
    std::pmr::vector<std::pmr::vector<uint64_t>>                m_entry_dependency_hashes;
    std::pmr::vector<std::optional<std::pmr::vector<uint64_t>>> m_dependent_entry_hashes;
#ifdef KILN_DEBUG
    std::pmr::vector<std::pmr::string> m_entry_name;
#endif


    [[nodiscard]]
    auto try_index_of_builder(uint64_t hash) const noexcept -> std::optional<std::size_t>;

    auto sort() -> void;

    [[nodiscard]]
    auto check_cyclic_dependencies(
        std::pmr::memory_resource& transient_memory_resource
    ) const -> bool;

    auto sort_based_on_builder_dependencies() -> void;
    auto sort_based_on_entry_dependencies() -> void;

    auto collect_dependent_builder_hashes() -> void;
    auto collect_dependent_entry_hashes() -> void;

    auto push_down_builder_dependencies_of(std::size_t index) -> void;
    auto bubble_up_entry_dependencies_of(std::size_t index) -> void;

    [[nodiscard]]
    auto check_cyclic_dependencies(
        std::size_t                 builder_index,
        const DependencyChainNode&  dependency_chain,
        std::pmr::vector<uint64_t>& hash_cache,
        std::pmr::memory_resource&  transient_memory_resource
    ) const -> bool;
};

}   // namespace kiln::app

namespace kiln::app {

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
    else if constexpr (std::derived_from<StrippedDependency, EntryBase>)
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

template <typename Builder_T>
struct ErasedEntryBuilderLambda {
    Builder_T builder;

    auto operator()(EntryBuilderContainer& builders, Registry& registry) && -> void
    {
        [this,
         &builders,
         &registry]<typename... Dependencies_T>(util::TypeList<Dependencies_T...>) -> void
        {
            registry.insert(
                std::move(builder)
                    .build(fetch_dependency<Dependencies_T>(builders, registry)...)
            );
        }(util::arguments_of_t<decltype(&Builder_T::build)>{});
    }
};

template <typename Builder_T>
auto EntryBuilderContainer::contains() const noexcept -> bool
{
    return std::ranges::contains(
        m_entry_hashes,
        util::hash_u64<util::result_of_t<decltype(&Builder_T::build)>>()
    );
}

template <typename Builder_T, typename Self_T>
auto EntryBuilderContainer::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::const_like_t<Builder_T, Self_T>>
{
    const std::optional<std::size_t> builder_index{
        self.try_index_of_builder(
            util::hash_u64<util::result_of_t<decltype(&Builder_T::build)>>()
        ),
    };
    if (!builder_index.has_value())
    {
        return std::nullopt;
    }

    return util::any_cast<ErasedEntryBuilderLambda<Builder_T>>(
               self.m_builders[*builder_index]
    )
        .builder;
}

template <typename Builder_T, typename Self_T>
auto EntryBuilderContainer::at(this Self_T& self) noexcept
    -> util::const_like_t<Builder_T, Self_T>&
{
    return *self.template find<Builder_T>();
}

template <typename Builder_T>
auto EntryBuilderContainer::insert(Builder_T&& builder) -> void
{
    [[maybe_unused]]
    const bool success
        = try_emplace<std::remove_cvref_t<Builder_T>>(std::forward<Builder_T>(builder));
    PRECOND(success);
}

template <typename Builder_T, typename... Args_T>
    requires std::constructible_from<Builder_T, Args_T&&...>
auto EntryBuilderContainer::try_emplace(Args_T&&... args) -> bool
{
    if (contains<Builder_T>())
    {
        return false;
    }

    m_builders.emplace_back(
        std::in_place_type<ErasedEntryBuilderLambda<Builder_T>>,
        std::forward<Args_T>(args)...
    );
    m_entry_hashes.push_back(
        util::hash_u64<util::result_of_t<decltype(&Builder_T::build)>>()
    );

    std::pmr::vector<uint64_t>& builder_dependency_hashes
        = m_builder_dependency_hashes.emplace_back();
    builder_dependency_hashes.reserve(
        util::arguments_of_t<decltype(&Builder_T::build)>::size()
    );
    m_dependent_builder_hashes.emplace_back();
    std::pmr::vector<uint64_t>& entry_dependency_hashes
        = m_entry_dependency_hashes.emplace_back();
    entry_dependency_hashes.reserve(
        util::arguments_of_t<decltype(&Builder_T::build)>::size()
    );
    m_dependent_entry_hashes.emplace_back();
    util::for_each(
        util::arguments_of_t<decltype(&Builder_T::build)>{},
        [&builder_dependency_hashes,
         &entry_dependency_hashes]<typename Dependency_T> -> void
        {
            using StrippedDependency = strip_dependency_t<Dependency_T>;
            if constexpr (std::derived_from<StrippedDependency, EntryBuilderBase>)
            {
                builder_dependency_hashes.push_back(
                    util::hash_u64<
                        util::result_of_t<decltype(&StrippedDependency::build)>>()
                );
            }
            else if constexpr (std::derived_from<StrippedDependency, EntryBase>)
            {
                entry_dependency_hashes.push_back(util::hash_u64<StrippedDependency>());
            }
            else
            {
                static_assert(false, "invalid build dependency");
            }
        }
    );

#ifdef KILN_DEBUG
    m_entry_name.emplace_back(
        util::name_of<util::result_of_t<decltype(&Builder_T::build)>>()
    );
#endif

    return true;
}

}   // namespace kiln::app
