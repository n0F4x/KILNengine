#pragma once

#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <format>
#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

#include "kiln/util/concepts.hpp"
#include "kiln/util/contract_macros.hpp"
#include "kiln/util/Deallocator.hpp"
#include "kiln/util/reflection.hpp"
#include "kiln/util/type_traits.hpp"

namespace kiln::util {

namespace internal {

template <std::size_t size_T, std::size_t alignment_T>
struct SmallBuffer {
    alignas(alignment_T) std::array<std::byte, size_T> data;

    SmallBuffer() = default;
};

template <std::size_t size_T, std::size_t alignment_T>
using storage_t = std::variant<SmallBuffer<size_T, alignment_T>, void*>;

template <std::size_t size_T, std::size_t alignment_T>
struct Operations {
    using Storage = storage_t<size_T, alignment_T>;

    using CopyFunc       = auto (*)(Storage& out, const Storage& storage) -> void;
    using MoveFunc       = auto (&)(Storage& out, Storage&& storage) -> void;
    using DropFunc       = auto (&)(Storage&&) -> void;
    using TypesMatchFunc = auto (&)(uint64_t type_hash) -> bool;
    using TypeNameFunc   = auto (&)() -> std::string_view;
    using VoidifyFunc    = auto (&)(Storage& storage) -> void*;

    CopyFunc       copy;
    MoveFunc       move;
    DropFunc       drop;
    TypesMatchFunc types_match;
    TypeNameFunc   type_name;
    VoidifyFunc    voidify;
};

class AnyBase {};

template <typename T>
using PermissiveConceptPolicy = always_true<T>;

}   // namespace internal

struct AnyProperties {
    bool move_only{};

    template <typename T>
    [[nodiscard]]
    constexpr auto satisfied() const noexcept -> bool
    {
        return storable_c<T> && move_only ? true : std::copy_constructible<T>;
    }
};

template <typename T>
concept any_c = std::derived_from<T, internal::AnyBase>;

template <typename T>
concept copyable_any_c = any_c<T> && (!T::is_move_only());

template <typename T>
concept move_only_any_c = any_c<T> && (T::is_move_only());

template <typename T, typename Any_T>
    requires any_c<std::remove_cvref_t<Any_T>>
          && (std::remove_cvref_t<Any_T>::template storable<T>())
auto any_cast(Any_T&& any) -> forward_like_t<T, Any_T>;

template <typename T, typename Any_T>
    requires any_c<std::remove_cvref_t<Any_T>>
          && (std::remove_cvref_t<Any_T>::template storable<T>())
auto dynamic_any_cast(Any_T&& any) -> forward_like_t<T, Any_T>;

template <lvalue_reference_c T, typename Any_T>
    requires any_c<std::remove_const_t<Any_T>>
          && preserves_const_c<std::remove_reference_t<T>, Any_T>
auto reinterpret_any_cast(Any_T& any) -> T;

template <
    AnyProperties properties_T                   = {},
    template <typename> typename ConceptPolicy_T = internal::PermissiveConceptPolicy,
    std::size_t size_T                           = 3 * sizeof(void*),
    std::size_t alignment_T                      = sizeof(void*)>
class BasicAny : public internal::AnyBase {
public:
    constexpr static AnyProperties properties{ properties_T };
    constexpr static std::size_t   size{ size_T };
    constexpr static std::size_t   alignment{ alignment_T };

    consteval static auto is_move_only() -> bool;
    template <typename T>
    consteval static auto storable() -> bool;


    constexpr BasicAny(const BasicAny&)
        requires(!properties_T.move_only);
    constexpr BasicAny(BasicAny&&) noexcept;
    constexpr ~BasicAny();

    template <typename T, typename... Args_T>
        requires std::constructible_from<T, Args_T&&...>
    constexpr explicit BasicAny(std::in_place_type_t<T>, Args_T&&... args)
        requires(storable<T>());

