module;

#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <format>
#include <functional>
#include <memory>
#include <memory_resource>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

#include "kiln/util/contract_macros.hpp"

export module kiln.util.Any;

import kiln.util.concepts.decayed;
import kiln.util.concepts.naked;
import kiln.util.concepts.nothrow_movable;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.storable;
import kiln.util.concepts.strips_to;
import kiln.util.contracts;
import kiln.util.Deleter;
import kiln.util.Dummy;
import kiln.util.reflection;
import kiln.util.type_traits.always_true;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;

namespace kiln::util {

namespace internal {

template <std::size_t size_T, std::size_t alignment_T>
struct SmallBuffer {
    alignas(alignment_T) std::array<std::byte, size_T> data;

    SmallBuffer() = default;
};

template <std::size_t size_T, std::size_t alignment_T>
using storage_t = std::variant<SmallBuffer<size_T, alignment_T>, void*>;

template <std::size_t size_T, std::size_t alignment_T, typename ExtraVTable_T>
struct VTable {
    using Storage = storage_t<size_T, alignment_T>;

    using CopyFunc = auto(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage&                           out,
        const Storage&                     storage
    ) -> void;
    using MoveFunc = auto(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage&                           out,
        Storage&&                          storage
    ) -> void;
    using DropFunc = auto(std::pmr::polymorphic_allocator<>& allocator, Storage&) -> void;
    using TypesMatchFunc   = auto(uint64_t type_hash) -> bool;
    using TypeNameFunc     = auto() -> std::string_view;
    using VoidifyFunc      = auto(Storage& storage) -> void*;
    using VoidifyConstFunc = auto(const Storage& storage) -> const void*;

    std::add_pointer_t<CopyFunc>                copy;
    std::reference_wrapper<MoveFunc>            move;
    std::reference_wrapper<DropFunc>            drop;
    std::reference_wrapper<TypesMatchFunc>      types_match;
    std::reference_wrapper<TypeNameFunc>        type_name;
    std::reference_wrapper<VoidifyFunc>         voidify;
    std::reference_wrapper<VoidifyConstFunc>    voidify_const;
    std::reference_wrapper<const ExtraVTable_T> extra_vtable;
};

class AnyBase {};

}   // namespace internal

export template <typename T>
concept any_c = std::derived_from<T, internal::AnyBase>;

export template <typename T>
concept any_traits_c = requires {
    typename T::ExtraVTable;
    decayed_c<typename T::ExtraVTable>;
    { T::is_move_only() } -> std::convertible_to<bool>;
    { T::template meets_custom_requirements<Dummy>() } -> std::convertible_to<bool>;
    {
        T::template extra_vtable<Dummy>()
    }
    -> std::same_as<std::add_lvalue_reference_t<std::add_const_t<typename T::ExtraVTable>>>;
};

export struct DefaultAnyExtraVTable {};

export template <typename>
struct DefaultAnyExtraVTableOperations {
    constexpr static DefaultAnyExtraVTable vtable;
};

export template <
    bool is_move_only_T    = false,
    typename ExtraVTable_T = DefaultAnyExtraVTable,
    template <typename> typename ExtraVTableOperations_T = DefaultAnyExtraVTableOperations,
    template <typename> typename Policy_T = always_true>
struct DefaultAnyTraits {
    using ExtraVTable = ExtraVTable_T;

    [[nodiscard]]
    consteval static auto is_move_only() noexcept -> bool
    {
        return is_move_only_T;
    }

    template <typename T>
    [[nodiscard]]
    consteval static auto meets_custom_requirements() noexcept -> bool
    {
        return Policy_T<T>::value;
    }

    template <typename T>
    [[nodiscard]]
    constexpr static auto extra_vtable() noexcept -> const ExtraVTable&
    {
        return ExtraVTableOperations_T<T>::vtable;
    }
};

export template <typename T, typename Any_T>
    requires any_c<std::remove_cvref_t<Any_T>>
          && (std::remove_cvref_t<Any_T>::template storable<T>())
auto any_cast(Any_T&& any) -> forward_like_t<T, Any_T>;

export template <naked_c T, typename Any_T>
    requires any_c<std::remove_cvref_t<Any_T>>
auto reinterpret_any_cast(Any_T&& any) -> forward_like_t<T, Any_T>;

export template <
    any_traits_c Traits_T    = DefaultAnyTraits<>,
    std::size_t  size_T      = 3 * sizeof(void*),
    std::size_t  alignment_T = sizeof(void*)>
class BasicAny : public internal::AnyBase {
public:
    using Traits = Traits_T;
    constexpr static std::size_t size{ size_T };
    constexpr static std::size_t alignment{ alignment_T };

