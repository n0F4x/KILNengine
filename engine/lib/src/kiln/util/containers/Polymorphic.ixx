module;

#include <concepts>
#include <functional>
#include <memory_resource>
#include <utility>

#include "kiln/util/contract_macros.hpp"
#include "kiln/util/no_unique_address.hpp"

export module kiln.util.containers.Polymorphic;

import kiln.util.concepts.naked;
import kiln.util.concepts.specialization_of;
import kiln.util.contracts;
import kiln.util.reflection;
import kiln.util.ScopeFail;

namespace kiln::util {

template <typename T, bool enable_T>
class CopyMechanism;

template <typename T>
class CopyMechanism<T, false> {
public:
    template <typename U>
    constexpr explicit CopyMechanism(std::in_place_type_t<U>) noexcept
    {
    }
};

template <typename T>
class CopyMechanism<T, true> {
public:
    template <typename U>
    constexpr explicit CopyMechanism(std::in_place_type_t<U>) noexcept;

    [[nodiscard]]
    auto construct(std::pmr::polymorphic_allocator<T>& allocator, const T* source) const
        -> T*;

    auto assign(
        std::pmr::polymorphic_allocator<T>& allocator,
        T*&                                 destination,
        const T*                            source,
        uint64_t                            destination_type,
        uint64_t                            source_type
    ) const -> void;

    [[nodiscard]]
    auto type_hash() const -> uint64_t;

private:
    struct VTable;

    std::reference_wrapper<const VTable> m_vtable;
};

export template <typename T, bool move_only_T = false>
class Polymorphic {
public:
    using ValueType      = T;
    // NOLINTNEXTLINE(*-identifier-naming)
    using allocator_type = std::pmr::polymorphic_allocator<T>;


    consteval static auto is_move_only() -> bool;
    template <typename U>
    consteval static auto storable() -> bool;


    Polymorphic(const Polymorphic&)
        requires(!is_move_only());
    Polymorphic(const Polymorphic&, const allocator_type& allocator)
        requires(!is_move_only());
    Polymorphic(Polymorphic&&) noexcept;
    Polymorphic(Polymorphic&&, const allocator_type& allocator)
        requires(!is_move_only());
    Polymorphic(Polymorphic&&, const allocator_type& allocator)
        requires(is_move_only());
    ~Polymorphic();

    template <typename U>
    explicit Polymorphic(U&& value)
        requires(
            !std::is_same_v<std::remove_cvref_t<U>, Polymorphic>
            && !util::specialization_of_c<U, std::in_place_type_t>
            && std::is_constructible_v<std::remove_cvref_t<U>, U &&>
            && storable<std::remove_cvref_t<U>>()
        );
    template <typename U>
    Polymorphic(std::allocator_arg_t, const allocator_type& allocator, U&& value)
        requires(
            !std::is_same_v<std::remove_cvref_t<U>, Polymorphic>
            && !util::specialization_of_c<U, std::in_place_type_t>
            && std::is_constructible_v<std::remove_cvref_t<U>, U &&>
            && storable<std::remove_cvref_t<U>>()
        );

    template <naked_c U, typename... Args_T>
    explicit Polymorphic(std::in_place_type_t<U>, Args_T&&... args)
        requires(std::is_constructible_v<U, Args_T && ...> && storable<U>());
    template <naked_c U, typename... Args_T>
    Polymorphic(
        std::allocator_arg_t,
        const allocator_type& allocator,
        std::in_place_type_t<U>,
        Args_T&&... args
    )
        requires(std::is_constructible_v<U, Args_T && ...> && storable<U>());

    auto operator=(const Polymorphic&) -> Polymorphic&
        requires(!is_move_only());
    // ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
    // NOLINTNEXTLINE(*-noexcept-move-operations,*-noexcept-move,*-noexcept-move-constructor)
    auto operator=(Polymorphic&&) -> Polymorphic&
        requires(!is_move_only());
    auto operator=(Polymorphic&&) noexcept -> Polymorphic&
        requires(is_move_only());


    [[nodiscard]]
    auto operator->() noexcept -> T*;
    [[nodiscard]]
    auto operator->() const noexcept -> const T*;