    template <typename T>
    constexpr explicit BasicAny(T&& value)
        requires(!std::same_as<std::decay_t<T>, BasicAny>)
             && (!specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T&&>
             && (storable<std::decay_t<T>>());


    auto operator=(const BasicAny&) -> BasicAny&
        requires(!properties_T.move_only);
    auto operator=(BasicAny&&) noexcept -> BasicAny&;


    template <typename T, typename Any_T>
        requires any_c<std::remove_cvref_t<Any_T>>
              && (std::remove_cvref_t<Any_T>::template storable<T>())
    friend auto any_cast(Any_T&& any) -> forward_like_t<T, Any_T>;

    template <typename T, typename Any_T>
        requires any_c<std::remove_cvref_t<Any_T>>
              && (std::remove_cvref_t<Any_T>::template storable<T>())
    friend auto dynamic_any_cast(Any_T&& any) -> forward_like_t<T, Any_T>;

    template <lvalue_reference_c T, typename Any_T>
        requires any_c<std::remove_const_t<Any_T>>
              && preserves_const_c<std::remove_reference_t<T>, Any_T>
    friend auto reinterpret_any_cast(Any_T& any) -> T;


private:
    using Storage = internal::storage_t<size_T, alignment_T>;

    const internal::Operations<size_T, alignment_T>* m_operations;
    Storage m_storage{ std::in_place_type<void*> };

    auto reset() -> void;
};

using Any = BasicAny<>;

template <std::size_t size_T = 3 * sizeof(void*), std::size_t alignment_T = sizeof(void*)>
using BasicCopyableAny = BasicAny<{}, always_true, size_T, alignment_T>;

using CopyableAny = BasicCopyableAny<>;

template <std::size_t size_T = 3 * sizeof(void*), std::size_t alignment_T = sizeof(void*)>
using BasicMoveOnlyAny =
    BasicAny<AnyProperties{ .move_only = true }, always_true, size_T, alignment_T>;

using MoveOnlyAny = BasicMoveOnlyAny<>;

}   // namespace kiln::util

namespace kiln::util {

namespace internal {

template <typename T, std::size_t size_T, std::size_t alignment_T>
concept small_c = sizeof(T) <= size_T && alignment_T % alignof(T) == 0
               && nothrow_movable_c<T>;

template <typename T, std::size_t size_T, std::size_t alignment_T>
concept large_c = !small_c<T, size_T, alignment_T>;

template <typename T, AnyProperties properties_T, std::size_t size_T, std::size_t alignment_T>
struct Traits;

template <typename Traits_T, AnyProperties properties_T>
struct TraitsFunctionSelector {
    [[nodiscard]]
    consteval static auto select_copy_function()
    {
        if constexpr (properties_T.move_only)
        {
            return nullptr;
        }
        else
        {
            return &Traits_T::copy;
        }
    }
};

template <typename T, AnyProperties properties_T, std::size_t size_T, std::size_t alignment_T>
    requires small_c<T, size_T, alignment_T>
struct Traits<T, properties_T, size_T, alignment_T> {
    using Storage     = storage_t<size_T, alignment_T>;
    using SmallBuffer = SmallBuffer<size_T, alignment_T>;
    using Operations  = Operations<size_T, alignment_T>;

    template <typename... Args_T>
    static auto create(Storage& out, Args_T&&... args) -> void
    {
        create_impl(out, std::forward<Args_T>(args)...);
    }

    static auto copy(Storage& out, const Storage& storage) -> void
        requires(!properties_T.move_only)
    {
        return create_impl(out, *static_cast<const T*>(voidify(storage)));
    }

    static auto move(Storage& out, Storage&& storage) noexcept -> void
    {
        return create_impl(out, std::move(*static_cast<T*>(voidify(storage))));
    }

    static auto drop(Storage&& storage) noexcept -> void
    {
        std::destroy_at(static_cast<T*>(voidify(storage)));
    }

    template <typename Storage_T>
        requires std::same_as<std::remove_cvref_t<Storage_T>, Storage>
    [[nodiscard]]
    static auto any_cast(Storage_T&& storage) noexcept -> forward_like_t<T, Storage_T>
    {
        using TPtr = std::
            conditional_t<std::is_const_v<std::remove_reference_t<Storage_T>>, const T*, T*>;
        using VoidPtr = std::conditional_t<
            std::is_const_v<std::remove_reference_t<Storage_T>>,
            const void*,
            void*>;

        return std::forward_like<Storage_T>(
            *static_cast<TPtr>(static_cast<VoidPtr>(voidify(storage)))
        );
    }

    [[nodiscard]]
    static auto types_match(uint64_t type_hash) -> bool
    {
        return type_hash == hash<T>();
    }

    [[nodiscard]]
    static auto type_name() -> std::string_view
    {
        return name_of<T>();
    }

    [[nodiscard]]
    static auto voidify(Storage& storage) -> void*
    {
        SmallBuffer* buffer_ptr{ std::get_if<SmallBuffer>(&storage) };
        assert(buffer_ptr != nullptr);
        return static_cast<void*>(buffer_ptr->data.data());
    }

