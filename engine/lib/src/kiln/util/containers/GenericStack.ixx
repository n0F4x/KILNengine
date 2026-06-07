module;

#include <algorithm>
#include <cstdint>
#include <deque>
#include <format>
#include <memory_resource>
#include <tuple>
#include <type_traits>
#include <utility>

#include "kiln/util/contract_macros.hpp"

export module kiln.util.containers.GenericStack;

import kiln.util.concepts.type_list_all_of;
import kiln.util.containers.MoveOnlyAny;
import kiln.util.containers.OptionalRef;
import kiln.util.contracts;
import kiln.util.reflection;
import kiln.util.transform;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;
import kiln.util.type_traits.result_of;
import kiln.util.type_traits.type_list_to;

namespace kiln::util {

export template <typename T, typename Any_T>
concept basic_generic_stack_item_c = Any_T::template storable<T>();

export template <typename T, typename Any_T>
concept decays_to_basic_generic_stack_item_c
    = basic_generic_stack_item_c<std::decay_t<T>, Any_T>;

namespace internal {

template <typename Any_T>
struct IsBasicGenericStackItemDependency {
    template <typename T>
    using type
        = std::bool_constant<basic_generic_stack_item_c<std::remove_cvref_t<T>, Any_T>>;
};

}   // namespace internal

export template <typename T, typename Any_T>
concept basic_generic_stack_item_injection_c
    = basic_generic_stack_item_c<result_of_t<T&&>, Any_T>
   && type_list_all_of_c<
          arguments_of_t<T&&>,
          internal::IsBasicGenericStackItemDependency<Any_T>::template type>;

export template <typename T, typename Any_T>
concept decays_to_basic_generic_stack_item_injection_c
    = basic_generic_stack_item_injection_c<std::decay_t<T>, Any_T>;

/*
 * References to contained items are valid until the instance is alive.
 * Destroying a "moved-from" instance does not invalidate any references to items.
 */
export template <move_only_any_c Any_T = BasicMoveOnlyAny<0>>
    requires(Any_T::size() == 0)
class BasicGenericStack {
public:
    using Any = Any_T;

    // required for interfacing with the standard
    using allocator_type = std::pmr::polymorphic_allocator<>;


    BasicGenericStack() = default;
    explicit BasicGenericStack(const allocator_type& allocator);
    BasicGenericStack(const BasicGenericStack&)     = delete;
    BasicGenericStack(BasicGenericStack&&) noexcept = default;
    BasicGenericStack(BasicGenericStack&&, const allocator_type& allocator);
    ~BasicGenericStack();

    auto operator=(const BasicGenericStack&) -> BasicGenericStack&     = delete;
    auto operator=(BasicGenericStack&&) noexcept -> BasicGenericStack& = default;


    // required for interfacing with the standard
    [[nodiscard]]
    auto get_allocator() const -> allocator_type;


    [[nodiscard]]
    auto empty() const -> bool;

    template <basic_generic_stack_item_c<Any_T> Item_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

    template <basic_generic_stack_item_c<Any_T> Item_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept -> OptionalRef<const_like_t<Item_T, Self_T>>;

    template <basic_generic_stack_item_c<Any_T> Item_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&) -> const_like_t<Item_T, Self_T>&;


    template <decays_to_basic_generic_stack_item_c<Any_T> Item_T>
    auto insert(Item_T&& item) -> Item_T&;
    template <basic_generic_stack_item_c<Any_T> Item_T, typename... Args_T>
        requires(std::is_constructible_v<Item_T, Args_T && ...>)
    auto emplace(Args_T&&... args) -> Item_T&;
    template <basic_generic_stack_item_c<Any_T> Item_T, typename... Args_T>
        requires(std::is_constructible_v<Item_T, Args_T && ...>)
    auto try_emplace(Args_T&&... args) -> std::pair<Item_T&, bool>;

private:
    // TODO: Use different containers for types and items
    std::pmr::deque<std::pair<uint64_t, Any>> m_types_and_items;

    template <typename Injection_T>
    [[nodiscard]]
    auto resolve_dependencies()
        -> type_list_to_t<arguments_of_t<Injection_T>, std::tuple>;
};

export using GenericStack = BasicGenericStack<>;

export template <typename T>
concept generic_stack_item_c = basic_generic_stack_item_c<T, GenericStack::Any>;

export template <typename T>
concept decays_to_generic_stack_item_c = generic_stack_item_c<std::decay_t<T>>;

export template <typename T>
concept generic_stack_item_injection_c
    = basic_generic_stack_item_injection_c<T, GenericStack::Any>;

