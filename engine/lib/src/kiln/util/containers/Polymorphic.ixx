module;

#include <concepts>
#include <functional>
#include <memory_resource>
#include <type_traits>
#include <utility>

#include "kiln/util/contract_macros.hpp"
#include "kiln/util/no_unique_address.hpp"

export module kiln.util.containers.Polymorphic;

import kiln.util.concepts.decayed;
import kiln.util.concepts.specialization_of;
import kiln.util.contracts;
import kiln.util.reflection;
import kiln.util.ScopeFail;

namespace kiln::util {

template <typename T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
class EraseMechanism;

template <typename T, bool move_only_T, std::size_t alignment_T>
class EraseMechanism<T, move_only_T, 0, alignment_T> {
public:
    struct Storage {
        T* handle;
    };

    template <typename U>
    constexpr explicit EraseMechanism(std::in_place_type_t<U>) noexcept
        requires(!move_only_T);
    template <typename U>
    constexpr explicit EraseMechanism(std::in_place_type_t<U>) noexcept
        requires(move_only_T);

    [[nodiscard]]
    auto copy_construct(
        std::pmr::polymorphic_allocator<T>& allocator,
        const Storage&                      other_storage
    ) const -> Storage
        requires(!move_only_T);
    [[nodiscard]]
    constexpr static auto move_construct(Storage&& other_storage) noexcept -> Storage;
    [[nodiscard]]
    auto move_construct(
        std::pmr::polymorphic_allocator<T>&       new_allocator,
        const std::pmr::polymorphic_allocator<T>& other_allocator,
        Storage&&                                 other_storage
    ) -> Storage
        requires(!move_only_T);
    [[nodiscard]]
    static auto move_construct(
        std::pmr::polymorphic_allocator<T>&       new_allocator,
        const std::pmr::polymorphic_allocator<T>& other_allocator,
        Storage&&                                 other_storage
    ) noexcept -> Storage
        requires(move_only_T);
    static auto drop(std::pmr::polymorphic_allocator<T>& allocator, Storage&& storage)
        -> void;

    template <typename U, typename... Args_T>
    [[nodiscard]]
    static auto construct(
        std::allocator_arg_t,
        std::pmr::polymorphic_allocator<T>& allocator,
        std::in_place_type_t<U>,
        Args_T&&... args
    ) -> Storage;

    auto copy_assign(
        std::pmr::polymorphic_allocator<T>& allocator,
        Storage&                            storage,
        const Storage&                      other_storage,
        uint64_t                            other_type_hash
    ) const -> void
        requires(!move_only_T);

    [[nodiscard]]
    constexpr static auto address_of(Storage& storage) -> T*;
    [[nodiscard]]
    constexpr static auto address_of(const Storage& storage) -> const T*;
    [[nodiscard]]
    constexpr static auto dereference(Storage& storage) -> T&;
    [[nodiscard]]
    constexpr static auto dereference(const Storage& storage) -> const T&;

    [[nodiscard]]
    auto type_hash() const -> uint64_t
        requires(!move_only_T);

    constexpr static auto swap(Storage& lhs, Storage& rhs) noexcept -> void;
    static auto release(std::pmr::polymorphic_allocator<T>& allocator, Storage& storage)
        -> void;

private:
    struct VTable;

    struct EmptyVTable {};

    [[kiln_no_unique_address]]
    std::conditional_t<!move_only_T, std::reference_wrapper<const VTable>, EmptyVTable>
        m_vtable;
};

export consteval auto default_polymorphic_size() -> std::size_t
{
    return 3 * sizeof(void*);
}

export consteval auto default_polymorphic_alignment() -> std::size_t
{
    return alignof(std::max_align_t);
}

export template <
    decayed_c   T,
    bool        move_only_T = false,
    std::size_t size_T      = 0,
    std::size_t alignment_T = 0>
class Polymorphic {
public:
    using ValueType      = T;
    using allocator_type = std::pmr::polymorphic_allocator<T>;