    [[nodiscard]]
    static auto voidify(const Storage& storage) -> const void*
    {
        const SmallBuffer* buffer_ptr{ std::get_if<SmallBuffer>(&storage) };
        assert(buffer_ptr != nullptr);
        return static_cast<const void*>(buffer_ptr->data.data());
    }

    constexpr static Operations s_operations{
        .copy = TraitsFunctionSelector<Traits, properties_T>::select_copy_function(),
        .move = move,
        .drop = drop,
        .types_match = types_match,
        .type_name   = type_name,
        .voidify     = static_cast<Operations::VoidifyFunc>(voidify),
    };

private:
    template <typename... Args_T>
    static auto create_impl(Storage& out, Args_T&&... args) -> void
    {
        SmallBuffer& small_buffer = out.template emplace<SmallBuffer>();

        std::construct_at(
            reinterpret_cast<T*>(small_buffer.data.data()), std::forward<Args_T>(args)...
        );
    }
};

template <typename T, AnyProperties properties_T, std::size_t size_T, std::size_t alignment_T>
    requires large_c<T, size_T, alignment_T>
struct Traits<T, properties_T, size_T, alignment_T> {
    using Storage    = storage_t<size_T, alignment_T>;
    using Operations = Operations<size_T, alignment_T>;
    using Allocator  = std::allocator<T>;

    template <typename... Args_T>
    static auto create(Storage& out, Args_T&&... args) -> void
    {
        using Deallocator = Deallocator<Allocator>;

        Allocator allocator{};

        std::unique_ptr<T, Deallocator> handle{
            allocator.allocate(1), Deallocator{ allocator }   //
        };
        std::construct_at(handle.get(), std::forward<Args_T>(args)...);

        out.template emplace<void*>(handle.release());
    }

    static auto copy(Storage& out, const Storage& storage) -> void
        requires(!properties_T.move_only)
    {
        return create(out, *static_cast<const T*>(voidify(storage)));
    }

    static auto move(Storage& out, Storage&& storage) noexcept -> void
    {
        out.template emplace<void*>(voidify(storage));
    }

    static auto drop(Storage&& storage) noexcept -> void
    {
        Allocator allocator{};
        allocator.deallocate(static_cast<T*>(voidify(storage)), 1);
    }

    template <typename Storage_T>
        requires std::same_as<std::remove_cvref_t<Storage_T>, Storage>
    [[nodiscard]]
    static auto any_cast(Storage_T&& storage) noexcept -> forward_like_t<T, Storage_T>
    {
        using TPtr = std::
            conditional_t<std::is_const_v<std::remove_reference_t<Storage_T>>, const T*, T*>;

        return std::forward_like<Storage_T>(*static_cast<TPtr>(voidify(storage)));
    }

    [[nodiscard]]
    static auto types_match(const uint64_t type_hash) -> bool
    {
        return type_hash == hash<T>();
    }

    [[nodiscard]]
    static auto type_name() -> std::string_view
    {
        return name_of<T>();
    }

    [[nodiscard]]
    static auto voidify(Storage& storage) -> void*
    {
        auto* const handle_ptr{ std::get_if<void*>(&storage) };
        assert(handle_ptr != nullptr);
        return *handle_ptr;
    }

    [[nodiscard]]
    static auto voidify(const Storage& storage) -> const void*
    {
        const auto* const handle_ptr{ std::get_if<void*>(&storage) };
        assert(handle_ptr != nullptr);
        return *handle_ptr;
    }

