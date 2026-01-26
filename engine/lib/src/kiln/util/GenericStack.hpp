#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <format>
#include <functional>
#include <tuple>
#include <type_traits>
#include <vector>

#include "kiln/util/Any.hpp"
#include "kiln/util/concepts/type_list_all_of.hpp"
#include "kiln/util/contract_macros.hpp"
#include "kiln/util/OptionalRef.hpp"
#include "kiln/util/reflection.hpp"
#include "kiln/util/type_traits/arguments_of.hpp"
#include "kiln/util/type_traits/const_like.hpp"
#include "kiln/util/type_traits/forward_like.hpp"
#include "kiln/util/type_traits/result_of.hpp"
#include "kiln/util/type_traits/type_list_to.hpp"

namespace kiln::util {

template <typename T, typename Any_T>
concept basic_generic_stack_item_c = Any_T::template storable<T>();

template <typename T, typename Any_T>
concept decays_to_basic_generic_stack_item_c =
    basic_generic_stack_item_c<std::decay_t<T>, Any_T>;

namespace internal {

template <typename Any_T>
struct IsBasicGenericStackItemDependency {
    template <typename T>
    using type =
        std::bool_constant<basic_generic_stack_item_c<std::remove_cvref_t<T>, Any_T>>;
};

}   // namespace internal

template <typename T, typename Any_T>
concept basic_generic_stack_item_injection_c =
    basic_generic_stack_item_c<result_of_t<T&&>, Any_T>
    && type_list_all_of_c<
        arguments_of_t<T&&>,
        internal::IsBasicGenericStackItemDependency<Any_T>::template type>;

template <typename T, typename Any_T>
concept decays_to_basic_generic_stack_item_injection_c =
    basic_generic_stack_item_injection_c<std::decay_t<T>, Any_T>;

/*
 * References to contained items are valid until the instance is alive.
 * Destroying a "moved-from" instance does not invalidate any references to items.
 */
template <move_only_any_c Any_T = BasicMoveOnlyAny<0>>
    requires(Any_T::size == 0)
class BasicGenericStack {
public:
    using Any = Any_T;


    BasicGenericStack()                             = default;
    BasicGenericStack(const BasicGenericStack&)     = delete;
    BasicGenericStack(BasicGenericStack&&) noexcept = default;
    ~BasicGenericStack();

    template <decays_to_basic_generic_stack_item_c<Any_T>... Items_T>
    explicit BasicGenericStack(Items_T&&... items);

    auto operator=(const BasicGenericStack&) -> BasicGenericStack&     = delete;
    auto operator=(BasicGenericStack&&) noexcept -> BasicGenericStack& = default;


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
    auto at(this Self_T&&) -> forward_like_t<Item_T, Self_T>;


    template <decays_to_basic_generic_stack_item_c<Any_T> Item_T>
    auto insert(Item_T&& item) -> Item_T&;
    template <basic_generic_stack_item_c<Any_T> Item_T, typename... Args_T>
    auto emplace(Args_T&&... args) -> Item_T&;
    template <decays_to_basic_generic_stack_item_injection_c<Any_T> Injection_T>
    auto inject(Injection_T&& injection) -> result_of_t<Injection_T>&;


    template <typename Self_T, std::invocable<forward_like_t<Any_T, Self_T>> F>
        requires(
            std::is_const_v<std::remove_reference_t<Self_T>>
            || std::is_rvalue_reference_v<Self_T &&>
        )
    auto for_each(this Self_T&&, F&& func) -> F;

private:
    std::vector<std::pair<uint64_t, Any_T>> m_types_and_items;

    template <typename Injection_T>
    [[nodiscard]]
    auto resolve_dependencies()
        -> type_list_to_t<arguments_of_t<Injection_T>, std::tuple>;
};

using GenericStack = BasicGenericStack<>;

template <typename T>
concept generic_stack_item_c = basic_generic_stack_item_c<T, GenericStack::Any>;

template <typename T>
concept decays_to_generic_stack_item_c = generic_stack_item_c<std::decay_t<T>>;

template <typename T>
concept generic_stack_item_injection_c =
    basic_generic_stack_item_injection_c<T, GenericStack::Any>;

template <typename T>
concept decays_to_generic_stack_item_injection_c =
    generic_stack_item_injection_c<std::decay_t<T>>;

}   // namespace kiln::util