    // required for interfacing with the standard
    using allocator_type =   // NOLINT(*-identifier-naming)
        std::pmr::polymorphic_allocator<>;

    consteval static auto is_move_only() -> bool;
    template <typename T>
    consteval static auto storable() -> bool;


    constexpr BasicAny(const BasicAny&, const allocator_type& allocator = {})
        requires(!is_move_only());
    constexpr BasicAny(BasicAny&&) noexcept;
    constexpr BasicAny(BasicAny&&, const allocator_type& allocator);
    constexpr ~BasicAny();

    template <typename T, typename... Args_T>
        requires std::constructible_from<T, Args_T&&...>
    constexpr explicit BasicAny(std::in_place_type_t<T>, Args_T&&... args)
        requires(storable<T>());

    template <typename T, typename... Args_T>
        requires std::constructible_from<T, Args_T&&...>
    constexpr explicit BasicAny(
        std::allocator_arg_t,
        const allocator_type& allocator,
        std::in_place_type_t<T>,
        Args_T&&... args
    )
        requires(storable<T>());

    template <typename T>
    constexpr explicit BasicAny(T&& value)
        requires(!strips_to_c<T, BasicAny>)   //
             && (!strips_to_c<T, std::allocator_arg_t>)
             && (!specialization_of_c<std::remove_cvref_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T&&>
             && (storable<std::decay_t<T>>());

    template <typename T>
    constexpr explicit BasicAny(
        std::allocator_arg_t,
        const allocator_type& allocator,
        T&&                   value
    )
        requires(!strips_to_c<T, BasicAny>)   //
             && (!strips_to_c<T, std::allocator_arg_t>)
             && (!specialization_of_c<std::remove_cvref_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T&&>
             && (storable<std::decay_t<T>>());


    auto operator=(const BasicAny&) -> BasicAny&
        requires(!is_move_only());
    auto operator=(BasicAny&&) noexcept -> BasicAny&;


    // required for interfacing with the standard
    [[nodiscard]]
    auto get_allocator() const -> allocator_type;


    template <typename T, typename Any_T>
        requires any_c<std::remove_cvref_t<Any_T>>
              && (std::remove_cvref_t<Any_T>::template storable<T>())
    friend auto any_cast(Any_T&& any) -> forward_like_t<T, Any_T>;

    template <typename T, typename Any_T>
        requires any_c<std::remove_cvref_t<Any_T>>
              && (std::remove_cvref_t<Any_T>::template storable<T>())
    friend auto dynamic_any_cast(Any_T&& any) -> forward_like_t<T, Any_T>;

    template <naked_c T, typename Any_T>
        requires any_c<std::remove_cvref_t<Any_T>>
    friend auto reinterpret_any_cast(Any_T&& any) -> forward_like_t<T, Any_T>;

protected:
    [[nodiscard]]
    auto extra_vtable() const -> const Traits::ExtraVTable&;

private:
    using Storage = internal::storage_t<size_T, alignment_T>;

    allocator_type m_allocator;
    const internal::VTable<size_T, alignment_T, typename Traits::ExtraVTable>* m_vtable{};
    Storage m_storage{ std::in_place_type<void*> };

    auto reset() -> void;
};

export using Any = BasicAny<>;

export template <std::size_t size_T = 3 * sizeof(void*), std::size_t alignment_T = sizeof(void*)>
using BasicCopyableAny = BasicAny<DefaultAnyTraits<>, size_T, alignment_T>;

export using CopyableAny = BasicCopyableAny<>;

export template <typename T>
concept copyable_any_c = any_c<T> && (!T::is_move_only());

export template <std::size_t size_T = 3 * sizeof(void*), std::size_t alignment_T = sizeof(void*)>
using BasicMoveOnlyAny = BasicAny<DefaultAnyTraits<true>, size_T, alignment_T>;

export using MoveOnlyAny = BasicMoveOnlyAny<>;

export template <typename T>
concept move_only_any_c = any_c<T> && (T::is_move_only());

}   // namespace kiln::util

namespace kiln::util {

namespace internal {

template <typename T, std::size_t size_T, std::size_t alignment_T>
concept small_c = sizeof(T) <= size_T && alignment_T % alignof(T) == 0
               && nothrow_movable_c<T>;

template <typename T, std::size_t size_T, std::size_t alignment_T>
concept large_c = !small_c<T, size_T, alignment_T>;

template <typename T, any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
struct Operations;

template <typename Operations_T, any_traits_c Traits_T>
struct OperationSelector {
    [[nodiscard]]
    consteval static auto select_copy_function()
    {
        if constexpr (Traits_T::is_move_only())
        {
            return nullptr;
        }
        else
        {
            return &Operations_T::copy;
        }
    }
};

template <typename T, any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
    requires small_c<T, size_T, alignment_T>
struct Operations<T, Traits_T, size_T, alignment_T> {
    using Storage     = storage_t<size_T, alignment_T>;
    using SmallBuffer = SmallBuffer<size_T, alignment_T>;
    using VTable      = VTable<size_T, alignment_T, typename Traits_T::ExtraVTable>;