    consteval static auto is_move_only() -> bool;
    consteval static auto size() -> std::size_t;
    consteval static auto alignment() -> std::size_t;
    template <typename U>
    consteval static auto storable() -> bool;


    Polymorphic(const Polymorphic&)
        requires(!is_move_only());
    Polymorphic(const Polymorphic&, const allocator_type& allocator)
        requires(!is_move_only());
    Polymorphic(Polymorphic&&) noexcept;
    Polymorphic(Polymorphic&&, const allocator_type& allocator);
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
    explicit Polymorphic(std::allocator_arg_t, const allocator_type& allocator, U&& value)
        requires(
            !std::is_same_v<std::remove_cvref_t<U>, Polymorphic>
            && !util::specialization_of_c<U, std::in_place_type_t>
            && std::is_constructible_v<std::remove_cvref_t<U>, U &&>
            && storable<std::remove_cvref_t<U>>()
        );

    template <decayed_c U, typename... Args_T>
    explicit Polymorphic(std::in_place_type_t<U>, Args_T&&... args)
        requires(std::is_constructible_v<U, Args_T && ...> && storable<U>());
    template <decayed_c U, typename... Args_T>
    explicit Polymorphic(
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
    // NOLINTNEXTLINE(*-noexcept-swap)
    auto swap(Polymorphic& other) -> void;

private:
    allocator_type                                               m_allocator;
    EraseMechanism<T, move_only_T, size_T, alignment_T>::Storage m_storage;
    [[kiln_no_unique_address]]
    EraseMechanism<T, move_only_T, size_T, alignment_T> m_erase_mechanism;


    auto release() -> void;
};

// ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
// NOLINTNEXTLINE(*-noexcept-swap)
export template <typename T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
auto swap(
    Polymorphic<T, move_only_T, size_T, alignment_T>& lhs,
    Polymorphic<T, move_only_T, size_T, alignment_T>& rhs
) -> void;

}   // namespace kiln::util

namespace kiln::util {

template <typename T, bool move_only_T, std::size_t alignment_T>
struct EraseMechanism<T, move_only_T, 0, alignment_T>::VTable {
    using CopyConstructFunc =
        auto(std::pmr::polymorphic_allocator<T>& allocator, const Storage& other_storage)
            -> Storage;
    using CopyAssignFunc = auto(
        std::pmr::polymorphic_allocator<T>& allocator,
        Storage&                            storage,
        const Storage&                      other_storage,
        uint64_t                            other_type_hash
    ) -> void;
    using TypeHash = auto() -> uint64_t;

    template <typename U>
    struct Operations;


    std::reference_wrapper<CopyConstructFunc> copy_construct;
    std::reference_wrapper<CopyAssignFunc>    copy_assign;
    std::reference_wrapper<TypeHash>          type_hash;
};

template <typename T, bool move_only_T, std::size_t alignment_T>
template <typename U>
struct EraseMechanism<T, move_only_T, 0, alignment_T>::VTable::Operations {
    static auto copy_construct(
        std::pmr::polymorphic_allocator<T>& allocator,
        const Storage&                      other_storage
    ) -> Storage
    {
        if (other_storage.handle == nullptr)
        {
            return Storage{ .handle = nullptr };
        }

        return Storage{
            .handle = allocator.template new_object<U>(
                static_cast<const U&>(*other_storage.handle)
            ),
        };
    }

    static auto copy_assign(
        std::pmr::polymorphic_allocator<T>& allocator,
        Storage&                            storage,
        const Storage&                      other_storage,
        const uint64_t                      other_type_hash
    ) -> void
    {
        if (storage.handle != nullptr && other_storage.handle != nullptr
            && type_hash() == other_type_hash)
        {
            static_cast<U&>(*storage.handle) =
                static_cast<const U&>(*other_storage.handle);
            return;
        }

        T* const        new_object = other_storage.handle == nullptr
                                       ? nullptr
                                       : allocator.template new_object<U>(
                                      static_cast<const U&>(*other_storage.handle)
                                  );
        const ScopeFail new_object_guard{
            [&] noexcept -> void
            {
                allocator.delete_object(new_object);   //
            }   //
        };

        if (storage.handle != nullptr)
        {
            allocator.delete_object(storage.handle);
        }

        storage.handle = new_object;
    }