    [[nodiscard]]
    auto operator*() noexcept -> T&;
    [[nodiscard]]
    auto operator*() const noexcept -> const T&;


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    // ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
    auto swap(Polymorphic& other) -> void;   // NOLINT(*-noexcept-swap)

private:
    allocator_type m_allocator;
    T*             m_handle;
    [[kiln_no_unique_address]]
    CopyMechanism<T, !is_move_only()> m_copy_mechanism;


    auto release() -> void;
};

template <typename T>
// ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
auto swap(Polymorphic<T>& lhs, Polymorphic<T>& rhs) -> void;   // NOLINT(*-noexcept-swap)

}   // namespace kiln::util

namespace kiln::util {

template <typename T>
struct CopyMechanism<T, true>::VTable {
    using ConstructFunc =
        auto(std::pmr::polymorphic_allocator<T>& allocator, const T* source) -> T*;
    using AssignFunc = auto(
        std::pmr::polymorphic_allocator<T>& allocator,
        T*&                                 destination,
        const T*                            source,
        uint64_t                            destination_type,
        uint64_t                            source_type
    ) -> void;
    using TypeHash = auto() -> uint64_t;

    template <typename U>
    struct Operations;


    std::reference_wrapper<ConstructFunc> construct;
    std::reference_wrapper<AssignFunc>    assign;
    std::reference_wrapper<TypeHash>      type_hash;
};

template <typename T>
template <typename U>
struct CopyMechanism<T, true>::VTable::Operations {
    static auto
        construct(std::pmr::polymorphic_allocator<T>& allocator, const T* const source)
            -> T*
    {
        if (source == nullptr)
        {
            return nullptr;
        }

        return allocator.template new_object<U>(static_cast<const U&>(*source));   //
    }

    static auto assign(
        std::pmr::polymorphic_allocator<T>& allocator,
        T*&                                 destination,
        const T* const                      source,
        const uint64_t                      destination_type,
        const uint64_t                      source_type
    ) -> void
    {
        if (destination != nullptr && source != nullptr && destination_type == source_type)
        {
            static_cast<U&>(*destination) = static_cast<const U&>(*source);
            return;
        }

        T* const new_object =
            source == nullptr
                ? nullptr
                : allocator.template new_object<U>(static_cast<const U&>(*source));
        const ScopeFail new_object_guard{
            [&] noexcept -> void
            {
                allocator.delete_object(new_object);   //
            }   //
        };

        if (destination != nullptr)
        {
            allocator.delete_object(destination);
        }

        destination = new_object;
    }

    constexpr static auto type_hash() -> uint64_t
    {
        return util::hash_u64<U>();
    }