    template <typename... Args_T>
    static auto create(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage&                           out,
        Args_T&&... args
    ) -> void
    {
        create_impl(allocator, out, std::forward<Args_T>(args)...);
    }

    static auto copy(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage&                           out,
        const Storage&                     storage
    ) -> void
        requires(!Traits_T::is_move_only())
    {
        create_impl(allocator, out, *static_cast<const T*>(voidify(storage)));
    }

    static auto move(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage&                           out,
        Storage&&                          storage
    ) noexcept -> void
    {
        create_impl(allocator, out, std::move(*static_cast<T*>(voidify(storage))));
    }

    static auto
        drop(std::pmr::polymorphic_allocator<>& allocator, Storage& storage) noexcept
        -> void
    {
        allocator.destroy(static_cast<T*>(voidify(storage)));
    }

    template <typename Storage_T>
        requires std::same_as<std::remove_cvref_t<Storage_T>, Storage>
    [[nodiscard]]
    static auto any_cast(Storage_T&& storage) noexcept -> forward_like_t<T, Storage_T>
    {
        using TPtr =
            std::add_pointer_t<const_like_t<T, std::remove_reference_t<Storage_T>>>;

        return std::forward_like<Storage_T>(*static_cast<TPtr>(voidify(storage)));
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

    constexpr static VTable vtable{
        .copy        = OperationSelector<Operations, Traits_T>::select_copy_function(),
        .move        = move,
        .drop        = drop,
        .types_match = types_match,
        .type_name   = type_name,
        .voidify = static_cast<std::add_lvalue_reference_t<typename VTable::VoidifyFunc>>(
            voidify
        ),
        .voidify_const =
            static_cast<std::add_lvalue_reference_t<typename VTable::VoidifyConstFunc>>(
                voidify
            ),
        .extra_vtable = Traits_T::template extra_vtable<T>(),
    };

private:
    template <typename... Args_T>
    static auto create_impl(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage&                           out,
        Args_T&&... args
    ) -> void
    {
        SmallBuffer& small_buffer = out.template emplace<SmallBuffer>();

        allocator.construct(
            reinterpret_cast<T*>(small_buffer.data.data()), std::forward<Args_T>(args)...
        );
    }
};

template <typename T, any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
    requires large_c<T, size_T, alignment_T>
struct Operations<T, Traits_T, size_T, alignment_T> {
    using Storage = storage_t<size_T, alignment_T>;
    using VTable  = VTable<size_T, alignment_T, typename Traits_T::ExtraVTable>;

    template <typename... Args_T>
    static auto create(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage&                           out,
        Args_T&&... args
    ) -> void
    {
        std::unique_ptr<T, Deleter> handle{
            allocator.new_object<T>(std::forward<Args_T>(args)...),
            Deleter{ allocator }   //
        };

        out.template emplace<void*>(handle.release());
    }

    static auto copy(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage&                           out,
        const Storage&                     storage
    ) -> void
        requires(!Traits_T::is_move_only())
    {
        create(allocator, out, *static_cast<const T*>(voidify(storage)));
    }

    static auto
        move(std::pmr::polymorphic_allocator<>&, Storage& out, Storage&& storage) noexcept
        -> void
    {
        out.template emplace<void*>(voidify(storage));
        storage.template emplace<void*>(nullptr);
    }

    static auto
        drop(std::pmr::polymorphic_allocator<>& allocator, Storage& storage) noexcept
        -> void
    {
        allocator.delete_object(static_cast<T*>(voidify(storage)));
    }