export template <typename T>
concept decays_to_generic_stack_item_injection_c
    = generic_stack_item_injection_c<std::decay_t<T>>;

}   // namespace kiln::util

namespace kiln::util {

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
BasicGenericStack<Any_T>::BasicGenericStack(const allocator_type& allocator)
    : m_types_and_items{ allocator }
{
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
BasicGenericStack<Any_T>::BasicGenericStack(
    BasicGenericStack&&   other,
    const allocator_type& allocator
)
    : m_types_and_items{ std::move(other.m_types_and_items), allocator }
{
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
BasicGenericStack<Any_T>::~BasicGenericStack()
{
    while (!m_types_and_items.empty())
    {
        m_types_and_items.pop_back();
    }
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
auto BasicGenericStack<Any_T>::get_allocator() const -> allocator_type
{
    return m_types_and_items.get_allocator();
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
auto BasicGenericStack<Any_T>::empty() const -> bool
{
    return m_types_and_items.empty();
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <basic_generic_stack_item_c<Any_T> Item_T>
auto BasicGenericStack<Any_T>::contains() const noexcept -> bool
{
    return std::ranges::contains(
        m_types_and_items,
        hash_u64<Item_T>(),
        &std::pair<uint64_t, Any>::first
    );
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <basic_generic_stack_item_c<Any_T> Item_T, typename Self_T>
auto BasicGenericStack<Any_T>::find(this Self_T& self) noexcept
    -> OptionalRef<const_like_t<Item_T, Self_T>>
{
    const auto iter = std::ranges::find(
        self.BasicGenericStack::m_types_and_items,
        hash_u64<Item_T>(),
        &std::pair<uint64_t, Any>::first
    );
    if (iter == self.BasicGenericStack::m_types_and_items.cend())
    {
        return std::nullopt;
    }

    return OptionalRef<const_like_t<Item_T, Self_T>>{ any_cast<Item_T>(iter->second) };
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <basic_generic_stack_item_c<Any_T> Item_T, typename Self_T>
auto BasicGenericStack<Any_T>::at(this Self_T& self) -> const_like_t<Item_T, Self_T>&
{
    const OptionalRef found_item{ self.BasicGenericStack::template find<Item_T>() };

    PRECOND(
        found_item.has_value(),
        std::format("Item of type `{}` not found", name_of<Item_T>())
    );

    return *found_item;
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <decays_to_basic_generic_stack_item_c<Any_T> Item_T>
auto BasicGenericStack<Any_T>::insert(Item_T&& item) -> Item_T&
{
    return emplace<std::decay_t<Item_T>>(std::forward<Item_T>(item));
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <basic_generic_stack_item_c<Any_T> Item_T, typename... Args_T>
    requires(std::is_constructible_v<Item_T, Args_T && ...>)
auto BasicGenericStack<Any_T>::emplace(Args_T&&... args) -> Item_T&
{
    auto&& [item, success]{ try_emplace<Item_T>(std::forward<Args_T>(args)...) };
    PRECOND(success);
    return item;
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <basic_generic_stack_item_c<Any_T> Item_T, typename... Args_T>
    requires(std::is_constructible_v<Item_T, Args_T && ...>)
auto BasicGenericStack<Any_T>::try_emplace(Args_T&&... args) -> std::pair<Item_T&, bool>
{
    if (const OptionalRef<Item_T> found{ find<Item_T>() }; found.has_value())
    {
        return std::pair<Item_T&, bool>{ *found, false };
    }

    return std::pair<Item_T&, bool>{
        any_cast<Item_T>(
            m_types_and_items
                .emplace_back(
                    std::piecewise_construct,
                    std::tuple{ hash_u64<Item_T>() },
                    std::forward_as_tuple(
                        std::in_place_type<Item_T>,
                        std::forward<Args_T>(args)...
                    )
                )
                .second   //
        ),
        true,
    };
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <typename Injection_T>
auto BasicGenericStack<Any_T>::resolve_dependencies()
    -> type_list_to_t<arguments_of_t<Injection_T>, std::tuple>
{
    return transform(
        arguments_of_t<Injection_T>{},
        [this]<typename Dependency_T> -> Dependency_T
        {
            std::ignore = this;   // supress buggy variable unused warning by Clang

            using Dependency = std::remove_cvref_t<Dependency_T>;

            PRECOND(
                contains<Dependency>(),
                std::format(
                    "Item dependency of type `{}` not found",
                    name_of<Dependency>()
                )
            );

            return at<Dependency>();
        }
    );
}

}   // namespace kiln::util