    constexpr static VTable vtable{
        .construct = construct,
        .assign    = assign,
        .type_hash = type_hash,
    };
};

template <typename T>
template <typename U>
constexpr CopyMechanism<T, true>::CopyMechanism(std::in_place_type_t<U>) noexcept
    : m_vtable{ VTable::template Operations<U>::vtable }
{
}

template <typename T>
auto CopyMechanism<T, true>::construct(
    std::pmr::polymorphic_allocator<T>& allocator,
    const T* const                      source
) const -> T*
{
    return m_vtable.get().construct(allocator, source);
}

template <typename T>
auto CopyMechanism<T, true>::assign(
    std::pmr::polymorphic_allocator<T>& allocator,
    T*&                                 destination,
    const T* const                      source,
    const uint64_t                      destination_type,
    const uint64_t                      source_type
) const -> void
{
    m_vtable.get().assign(allocator, destination, source, destination_type, source_type);
}

template <typename T>
auto CopyMechanism<T, true>::type_hash() const -> uint64_t
{
    return m_vtable.get().type_hash();
}

template <typename T, bool move_only_T>
consteval auto Polymorphic<T, move_only_T>::is_move_only() -> bool
{
    return move_only_T;
}

template <typename T, bool move_only_T>
template <typename U>
consteval auto Polymorphic<T, move_only_T>::storable() -> bool
{
    return is_move_only() || std::copyable<U>;
}

template <typename T, bool move_only_T>
Polymorphic<T, move_only_T>::Polymorphic(const Polymorphic& other)
    requires(!is_move_only())
    : Polymorphic{ other, other.m_allocator }
{
}

template <typename T, bool move_only_T>
Polymorphic<T, move_only_T>::Polymorphic(
    const Polymorphic&    other,
    const allocator_type& allocator
)
    requires(!is_move_only())
    : m_allocator{ allocator },
      m_handle{ other.m_copy_mechanism.construct(m_allocator, other.m_handle) },
      m_copy_mechanism{ other.m_copy_mechanism }
{
}

template <typename T, bool move_only_T>
Polymorphic<T, move_only_T>::Polymorphic(Polymorphic&& other) noexcept
    : m_allocator{ other.m_allocator },
      m_handle{ std::exchange(other.m_handle, nullptr) },
      m_copy_mechanism{ other.m_copy_mechanism }
{
}

template <typename T, bool enable_copy_T>
[[nodiscard]]
auto move_construct_copyable_handle(
    std::pmr::polymorphic_allocator<T>&       new_allocator,
    const std::pmr::polymorphic_allocator<T>& other_allocator,
    T*&                                       other_handle,
    const CopyMechanism<T, enable_copy_T>&    copy_mechanism
) -> T*
{
    if (new_allocator == other_allocator)
    {
        return std::exchange(other_handle, nullptr);
    }

    return copy_mechanism.construct(new_allocator, other_handle);
}

template <typename T, bool move_only_T>
Polymorphic<T, move_only_T>::Polymorphic(
    Polymorphic&&         other,
    const allocator_type& allocator
)
    requires(!is_move_only())
    : m_allocator{ allocator },
      m_handle{
          move_construct_copyable_handle(
              m_allocator,
              other.m_allocator,
              other.m_handle,
              other.m_copy_mechanism
          )   //
      },
      m_copy_mechanism{ other.m_copy_mechanism }
{
}

template <typename T>
[[nodiscard]]
auto assert_allocator_upon_move_only_move_construct(
    const std::pmr::polymorphic_allocator<T>& new_allocator,
    const std::pmr::polymorphic_allocator<T>& other_allocator
) -> std::pmr::polymorphic_allocator<T>
{
    PRECOND(new_allocator == other_allocator);
    return new_allocator;
}

template <typename T, bool move_only_T>
Polymorphic<T, move_only_T>::Polymorphic(
    Polymorphic&&         other,
    const allocator_type& allocator
)
    requires(is_move_only())
    : m_allocator{
          assert_allocator_upon_move_only_move_construct(allocator, other.m_allocator)
      },
      m_handle{ std::exchange(other.m_handle, nullptr) },
      m_copy_mechanism{ other.m_copy_mechanism }
{
}

template <typename T, bool move_only_T>
Polymorphic<T, move_only_T>::~Polymorphic()
{
    if (m_handle)
    {
        m_allocator.delete_object(m_handle);
    }
}

template <typename T, bool move_only_T>
template <typename U>
Polymorphic<T, move_only_T>::Polymorphic(U&& value)
    requires(
        !std::is_same_v<std::remove_cvref_t<U>, Polymorphic>
        && !util::specialization_of_c<U, std::in_place_type_t>
        && std::is_constructible_v<std::remove_cvref_t<U>, U &&>
        && storable<std::remove_cvref_t<U>>()
    )
    : Polymorphic{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          std::forward<U>(value),
      }
{
}

template <typename T, bool move_only_T>
template <typename U>
Polymorphic<T, move_only_T>::Polymorphic(
    std::allocator_arg_t,
    const allocator_type& allocator,
    U&&                   value
)
    requires(!std::is_same_v<std::remove_cvref_t<U>, Polymorphic>
             && !util::specialization_of_c<U, std::in_place_type_t>
             && std::is_constructible_v<std::remove_cvref_t<U>, U &&>
             && storable<std::remove_cvref_t<U>>())
    : m_allocator{ allocator },
      m_handle{
          m_allocator.template new_object<std::remove_cvref_t<U>>(std::forward<U>(value))
      },
      m_copy_mechanism{ std::in_place_type<U> }
{
}

template <typename T, bool move_only_T>
template <naked_c U, typename... Args_T>
Polymorphic<T, move_only_T>::Polymorphic(
    std::in_place_type_t<U> in_place_type,
    Args_T&&... args
)
    requires(std::is_constructible_v<U, Args_T && ...> && storable<U>())
    : Polymorphic{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          in_place_type,
          std::forward<Args_T>(args)...,
      }
{
}

template <typename T, bool move_only_T>
template <naked_c U, typename... Args_T>
Polymorphic<T, move_only_T>::Polymorphic(
    std::allocator_arg_t,
    const allocator_type& allocator,
    std::in_place_type_t<U>,
    Args_T&&... args
)
    requires(std::is_constructible_v<U, Args_T && ...> && storable<U>())
    : m_allocator{ allocator },
      m_handle{ m_allocator.template new_object<U>(std::forward<Args_T>(args)...) },
      m_copy_mechanism{ std::in_place_type<U> }
{
}

template <typename T, bool move_only_T>
auto Polymorphic<T, move_only_T>::operator=(const Polymorphic& other) -> Polymorphic&
    requires(!is_move_only())
{
    if (this == &other)
    {
        return *this;
    }

    other.m_copy_mechanism.assign(
        m_allocator,
        m_handle,
        other.m_handle,
        m_copy_mechanism.type_hash(),
        other.m_copy_mechanism.type_hash()
    );
    m_copy_mechanism = other.m_copy_mechanism;

    return *this;
}

template <typename T, bool move_only_T>
// NOLINTNEXTLINE(*-noexcept-move-operations,*-noexcept-move,*-noexcept-move-constructor)
auto Polymorphic<T, move_only_T>::operator=(Polymorphic&& other) -> Polymorphic&
    requires(!is_move_only())
{
    if (this == &other)
    {
        return *this;
    }

    if (m_allocator != other.m_allocator)
    {
        operator=(other);
        return *this;
    }

    swap(other);
    other.release();

    return *this;
}

template <typename T, bool move_only_T>
auto Polymorphic<T, move_only_T>::operator=(Polymorphic&& other) noexcept -> Polymorphic&
    requires(is_move_only())
{
    if (this == &other)
    {
        return *this;
    }

    PRECOND(m_allocator == other.m_allocator);

    swap(other);
    other.release();

    return *this;
}

template <typename T, bool move_only_T>
auto Polymorphic<T, move_only_T>::operator->() noexcept -> T*
{
    PRECOND(m_handle != nullptr);
    return m_handle;
}

template <typename T, bool move_only_T>
auto Polymorphic<T, move_only_T>::operator->() const noexcept -> const T*
{
    PRECOND(m_handle != nullptr);
    return m_handle;
}

template <typename T, bool move_only_T>
auto Polymorphic<T, move_only_T>::operator*() noexcept -> T&
{
    PRECOND(m_handle != nullptr);
    // ReSharper disable once CppDFANullDereference
    return *m_handle;
}

template <typename T, bool move_only_T>
auto Polymorphic<T, move_only_T>::operator*() const noexcept -> const T&
{
    PRECOND(m_handle != nullptr);
    // ReSharper disable once CppDFANullDereference
    return *m_handle;
}

template <typename T, bool move_only_T>
auto Polymorphic<T, move_only_T>::get_allocator() const noexcept -> allocator_type
{
    return m_allocator;
}

template <typename T, bool move_only_T>
// NOLINTNEXTLINE(*-noexcept-swap)
auto Polymorphic<T, move_only_T>::swap(Polymorphic& other) -> void
{
    PRECOND(m_allocator == other.m_allocator);

    std::swap(m_handle, other.m_handle);
    std::swap(m_copy_mechanism, other.m_copy_mechanism);
}

template <typename T, bool move_only_T>
auto Polymorphic<T, move_only_T>::release() -> void
{
    if (m_handle != nullptr)
    {
        m_allocator.delete_object(m_handle);
        m_handle = nullptr;
    }
}

template <typename T>
// ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
auto swap(Polymorphic<T>& lhs, Polymorphic<T>& rhs) -> void   // NOLINT(*-noexcept-swap)
{
    lhs.swap(rhs);
}

}   // namespace kiln::util