    template <typename Storage_T>
        requires std::same_as<std::remove_cvref_t<Storage_T>, Storage>
    [[nodiscard]]
    static auto any_cast(Storage_T&& storage) noexcept -> forward_like_t<T, Storage_T>
    {
        using TPtr =
            std::add_pointer_t<const_like_t<T, std::remove_reference_t<Storage_T>>>;

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

    constexpr static VTable vtable{
        .copy        = OperationSelector<Operations, Traits_T>::select_copy_function(),
        .move        = move,
        .drop        = drop,
        .types_match = types_match,
        .type_name   = type_name,
        .voidify = static_cast<std::add_lvalue_reference_t<typename VTable::VoidifyFunc>>(
            voidify
        ),
        .voidify_const =
            static_cast<std::add_lvalue_reference_t<typename VTable::VoidifyConstFunc>>(
                voidify
            ),
        .extra_vtable = Traits_T::template extra_vtable<T>(),
    };
};

}   // namespace internal

template <typename T, typename Any_T>
    requires any_c<std::remove_cvref_t<Any_T>>
          && (std::remove_cvref_t<Any_T>::template storable<T>())
auto any_cast(Any_T&& any) -> forward_like_t<T, Any_T>
{
#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
    // GCC and MSVC needs this to lookup base type
    using NakedAny = std::remove_cvref_t<Any_T>;
#ifdef __clang__
  #pragma clang diagnostic pop
#endif

    PRECOND(
        any.NakedAny::BasicAny::m_vtable != nullptr,
        "Don't use a 'moved-from' (or destroyed) Any!"
    );

    PRECOND(
        any.NakedAny::BasicAny::m_vtable->types_match(util::hash<T>()),
        std::format(
            "`Any` has type {}, but requested type is {}",
            any.NakedAny::BasicAny::m_vtable->type_name(),
            util::name_of<T>()
        )
    );

    return internal::Operations<
        T,
        typename std::remove_cvref_t<Any_T>::BasicAny::Traits,
        std::remove_cvref_t<Any_T>::BasicAny::size,
        std::remove_cvref_t<Any_T>::BasicAny::alignment>::
        any_cast(std::forward_like<Any_T>(any.NakedAny::BasicAny::m_storage));
}

template <naked_c T, typename Any_T>
    requires any_c<std::remove_cvref_t<Any_T>>
auto reinterpret_any_cast(Any_T&& any) -> forward_like_t<T, Any_T>
{
#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
    // GCC and MSVC needs this to lookup base type
    using NakedAny = std::remove_cvref_t<Any_T>;
#ifdef __clang__
  #pragma clang diagnostic pop
#endif

    PRECOND(
        any.NakedAny::BasicAny::m_vtable != nullptr,
        "Don't use a 'moved-from' (or destroyed) Any!"
    );

    if constexpr (!std::is_const_v<std::remove_reference_t<Any_T>>)
    {
        return std::forward_like<Any_T>(*reinterpret_cast<std::add_pointer_t<T>>(
            any.NakedAny::BasicAny::m_vtable->voidify(any.NakedAny::BasicAny::m_storage)
        ));
    }
    else
    {
        return std::forward_like<Any_T>(
            *reinterpret_cast<std::add_pointer_t<std::add_const_t<T>>>(
                any.NakedAny::BasicAny::m_vtable->voidify_const(
                    any.NakedAny::BasicAny::m_storage
                )
            )
        );
    }
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
consteval auto BasicAny<Traits_T, size_T, alignment_T>::is_move_only() -> bool
{
    return Traits_T::is_move_only();
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
template <typename T>
consteval auto BasicAny<Traits_T, size_T, alignment_T>::storable() -> bool
{
    return storable_c<T> && (is_move_only() || std::copy_constructible<T>)
        && Traits_T::template meets_custom_requirements<T>();
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
constexpr BasicAny<Traits_T, size_T, alignment_T>::BasicAny(
    const BasicAny&       other,
    const allocator_type& allocator
)
    requires(!is_move_only())
    : m_allocator{ allocator },
      m_vtable{ other.m_vtable }
{
    PRECOND(other.m_vtable != nullptr, "Don't use a 'moved-from' (or destroyed) Any!");
    if (m_vtable)
    {
        m_vtable->copy(m_allocator, m_storage, other.m_storage);
    }
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
constexpr BasicAny<Traits_T, size_T, alignment_T>::BasicAny(BasicAny&& other) noexcept
    : BasicAny{ std::move(other), other.get_allocator() }
{
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
constexpr BasicAny<Traits_T, size_T, alignment_T>::BasicAny(
    BasicAny&&            other,
    const allocator_type& allocator
)
    : m_allocator{ allocator }
{
    operator=(std::move(other));
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
constexpr BasicAny<Traits_T, size_T, alignment_T>::
    ~BasicAny<Traits_T, size_T, alignment_T>()
{
    reset();
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
template <typename T, typename... Args_T>
    requires std::constructible_from<T, Args_T&&...>
constexpr BasicAny<Traits_T, size_T, alignment_T>::BasicAny(
    std::in_place_type_t<T>,
    Args_T&&... args
)
    requires(storable<T>())
    : BasicAny{
          std::allocator_arg,
          allocator_type{},
          std::in_place_type<std::decay_t<T>>,
          std::forward<Args_T>(args)...,
      }
{
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
template <typename T, typename... Args_T>
    requires std::constructible_from<T, Args_T&&...>
constexpr BasicAny<Traits_T, size_T, alignment_T>::BasicAny(
    std::allocator_arg_t,
    const allocator_type& allocator,
    std::in_place_type_t<T>,
    Args_T&&... args
)
    requires(storable<T>())
    : m_allocator{ allocator },
      m_vtable{ &internal::Operations<T, Traits_T, size_T, alignment_T>::vtable }
{
    internal::Operations<T, Traits_T, size_T, alignment_T>::create(
        m_allocator, m_storage, std::forward<Args_T>(args)...
    );
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
template <typename T>
constexpr BasicAny<Traits_T, size_T, alignment_T>::BasicAny(T&& value)
    requires(!strips_to_c<T, BasicAny>)   //
         && (!strips_to_c<T, std::allocator_arg_t>)
         && (!specialization_of_c<std::remove_cvref_t<T>, std::in_place_type_t>)
         && std::constructible_from<std::decay_t<T>, T&&>   //
         && (storable<std::decay_t<T>>())
    : BasicAny{
          std::allocator_arg,
          allocator_type{},
          std::forward<T>(value),
      }
{
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
template <typename T>
constexpr BasicAny<Traits_T, size_T, alignment_T>::BasicAny(
    std::allocator_arg_t,
    const allocator_type& allocator,
    T&&                   value
)
    requires(!strips_to_c<T, BasicAny>)   //
         && (!strips_to_c<T, std::allocator_arg_t>)
         && (!specialization_of_c<std::remove_cvref_t<T>, std::in_place_type_t>)
         && std::constructible_from<std::decay_t<T>, T&&>   //
         && (storable<std::decay_t<T>>())
    : BasicAny{
          std::allocator_arg,
          allocator,
          std::in_place_type<std::decay_t<T>>,
          std::forward<T>(value),
      }
{
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
auto BasicAny<Traits_T, size_T, alignment_T>::operator=(const BasicAny& other)
    -> BasicAny&
    requires(!is_move_only())
{
    if (&other == this)
    {
        return *this;
    }

    return *this = BasicAny{ other };
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
auto BasicAny<Traits_T, size_T, alignment_T>::operator=(BasicAny&& other) noexcept
    -> BasicAny&
{
    PRECOND(other.m_vtable != nullptr, "Don't use a 'moved-from' (or destroyed) Any!");

    if (&other == this)
    {
        return *this;
    }

    reset();

    m_vtable = std::exchange(other.m_vtable, nullptr);
    if (m_vtable)
    {
        if (m_allocator == other.m_allocator)
        {
            m_vtable->move(m_allocator, m_storage, std::move(other.m_storage));
            m_vtable->drop(other.m_allocator, other.m_storage);
        }
        else
        {
            if constexpr (!is_move_only())
            {
                m_vtable->copy(m_allocator, m_storage, other.m_storage);
                m_vtable->drop(other.m_allocator, other.m_storage);
            }
            else
            {
                PRECOND(
                    false,
                    "Move only any cannot be move assigned using different allocators"
                );
            }
        }
    }

    other.reset();

    return *this;
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
auto BasicAny<Traits_T, size_T, alignment_T>::get_allocator() const -> allocator_type
{
    return m_allocator;
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
auto BasicAny<Traits_T, size_T, alignment_T>::extra_vtable() const
    -> const Traits_T::ExtraVTable&
{
    PRECOND(m_vtable != nullptr, "Don't use a 'moved-from' (or destroyed) Any!");
    return m_vtable->extra_vtable;
}

template <any_traits_c Traits_T, std::size_t size_T, std::size_t alignment_T>
auto BasicAny<Traits_T, size_T, alignment_T>::reset() -> void
{
    if (m_vtable)
    {
        m_vtable->drop(m_allocator, m_storage);
        m_vtable = nullptr;
    }
}

}   // namespace kiln::util
