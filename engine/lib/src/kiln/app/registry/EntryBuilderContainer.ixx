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

export using ErasedEntryBuilder
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
    auto emplace_back(Args_T&&... args) -> void;

    auto sort(std::pmr::memory_resource& transient_memory_resource) -> void;

    template <
        typename Self_T,
        std::invocable<util::forward_like_t<ErasedEntryBuilder, Self_T>> F>
    auto for_each(this Self_T&&, F&& func) -> F;

private:
    std::pmr::vector<ErasedEntryBuilder>         m_builders;
    std::pmr::vector<uint64_t>                   m_entry_hashes;
    std::pmr::vector<std::pmr::vector<uint64_t>> m_builder_dependency_hashes;
    std::pmr::vector<std::pmr::vector<uint64_t>> m_entry_dependency_hashes;


    [[nodiscard]]
    auto try_index_of_builder(uint64_t hash) const noexcept -> std::optional<std::size_t>;

    auto bubble_up_entry_dependencies_of(
        uint64_t                   hash,
        std::pmr::memory_resource& transient_memory_resource
    ) -> void;
    auto push_down_builder_dependencies_of(
        uint64_t                   hash,
        std::pmr::memory_resource& transient_memory_resource
    ) -> void;

    auto collect_entry_dependencies_of(uint64_t hash, std::pmr::vector<uint64_t>& result
    ) const -> void;
    auto collect_builder_dependencies_of(uint64_t hash, std::pmr::vector<uint64_t>& result
    ) const -> void;
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

template <typename Builder_T, typename... Args_T>
    requires std::constructible_from<Builder_T, Args_T&&...>
[[nodiscard]]
auto make_erased_entry_builder_lambda(Args_T&&... args)
    -> ErasedEntryBuilderLambda<Builder_T>
{
    return ErasedEntryBuilderLambda<Builder_T>{
        .builder{ std::forward<Args_T>(args)... },
    };
}

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

    std::pmr::vector<uint64_t>& builder_dependency_hashes
        = m_builder_dependency_hashes.emplace_back();
    builder_dependency_hashes.reserve(
        util::arguments_of_t<decltype(&Builder_T::build)>::size()
    );
    std::pmr::vector<uint64_t>& entry_dependency_hashes
        = m_entry_dependency_hashes.emplace_back();
    entry_dependency_hashes.reserve(
        util::arguments_of_t<decltype(&Builder_T::build)>::size()
    );
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