    constexpr static auto type_hash() -> uint64_t
    {
        return util::hash_u64<U>();
    }

    constexpr static VTable vtable{
        .copy_construct = copy_construct,
        .copy_assign    = copy_assign,
        .type_hash      = type_hash,
    };
};

template <typename T, bool move_only_T, std::size_t alignment_T>
template <typename U>
constexpr EraseMechanism<T, move_only_T, 0, alignment_T>::EraseMechanism(
    std::in_place_type_t<U>
) noexcept
    requires(!move_only_T)
    : m_vtable{ VTable::template Operations<U>::vtable }
{
}

template <typename T, bool move_only_T, std::size_t alignment_T>
template <typename U>
constexpr EraseMechanism<T, move_only_T, 0, alignment_T>::EraseMechanism(
    std::in_place_type_t<U>
) noexcept
    requires(move_only_T)
{
}

template <typename T, bool move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, move_only_T, 0, alignment_T>::copy_construct(
    std::pmr::polymorphic_allocator<T>& allocator,
    const Storage&                      other_storage
) const -> Storage
    requires(!move_only_T)
{
    return m_vtable.get().copy_construct(allocator, other_storage);
}

template <typename T, bool move_only_T, std::size_t alignment_T>
constexpr auto EraseMechanism<T, move_only_T, 0, alignment_T>::move_construct(
    Storage&& other_storage
) noexcept -> Storage
{
    return Storage{ .handle = std::exchange(other_storage.handle, nullptr) };
}

template <typename T, bool move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, move_only_T, 0, alignment_T>::move_construct(
    std::pmr::polymorphic_allocator<T>&       new_allocator,
    const std::pmr::polymorphic_allocator<T>& other_allocator,
    Storage&&                                 other_storage
) -> Storage
    requires(!move_only_T)
{
    if (new_allocator == other_allocator)
    {
        return Storage{ .handle = std::exchange(other_storage.handle, nullptr) };
    }

    return copy_construct(new_allocator, other_storage);
}

template <typename T, bool move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, move_only_T, 0, alignment_T>::move_construct(
    std::pmr::polymorphic_allocator<T>&       new_allocator,
    const std::pmr::polymorphic_allocator<T>& other_allocator,
    Storage&&                                 other_storage
) noexcept -> Storage
    requires(move_only_T)
{
    PRECOND(new_allocator == other_allocator);
    return Storage{ .handle = std::exchange(other_storage.handle, nullptr) };
}

template <typename T, bool move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, move_only_T, 0, alignment_T>::drop(
    std::pmr::polymorphic_allocator<T>& allocator,
    Storage&&                           storage
) -> void
{
    if (storage.handle != nullptr)
    {
        allocator.delete_object(storage.handle);
    }
}

template <typename T, bool move_only_T, std::size_t alignment_T>
template <typename U, typename... Args_T>
auto EraseMechanism<T, move_only_T, 0, alignment_T>::construct(
    std::allocator_arg_t,
    std::pmr::polymorphic_allocator<T>& allocator,
    std::in_place_type_t<U>,
    Args_T&&... args
) -> Storage
{
    return Storage{
        .handle = allocator.template new_object<U>(std::forward<Args_T>(args)...),
    };
}

template <typename T, bool move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, move_only_T, 0, alignment_T>::copy_assign(
    std::pmr::polymorphic_allocator<T>& allocator,
    Storage&                            storage,
    const Storage&                      other_storage,
    const uint64_t                      other_type_hash
) const -> void
    requires(!move_only_T)
{
    m_vtable.get().copy_assign(allocator, storage, other_storage, other_type_hash);
}

template <typename T, bool move_only_T, std::size_t alignment_T>
constexpr auto EraseMechanism<T, move_only_T, 0, alignment_T>::address_of(Storage& storage)
    -> T*
{
    PRECOND(storage.handle != nullptr);
    return storage.handle;
}

template <typename T, bool move_only_T, std::size_t alignment_T>
constexpr auto EraseMechanism<T, move_only_T, 0, alignment_T>::address_of(
    const Storage& storage
) -> const T*
{
    PRECOND(storage.handle != nullptr);
    return storage.handle;
}

template <typename T, bool move_only_T, std::size_t alignment_T>
constexpr auto EraseMechanism<T, move_only_T, 0, alignment_T>::dereference(
    Storage& storage
) -> T&
{
    PRECOND(storage.handle != nullptr);
    return *storage.handle;
}

template <typename T, bool move_only_T, std::size_t alignment_T>
constexpr auto EraseMechanism<T, move_only_T, 0, alignment_T>::dereference(
    const Storage& storage
) -> const T&
{
    PRECOND(storage.handle != nullptr);
    return *storage.handle;
}

template <typename T, bool move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, move_only_T, 0, alignment_T>::type_hash() const -> uint64_t
    requires(!move_only_T)
{
    return m_vtable.get().type_hash();
}

template <typename T, bool move_only_T, std::size_t alignment_T>
constexpr auto EraseMechanism<T, move_only_T, 0, alignment_T>::swap(
    Storage& lhs,
    Storage& rhs
) noexcept -> void
{
    std::swap(lhs.handle, rhs.handle);
}

template <typename T, bool move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, move_only_T, 0, alignment_T>::release(
    std::pmr::polymorphic_allocator<T>& allocator,
    Storage&                            storage
) -> void
{
    if (storage.handle != nullptr)
    {
        allocator.delete_object(storage.handle);
        storage.handle = nullptr;
    }
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
consteval auto Polymorphic<T, move_only_T, size_T, alignment_T>::is_move_only() -> bool
{
    return move_only_T;
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
consteval auto Polymorphic<T, move_only_T, size_T, alignment_T>::size() -> std::size_t
{
    return size_T;
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
consteval auto Polymorphic<T, move_only_T, size_T, alignment_T>::alignment()
    -> std::size_t
{
    return alignment_T;
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
template <typename U>
consteval auto Polymorphic<T, move_only_T, size_T, alignment_T>::storable() -> bool
{
    return is_move_only() || std::copyable<U>;
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
Polymorphic<T, move_only_T, size_T, alignment_T>::Polymorphic(const Polymorphic& other)
    requires(!is_move_only())
    : Polymorphic{ other, other.m_allocator }
{
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
Polymorphic<T, move_only_T, size_T, alignment_T>::Polymorphic(
    const Polymorphic&    other,
    const allocator_type& allocator
)
    requires(!is_move_only())
    : m_allocator{ allocator },
      m_storage{ other.m_erase_mechanism.copy_construct(m_allocator, other.m_storage) },
      m_erase_mechanism{ other.m_erase_mechanism }
{
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
Polymorphic<T, move_only_T, size_T, alignment_T>::Polymorphic(Polymorphic&& other) noexcept
    : m_allocator{ other.m_allocator },
      m_storage{ other.m_erase_mechanism.move_construct(std::move(other.m_storage)) },
      m_erase_mechanism{ other.m_erase_mechanism }
{
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
Polymorphic<T, move_only_T, size_T, alignment_T>::Polymorphic(
    Polymorphic&&         other,
    const allocator_type& allocator
)
    : m_allocator{ allocator },
      m_storage{
          other.m_erase_mechanism.move_construct(
              m_allocator,
              other.m_allocator,
              std::move(other.m_storage)
          )   //
      },
      m_erase_mechanism{ other.m_erase_mechanism }
{
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
Polymorphic<T, move_only_T, size_T, alignment_T>::~Polymorphic()
{
    m_erase_mechanism.drop(m_allocator, std::move(m_storage));
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
template <typename U>
Polymorphic<T, move_only_T, size_T, alignment_T>::Polymorphic(U&& value)
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

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
template <typename U>
Polymorphic<T, move_only_T, size_T, alignment_T>::Polymorphic(
    std::allocator_arg_t,
    const allocator_type& allocator,
    U&&                   value
)
    requires(!std::is_same_v<std::remove_cvref_t<U>, Polymorphic>
             && !util::specialization_of_c<U, std::in_place_type_t>
             && std::is_constructible_v<std::remove_cvref_t<U>, U &&>
             && storable<std::remove_cvref_t<U>>())
    : m_allocator{ allocator },
      m_storage{
          EraseMechanism<T, move_only_T, size_T, alignment_T>::construct(
              std::allocator_arg,
              m_allocator,
              std::in_place_type<std::remove_cvref_t<U>>,
              std::forward<U>(value)
          )   //
      },
      m_erase_mechanism{ std::in_place_type<U> }
{
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
template <decayed_c U, typename... Args_T>
Polymorphic<T, move_only_T, size_T, alignment_T>::Polymorphic(
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

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
template <decayed_c U, typename... Args_T>
Polymorphic<T, move_only_T, size_T, alignment_T>::Polymorphic(
    std::allocator_arg_t,
    const allocator_type&   allocator,
    std::in_place_type_t<U> in_place_type,
    Args_T&&... args
)
    requires(std::is_constructible_v<U, Args_T && ...> && storable<U>())
    : m_allocator{ allocator },
      m_storage{
          EraseMechanism<T, move_only_T, size_T, alignment_T>::construct(
              std::allocator_arg,
              m_allocator,
              in_place_type,
              std::forward<Args_T>(args)...
          )   //
      },
      m_erase_mechanism{ std::in_place_type<U> }
{
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, move_only_T, size_T, alignment_T>::operator=(const Polymorphic& other)
    -> Polymorphic&
    requires(!is_move_only())
{
    if (this == &other)
    {
        return *this;
    }

    other.m_erase_mechanism.copy_assign(
        m_allocator, m_storage, other.m_storage, other.m_erase_mechanism.type_hash()
    );
    m_erase_mechanism = other.m_erase_mechanism;

    return *this;
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
// NOLINTNEXTLINE(*-noexcept-move-operations,*-noexcept-move,*-noexcept-move-constructor)
auto Polymorphic<T, move_only_T, size_T, alignment_T>::operator=(Polymorphic&& other)
    -> Polymorphic&
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

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, move_only_T, size_T, alignment_T>::operator=(
    Polymorphic&& other
) noexcept -> Polymorphic&
    requires(is_move_only())
{
    if (this == &other)
    {
        return *this;
    }

    swap(other);
    other.release();

    return *this;
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, move_only_T, size_T, alignment_T>::operator->() noexcept -> T*
{
    return m_erase_mechanism.address_of(m_storage);
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, move_only_T, size_T, alignment_T>::operator->() const noexcept
    -> const T*
{
    return m_erase_mechanism.address_of(m_storage);
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, move_only_T, size_T, alignment_T>::operator*() noexcept -> T&
{
    return m_erase_mechanism.dereference(m_storage);
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, move_only_T, size_T, alignment_T>::operator*() const noexcept
    -> const T&
{
    return m_erase_mechanism.dereference(m_storage);
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, move_only_T, size_T, alignment_T>::get_allocator() const noexcept
    -> allocator_type
{
    return m_allocator;
}

// NOLINTNEXTLINE(*-noexcept-swap)
template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, move_only_T, size_T, alignment_T>::swap(Polymorphic& other) -> void
{
    PRECOND(m_allocator == other.m_allocator);

    m_erase_mechanism.swap(m_storage, other.m_storage);
    std::swap(m_erase_mechanism, other.m_erase_mechanism);
}

template <decayed_c T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, move_only_T, size_T, alignment_T>::release() -> void
{
    m_erase_mechanism.release(m_allocator, m_storage);
}

// ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
// NOLINTNEXTLINE(*-noexcept-swap)
template <typename T, bool move_only_T, std::size_t size_T, std::size_t alignment_T>
auto swap(
    Polymorphic<T, move_only_T, size_T, alignment_T>& lhs,
    Polymorphic<T, move_only_T, size_T, alignment_T>& rhs
) -> void
{
    lhs.swap(rhs);
}

}   // namespace kiln::util