    constexpr static Operations s_operations{
        .copy = TraitsFunctionSelector<Traits, properties_T>::select_copy_function(),
        .move = move,
        .drop = drop,
        .types_match = types_match,
        .type_name   = type_name,
        .voidify     = static_cast<Operations::VoidifyFunc>(voidify),
    };
};

}   // namespace internal

template <typename T, typename Any_T>
    requires any_c<std::remove_cvref_t<Any_T>>
          && (std::remove_cvref_t<Any_T>::template storable<T>())
auto any_cast(Any_T&& any) -> forward_like_t<T, Any_T>
{
    PRECOND(
        any.BasicAny::m_operations->types_match(util::hash<T>()),
        std::format(
            "`Any` has type {}, but requested type is {}",
            any.BasicAny::m_operations->type_name(),
            util::name_of<T>()
        )
    );

    return dynamic_any_cast<T>(std::forward<Any_T>(any));
}

template <typename T, typename Any_T>
    requires any_c<std::remove_cvref_t<Any_T>>
          && (std::remove_cvref_t<Any_T>::template storable<T>())
auto dynamic_any_cast(Any_T&& any) -> forward_like_t<T, Any_T>
{
    PRECOND(
        any.BasicAny::m_operations != nullptr,
        "Don't use a 'moved-from' (or destroyed) Any!"
    );

    return internal::Traits<
        T,
        std::remove_cvref_t<Any_T>::BasicAny::properties,
        std::remove_cvref_t<Any_T>::BasicAny::size,
        std::remove_cvref_t<Any_T>::BasicAny::alignment>::
        any_cast(std::forward_like<Any_T>(any.BasicAny::m_storage));
}

template <lvalue_reference_c T, typename Any_T>
    requires any_c<std::remove_const_t<Any_T>>
          && preserves_const_c<std::remove_reference_t<T>, Any_T>
auto reinterpret_any_cast(Any_T& any) -> T
{
    return *reinterpret_cast<std::add_pointer_t<std::remove_reference_t<T>>>(
        any.BasicAny::m_operations->voidify(
            const_cast<Any::BasicAny::Storage&>(any.BasicAny::m_storage)
        )
    );
}

template <
    AnyProperties properties_T,
    template <typename> class ConceptPolicy_T,
    std::size_t size_T,
    std::size_t alignment_T>
consteval auto BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T>::is_move_only()
    -> bool
{
    return properties_T.move_only;
}

template <
    AnyProperties properties_T,
    template <typename> class ConceptPolicy_T,
    std::size_t size_T,
    std::size_t alignment_T>
template <typename T>
consteval auto BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T>::storable()
    -> bool
{
    return properties_T.template satisfied<T>() && ConceptPolicy_T<T>::value;
}

template <
    AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T>::BasicAny(
    const BasicAny& other
)
    requires(!properties_T.move_only)
    : m_operations{ other.m_operations }
{
    PRECOND(other.m_operations != nullptr, "Don't use a 'moved-from' (or destroyed) Any!");
    if (m_operations)
    {
        m_operations->copy(m_storage, other.m_storage);
    }
}

template <
    AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T>::BasicAny(
    BasicAny&& other
) noexcept
    : m_operations{ std::exchange(other.m_operations, nullptr) }
{
    PRECOND(m_operations != nullptr, "Don't use a 'moved-from' (or destroyed) Any!");
    if (m_operations)
    {
        m_operations->move(m_storage, std::move(other.m_storage));
    }
    other.reset();
}

template <
    AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T>::
    ~BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T>()
{
    reset();
}

template <
    AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t size_T,
    std::size_t alignment_T>
template <typename T, typename... Args_T>
    requires std::constructible_from<T, Args_T&&...>
constexpr BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T>::BasicAny(
    std::in_place_type_t<T>,
    Args_T&&... args
)
    requires(storable<T>())
    : m_operations{ &internal::Traits<T, properties_T, size_T, alignment_T>::s_operations }
{
    internal::Traits<T, properties_T, size_T, alignment_T>::create(
        m_storage, std::forward<Args_T>(args)...
    );
}

template <
    AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t size_T,
    std::size_t alignment_T>
template <typename T>
constexpr BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T>::BasicAny(T&& value)
    requires(!std::same_as<std::decay_t<T>, BasicAny>)
         && (!specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
         && std::constructible_from<std::decay_t<T>, T&&> && (storable<std::decay_t<T>>())
    : BasicAny{ std::in_place_type<std::decay_t<T>>, std::forward<T>(value) }
{
}

template <
    AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t size_T,
    std::size_t alignment_T>
auto BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T>::operator=(
    const BasicAny& other
) -> BasicAny&
    requires(!properties_T.move_only)   //
{
    if (&other == this)
    {
        return *this;
    }

    return *this = BasicAny{ other };
}

template <
    AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t size_T,
    std::size_t alignment_T>
auto BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T>::operator=(
    BasicAny&& other
) noexcept -> BasicAny&
{
    PRECOND(other.m_operations != nullptr, "Don't use a 'moved-from' (or destroyed) Any!");

    if (&other == this)
    {
        return *this;
    }

    reset();

    m_operations = std::exchange(other.m_operations, nullptr);
    if (m_operations)
    {
        m_operations->move(m_storage, std::move(other.m_storage));
    }

    other.reset();

    return *this;
}

template <
    AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t size_T,
    std::size_t alignment_T>
auto BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T>::reset() -> void
{
    if (m_operations)
    {
        m_operations->drop(std::move(m_storage));
        m_operations = nullptr;
    }
}

}   // namespace kiln::util