namespace kiln::util {

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
BasicGenericStack<Any_T>::~BasicGenericStack()
{
    while (!m_types_and_items.empty())
    {
        m_types_and_items.pop_back();
    }
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <decays_to_basic_generic_stack_item_c<Any_T>... Items_T>
BasicGenericStack<Any_T>::BasicGenericStack(Items_T&&... items)
{
    (emplace<Items_T>(std::forward<Items_T>(items)), ...);
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <basic_generic_stack_item_c<Any_T> Item_T, typename Self_T>
auto BasicGenericStack<Any_T>::find(this Self_T& self) noexcept
    -> OptionalRef<const_like_t<Item_T, Self_T>>
{
    const auto iter = std::ranges::find(
        self.BasicGenericStack::m_types_and_items,
        hash<Item_T>(),
        &std::pair<uint64_t, Any_T>::first
    );
    if (iter == self.BasicGenericStack::m_types_and_items.cend())
    {
        return std::nullopt;
    }

    return OptionalRef<const_like_t<Item_T, Self_T>>{ any_cast<Item_T>(iter->second) };
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <basic_generic_stack_item_c<Any_T> Item_T, typename Self_T>
auto BasicGenericStack<Any_T>::at(this Self_T&& self) -> forward_like_t<Item_T, Self_T>
{
    const OptionalRef found_item{ std::forward<Self_T>(self).template find<Item_T>() };

    PRECOND(
        found_item.has_value(),
        std::format("Item of type `{}` not found", name_of<Item_T>())
    );

    return std::forward_like<Self_T>(*found_item);
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
auto BasicGenericStack<Any_T>::empty() const -> bool
{
    return m_types_and_items.empty();
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <basic_generic_stack_item_c<Any_T> Item_T>
auto BasicGenericStack<Any_T>::contains() const noexcept -> bool
{
    return std::ranges::contains(
        m_types_and_items, hash_u64<Item_T>(), &std::pair<uint64_t, Any_T>::first
    );
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <decays_to_basic_generic_stack_item_c<Any_T> Item_T>
auto BasicGenericStack<Any_T>::insert(Item_T&& item) -> Item_T&
{
    return emplace<std::decay_t<Item_T>>(std::forward<Item_T>(item));
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <basic_generic_stack_item_c<Any_T> Item_T, typename... Args_T>
auto BasicGenericStack<Any_T>::emplace(Args_T&&... args) -> Item_T&
{
    PRECOND(!contains<Item_T>());

    return any_cast<Item_T>(
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
    );
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <decays_to_basic_generic_stack_item_injection_c<Any_T> Injection_T>
auto BasicGenericStack<Any_T>::inject(Injection_T&& injection)
    -> result_of_t<Injection_T>&
{
    using Item = result_of_t<Injection_T>;

    PRECOND(
        (!contains<Item>()),
        std::format(
            "Attempt to inject item of type `{}`, but it has already been injected",
            name_of<Item>()
        )
    );

    return insert(
        std::apply(
            std::forward<Injection_T>(injection), resolve_dependencies<Injection_T>()
        )
    );
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <typename Self_T, std::invocable<forward_like_t<Any_T, Self_T>> F>
    requires(
        std::is_const_v<std::remove_reference_t<Self_T>>
        || std::is_rvalue_reference_v<Self_T &&>
    )
auto BasicGenericStack<Any_T>::for_each(this Self_T&& self, F&& func) -> F
{
    for (auto&& [type, item] :
         std::forward_like<Self_T>(self.BasicGenericStack::m_types_and_items))
    {
        std::invoke(func, std::forward_like<Self_T>(item));
    }

    return std::forward<F>(func);
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <typename Injection_T>
auto BasicGenericStack<Any_T>::resolve_dependencies()
    -> type_list_to_t<arguments_of_t<Injection_T>, std::tuple>
{
    return [this]<typename... Dependencies_T>(TypeList<Dependencies_T...>) -> auto
    {
        std::ignore = this;

        (PRECOND(
             contains<std::remove_cvref_t<Dependencies_T>>(),
             std::format(
                 "Item dependency of type `{}` not found",
                 util::name_of<std::remove_cvref_t<Dependencies_T>>()
             )
         ),
         ...);

        return std::forward_as_tuple(at<std::remove_cvref_t<Dependencies_T>>()...);
    }(util::arguments_of_t<Injection_T>{});
}

}   // namespace kiln::util
