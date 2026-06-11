module;

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <functional>
#include <memory_resource>
#include <optional>
#include <utility>
#include <vector>

#include "kiln/util/contract_macros.hpp"

export module kiln.app.registry.EntryBuilderContainer;

import kiln.app.registry.EntryBase;
import kiln.app.registry.ErasedEntryBuilder;
import kiln.app.registry.strip_dependency_t;
import kiln.util.containers.OptionalRef;
import kiln.util.containers.Any;
import kiln.util.contracts;
import kiln.util.for_each;
import kiln.util.reflection;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;
import kiln.util.type_traits.result_of;

namespace kiln::app {

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
    auto emplace_back(Args_T&&... args) -> void;

    auto sort(std::pmr::memory_resource& transient_memory_resource) -> void;

    template <
        typename Self_T,
        std::invocable<util::forward_like_t<ErasedEntryBuilder, Self_T>> F>
    auto for_each(this Self_T&&, F&& func) -> F;

private:
    std::pmr::vector<ErasedEntryBuilder>         m_builders;
    std::pmr::vector<uint64_t>                   m_entry_hashes;
    std::pmr::vector<std::pmr::vector<uint64_t>> m_dependency_entry_hashes;


    auto bubble_up_dependencies_of(
        uint64_t                   hash,
        std::pmr::memory_resource& transient_memory_resource
    ) -> void;

    auto try_index_of_builder(uint64_t hash) -> std::optional<std::size_t>;
    auto collect_dependencies_of(uint64_t hash, std::pmr::vector<uint64_t>& result)
        -> void;
};

}   // namespace kiln::app

namespace kiln::app {

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
    emplace_back<std::remove_cvref_t<Builder_T>>(std::forward<Builder_T>(builder));
}

template <typename Builder_T, typename... Args_T>
    requires std::constructible_from<Builder_T, Args_T&&...>
auto EntryBuilderContainer::emplace_back(Args_T&&... args) -> void
{
    PRECOND(!contains<Builder_T>());

    m_builders.emplace_back(
        make_erased_entry_builder_lambda<Builder_T>(std::forward<Args_T>(args)...)
    );
    m_entry_hashes.push_back(
        util::hash_u64<util::result_of_t<decltype(&Builder_T::build)>>()
    );

    std::pmr::vector<uint64_t>& dependency_entry_hashes
        = m_dependency_entry_hashes.emplace_back();
    dependency_entry_hashes.reserve(
        util::arguments_of_t<decltype(&Builder_T::build)>::size()
    );
    util::for_each(
        util::arguments_of_t<decltype(&Builder_T::build)>{},
        [&dependency_entry_hashes]<typename Dependency_T> -> void
        {
            using StrippedDependency = strip_dependency_t<Dependency_T>;
            static_assert(std::derived_from<StrippedDependency, EntryBase>);
            dependency_entry_hashes.push_back(util::hash_u64<StrippedDependency>());
        }
    );
}

template <typename Self_T, std::invocable<util::forward_like_t<ErasedEntryBuilder, Self_T>> F>
auto EntryBuilderContainer::for_each(this Self_T&& self, F&& func) -> F
{
    for (auto&& builder : self.m_builders)
    {
        std::invoke(func, std::forward_like<Self_T>(builder));
    }

    return std::forward<F>(func);
}

}   // namespace kiln::app
