module;

#include <array>
#include <concepts>
#include <functional>
#include <memory_resource>
#include <type_traits>
#include <utility>
#include <variant>

#include "kiln/util/contract_macros.hpp"
#include "kiln/util/no_unique_address.hpp"

export module kiln.util.containers.Polymorphic;

import kiln.util.concepts.decayed;
import kiln.util.concepts.nothrow_movable;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.storable;
import kiln.util.contracts;
import kiln.util.reflection;
import kiln.util.ScopeFail;

namespace kiln::util {

export consteval auto default_polymorphic_size() -> std::size_t
{
    return 3 * sizeof(void*);
}

export consteval auto default_polymorphic_alignment() -> std::size_t
{
    return alignof(std::max_align_t);
}

export template <
    std::destructible T,
    bool              is_move_only_T = false,
    std::size_t       size_T         = default_polymorphic_size(),
    std::size_t       alignment_T    = default_polymorphic_alignment()>
class Polymorphic;

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
class EraseMechanism;

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
class EraseMechanism<T, is_move_only_T, size_T, alignment_T> {
public:
    union Storage {
        T* handle;
        alignas(alignment_T) std::array<std::byte, size_T> small_buffer;
    };

    template <typename U>
    constexpr explicit EraseMechanism(std::in_place_type_t<U>) noexcept;

    [[nodiscard]]
    auto copy_construct(
        std::pmr::polymorphic_allocator<T>& allocator,
        const Storage&                      source_storage
    ) const -> Storage
        requires(!is_move_only_T);
    [[nodiscard]]
    constexpr auto move_construct(Storage&& source_storage) const noexcept -> Storage;
    [[nodiscard]]
    auto move_construct(
        std::pmr::polymorphic_allocator<T>&       destination_allocator,
        const std::pmr::polymorphic_allocator<T>& source_allocator,
        Storage&&                                 source_storage
    ) const -> Storage;
    constexpr auto move_construct_at(
        Storage&  destination_storage,
        Storage&& source_storage
    ) const noexcept -> void;
    auto move_construct_at(
        std::pmr::polymorphic_allocator<T>&       destination_allocator,
        Storage&                                  destination_storage,
        const std::pmr::polymorphic_allocator<T>& source_allocator,
        Storage&&                                 source_storage
    ) const -> void;
    auto drop(std::pmr::polymorphic_allocator<T>& allocator, Storage& storage) const
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
        std::pmr::polymorphic_allocator<T>& destination_allocator,
        Storage&                            destination_storage,
        const EraseMechanism&               destination_erase_mechanism,
        const Storage&                      source_storage
    ) const -> void
        requires(!is_move_only_T);

    [[nodiscard]]
    constexpr auto address_of(Storage& storage) const -> T*;
    [[nodiscard]]
    constexpr auto address_of(const Storage& storage) const -> const T*;
    [[nodiscard]]
    constexpr auto dereference(Storage& storage) const -> T&;
    [[nodiscard]]
    constexpr auto dereference(const Storage& storage) const -> const T&;

    [[nodiscard]]
    auto type_hash() const -> uint64_t;
    [[nodiscard]]
    auto uses_small_buffer() const noexcept -> bool;

    constexpr auto swap(
        std::pmr::polymorphic_allocator<T>& lhs_allocator,
        Storage&                            lhs_storage,
        std::pmr::polymorphic_allocator<T>& rhs_allocator,
        Storage&                            rhs_storage,
        const EraseMechanism&               rhs_erase_mechanism
    ) const -> void;
    auto release(std::pmr::polymorphic_allocator<T>& allocator, Storage& storage) const
        -> void;

private:
    struct VTable;

    std::reference_wrapper<const VTable> m_vtable;
};

template <typename T, bool is_move_only_T, std::size_t alignment_T>
class EraseMechanism<T, is_move_only_T, 0, alignment_T> {
public:
    struct Storage {
        T* handle;
    };

    template <typename U>
    constexpr explicit EraseMechanism(std::in_place_type_t<U>) noexcept
        requires(!is_move_only_T);
    template <typename U>
    constexpr explicit EraseMechanism(std::in_place_type_t<U>) noexcept
        requires(is_move_only_T);

    [[nodiscard]]
    auto copy_construct(
        std::pmr::polymorphic_allocator<T>& allocator,
        const Storage&                      source_storage
    ) const -> Storage
        requires(!is_move_only_T);
    [[nodiscard]]
    constexpr static auto move_construct(Storage&& source_storage) noexcept -> Storage;
    [[nodiscard]]
    auto move_construct(
        std::pmr::polymorphic_allocator<T>&       destination_allocator,
        const std::pmr::polymorphic_allocator<T>& source_allocator,
        Storage&&                                 source_storage
    ) -> Storage
        requires(!is_move_only_T);
    [[nodiscard]]
    static auto move_construct(
        std::pmr::polymorphic_allocator<T>&       destination_allocator,
        const std::pmr::polymorphic_allocator<T>& source_allocator,
        Storage&&                                 source_storage
    ) noexcept -> Storage
        requires(is_move_only_T);
    static auto drop(std::pmr::polymorphic_allocator<T>& allocator, Storage& storage)
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
        std::pmr::polymorphic_allocator<T>& destination_allocator,
        Storage&                            destination_storage,
        const EraseMechanism&               destination_erase_mechanism,
        const Storage&                      source_storage
    ) const -> void
        requires(!is_move_only_T);

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
        requires(!is_move_only_T);

    constexpr static auto swap(
        std::pmr::polymorphic_allocator<T>& lhs_allocator,
        Storage&                            lhs_storage,
        std::pmr::polymorphic_allocator<T>& rhs_allocator,
        Storage&                            rhs_storage,
        const EraseMechanism&               rhs_erase_mechanism
    ) -> void;
    static auto release(std::pmr::polymorphic_allocator<T>& allocator, Storage& storage)
        -> void;

private:
    struct VTable;

    struct EmptyVTable {};

    [[kiln_no_unique_address]]
    std::conditional_t<!is_move_only_T, std::reference_wrapper<const VTable>, EmptyVTable>
        m_vtable;
};

export template <
    std::destructible T,
    bool              is_move_only_T,
    std::size_t       size_T,
    std::size_t       alignment_T>
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
            && !util::specialization_of_c<std::remove_cvref_t<U>, std::in_place_type_t>
            && std::is_constructible_v<std::remove_cvref_t<U>, U &&>
            && storable<std::remove_cvref_t<U>>()
        );
    template <typename U>
    explicit Polymorphic(std::allocator_arg_t, const allocator_type& allocator, U&& value)
        requires(
            !std::is_same_v<std::remove_cvref_t<U>, Polymorphic>
            && !util::specialization_of_c<std::remove_cvref_t<U>, std::in_place_type_t>
            && std::is_constructible_v<std::remove_cvref_t<U>, U &&>
            && storable<std::remove_cvref_t<U>>()
        );

    template <typename U, typename... Args_T>
    explicit Polymorphic(std::in_place_type_t<U>, Args_T&&... args)
        requires(std::is_constructible_v<U, Args_T && ...> && storable<U>());
    template <typename U, typename... Args_T>
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
    allocator_type                                                  m_allocator;
    EraseMechanism<T, is_move_only_T, size_T, alignment_T>::Storage m_storage;
    [[kiln_no_unique_address]]
    EraseMechanism<T, is_move_only_T, size_T, alignment_T> m_erase_mechanism;


    auto release() -> void;
};

// ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
// NOLINTNEXTLINE(*-noexcept-swap)
export template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
auto swap(
    Polymorphic<T, is_move_only_T, size_T, alignment_T>& lhs,
    Polymorphic<T, is_move_only_T, size_T, alignment_T>& rhs
) -> void;

}   // namespace kiln::util

namespace kiln::util {

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
struct EraseMechanism<T, is_move_only_T, size_T, alignment_T>::VTable {
    using CopyConstructAtFunc = auto(
        std::pmr::polymorphic_allocator<T>& allocator,
        Storage&                            destination_storage,
        const Storage&                      source_storage
    ) -> void;
    using MoveConstructAtFunc =
        auto(Storage& destination_storage, Storage&& source_storage) -> void;
    using MoveConstructAtUsingAllocatorFunc = auto(
        std::pmr::polymorphic_allocator<T>&       destination_allocator,
        Storage&                                  destination_storage,
        const std::pmr::polymorphic_allocator<T>& source_allocator,
        Storage&&                                 source_storage
    ) -> void;
    using DropFunc = auto(std::pmr::polymorphic_allocator<T>& allocator, Storage& storage)
        -> void;
    using CopyAssignFunc = auto(
        std::pmr::polymorphic_allocator<T>& allocator,
        Storage&                            destination_storage,
        const EraseMechanism&               destination_erase_mechanism,
        const Storage&                      source_storage
    ) -> void;
    using AddressOfFunc        = auto(Storage&) -> T*;
    using ConstAddressOfFunc   = auto(const Storage&) -> const T*;
    using DereferenceFunc      = auto(Storage&) -> T&;
    using ConstDereferenceFunc = auto(const Storage&) -> const T&;
    using TypeHashFunc         = auto() -> uint64_t;
    using UsesSmallBufferFunc  = auto() -> bool;
    using SwapFunc             = auto(
        std::pmr::polymorphic_allocator<T>& lhs_allocator,
        Storage&                            lhs_storage,
        std::pmr::polymorphic_allocator<T>& rhs_allocator,
        Storage&                            rhs_storage,
        const EraseMechanism&               rhs_erase_mechanism
    ) -> void;
    using ReleaseFunc =
        auto(std::pmr::polymorphic_allocator<T>& allocator, Storage& storage) -> void;

    template <typename U>
    struct Operations;


    std::add_pointer_t<CopyConstructAtFunc>     copy_construct_at;
    std::reference_wrapper<MoveConstructAtFunc> move_construct_at;
    std::reference_wrapper<MoveConstructAtUsingAllocatorFunc>
                                                 move_construct_at_using_allocator;
    std::reference_wrapper<DropFunc>             drop;
    std::add_pointer_t<CopyAssignFunc>           copy_assign;
    std::reference_wrapper<AddressOfFunc>        address_of;
    std::reference_wrapper<ConstAddressOfFunc>   const_address_of;
    std::reference_wrapper<DereferenceFunc>      dereference;
    std::reference_wrapper<ConstDereferenceFunc> const_dereference;
    std::reference_wrapper<TypeHashFunc>         type_hash;
    std::reference_wrapper<UsesSmallBufferFunc>  uses_small_buffer;
    std::reference_wrapper<SwapFunc>             swap;
    std::reference_wrapper<ReleaseFunc>          release;
};

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
template <typename U>
struct EraseMechanism<T, is_move_only_T, size_T, alignment_T>::VTable::Operations {
    constexpr static auto uses_small_buffer() noexcept -> bool
    {
        return sizeof(U) <= size_T && alignment_T % alignof(U) == 0
            && nothrow_movable_c<U>;
    }

    template <typename... Args_T>
    static auto create_at(
        std::pmr::polymorphic_allocator<T>& allocator,
        Storage&                            storage,
        Args_T&&... args
    ) -> void
    {
        if constexpr (uses_small_buffer())
        {
            allocator.construct(
                static_cast<U*>(storage.small_buffer.data()), std::forward<Args_T>(args)...
            );
        }
        else
        {
            storage.handle =
                allocator.template new_object<U>(std::forward<Args_T>(args)...);
        }
    }

    static auto copy_construct_at(
        std::pmr::polymorphic_allocator<T>& allocator,
        Storage&                            destination_storage,
        const Storage&                      source_storage
    ) -> void
    {
        if constexpr (uses_small_buffer())
        {
            create_at(
                allocator,
                destination_storage,
                *static_cast<const U*>(source_storage.small_buffer.data())
            );
        }
        else
        {
            if (source_storage.handle == nullptr)
            {
                destination_storage.handle = nullptr;
                return;
            }

            create_at(
                allocator,
                destination_storage,
                static_cast<const U&>(*source_storage.handle)
            );
        }
    }

    constexpr static auto
        move_construct_at(Storage& destination_storage, Storage&& source_storage) -> void
    {
        if constexpr (uses_small_buffer())
        {
            std::construct_at(
                static_cast<U*>(destination_storage.small_buffer.data()),
                std::move(*static_cast<U*>(source_storage.small_buffer.data()))
            );
        }
        else
        {
            destination_storage.handle = std::exchange(source_storage.handle, nullptr);
        }
    }

    constexpr static auto move_construct_at_using_allocator(
        std::pmr::polymorphic_allocator<T>&       destination_allocator,
        Storage&                                  destination_storage,
        const std::pmr::polymorphic_allocator<T>& source_allocator,
        Storage&&                                 source_storage
    ) -> void
    {
        if constexpr (uses_small_buffer())
        {
            destination_allocator.construct(
                static_cast<U*>(destination_storage.small_buffer.data()),
                std::move(*static_cast<U*>(source_storage.small_buffer.data()))
            );
        }
        else
        {
            if constexpr (is_move_only_T)
            {
                PRECOND(destination_allocator == source_allocator);
                destination_storage.handle = std::exchange(source_storage.handle, nullptr);
            }
            else
            {
                if (destination_allocator == source_allocator)
                {
                    destination_storage.handle =
                        std::exchange(source_storage.handle, nullptr);
                }
                else
                {
                    copy_construct_at(
                        destination_allocator, destination_storage, source_storage
                    );
                }
            }
        }
    }

    constexpr static auto
        drop(std::pmr::polymorphic_allocator<T>& allocator, Storage& storage)
    {
        if constexpr (uses_small_buffer())
        {
            std::destroy_at(static_cast<U*>(storage.small_buffer.data()));
        }
        else
        {
            if (storage.handle != nullptr)
            {
                allocator.delete_object(static_cast<U*>(storage.handle));
            }
        }
    }

    static auto copy_assign(
        std::pmr::polymorphic_allocator<T>& destination_allocator,
        Storage&                            destination_storage,
        const EraseMechanism&               destination_erase_mechanism,
        const Storage&                      source_storage
    ) -> void
    {
        if constexpr (uses_small_buffer())
        {
            if (destination_erase_mechanism.type_hash() == type_hash())
            {
                *static_cast<U*>(destination_storage.small_buffer.data()) =
                    *static_cast<U*>(source_storage.small_buffer.data());
                return;
            }

            U new_object{ *static_cast<U*>(source_storage.small_buffer.data()) };

            destination_erase_mechanism.drop(destination_allocator, destination_storage);

            destination_allocator.construct(
                static_cast<U*>(destination_storage.small_buffer.data()), new_object
            );
        }
        else
        {
            if (destination_erase_mechanism.type_hash() == type_hash()
                && destination_storage.handle != nullptr
                && source_storage.handle != nullptr)
            {
                static_cast<U&>(*destination_storage.handle) =
                    static_cast<const U&>(*source_storage.handle);
                return;
            }

            T* const        new_object = source_storage.handle == nullptr
                                           ? nullptr
                                           : destination_allocator.template new_object<U>(
                                          static_cast<const U&>(*source_storage.handle)
                                      );
            const ScopeFail new_object_guard{
                [&] noexcept -> void
                {
                    if (new_object != nullptr)
                    {
                        destination_allocator.delete_object(new_object);
                    }
                }   //
            };

            destination_erase_mechanism.drop(destination_allocator, destination_storage);

            destination_storage.handle = new_object;
        }
    }

    constexpr static auto address_of(Storage& storage) -> T*
    {
        if constexpr (uses_small_buffer())
        {
            return static_cast<T*>(storage.small_buffer.data());
        }
        else
        {
            return storage.handle;
        }
    }

    constexpr static auto const_address_of(const Storage& storage) -> const T*
    {
        if constexpr (uses_small_buffer())
        {
            return static_cast<const T*>(storage.small_buffer.data());
        }
        else
        {
            return storage.handle;
        }
    }

    constexpr static auto dereference(Storage& storage) -> T&
    {
        if constexpr (uses_small_buffer())
        {
            return *static_cast<T*>(storage.small_buffer.data());
        }
        else
        {
            PRECOND(storage.handle != nullptr);
            return *storage.handle;
        }
    }

    constexpr static auto const_dereference(const Storage& storage) -> const T&
    {
        if constexpr (uses_small_buffer())
        {
            return *static_cast<const T*>(storage.small_buffer.data());
        }
        else
        {
            PRECOND(storage.handle != nullptr);
            return *storage.handle;
        }
    }

    constexpr static auto type_hash() -> uint64_t
    {
        return util::hash_u64<U>();
    }

    constexpr static auto swap(
        std::pmr::polymorphic_allocator<T>& lhs_allocator,
        Storage&                            lhs_storage,
        std::pmr::polymorphic_allocator<T>& rhs_allocator,
        Storage&                            rhs_storage,
        const EraseMechanism&               rhs_erase_mechanism
    ) -> void
    {
        if constexpr (uses_small_buffer())
        {
            if (type_hash() == rhs_erase_mechanism.type_hash())
            {
                std::swap(
                    *static_cast<U*>(lhs_storage.small_buffer.data()),
                    *static_cast<U*>(rhs_storage.small_buffer.data())
                );
            }
            else
            {
                PRECOND(
                    rhs_erase_mechanism.uses_small_buffer()
                    || lhs_allocator == rhs_allocator
                );
                Storage tmp;
                rhs_erase_mechanism.move_construct_at(
                    lhs_allocator, tmp, rhs_allocator, std::move(rhs_storage)
                );
                rhs_erase_mechanism.drop(rhs_allocator, rhs_storage);
                const ScopeFail tmp_guard{
                    [&] noexcept -> void { rhs_erase_mechanism.drop(lhs_allocator, tmp); }
                };

                std::construct_at(
                    static_cast<U*>(rhs_storage.small_buffer.data()),
                    std::move(*static_cast<U*>(lhs_storage.small_buffer.data()))
                );

                rhs_erase_mechanism.move_construct_at(lhs_storage, std::move(tmp));
                rhs_erase_mechanism.drop(lhs_allocator, tmp);
            }
        }
        else
        {
            PRECOND(lhs_allocator == rhs_allocator);
            if (type_hash() == rhs_erase_mechanism.type_hash())
            {
                std::swap(lhs_storage.handle, rhs_storage.handle);
            }
            else
            {
                T* tmp{ lhs_storage.handle };

                rhs_erase_mechanism.move_construct_at(
                    lhs_allocator, lhs_storage, rhs_allocator, std::move(rhs_storage)
                );
                rhs_erase_mechanism.drop(rhs_allocator, rhs_storage);

                rhs_storage.handle = tmp;
            }
        }
    }

    constexpr static auto
        release(std::pmr::polymorphic_allocator<T>& allocator, Storage& storage) -> void
    {
        /*
         * Small buffer release is no-op. It will destroy at drop.
         */

        if constexpr (!uses_small_buffer())
        {
            if (storage.handle != nullptr)
            {
                allocator.delete_object(static_cast<U*>(storage.handle));
                storage.handle = nullptr;
            }
        }
    }

    constexpr static VTable vtable{
        .copy_construct_at = [] -> auto
        {
            if constexpr (is_move_only_T)
            {
                return nullptr;
            }
            else
            {
                return copy_construct_at;
            }
        }(),
        .move_construct_at                 = move_construct_at,
        .move_construct_at_using_allocator = move_construct_at_using_allocator,
        .drop                              = drop,
        .copy_assign                       = [] -> auto
        {
            if constexpr (is_move_only_T)
            {
                return nullptr;
            }
            else
            {
                return copy_assign;
            }
        }(),
        .address_of        = address_of,
        .const_address_of  = const_address_of,
        .dereference       = dereference,
        .const_dereference = const_dereference,
        .type_hash         = type_hash,
        .uses_small_buffer = uses_small_buffer,
        .swap              = swap,
        .release           = release,
    };
};

template <typename T, bool is_move_only_T, std::size_t alignment_T>
struct EraseMechanism<T, is_move_only_T, 0, alignment_T>::VTable {
    using CopyConstructFunc =
        auto(std::pmr::polymorphic_allocator<T>& allocator, const Storage& source_storage)
            -> Storage;
    using CopyAssignFunc = auto(
        std::pmr::polymorphic_allocator<T>& allocator,
        Storage&                            destination_storage,
        const EraseMechanism&               destination_erase_mechanism,
        const Storage&                      source_storage
    ) -> void;
    using TypeHash = auto() -> uint64_t;

    template <typename U>
    struct Operations;


    std::reference_wrapper<CopyConstructFunc> copy_construct;
    std::reference_wrapper<CopyAssignFunc>    copy_assign;
    std::reference_wrapper<TypeHash>          type_hash;
};

template <typename T, bool is_move_only_T, std::size_t alignment_T>
template <typename U>
struct EraseMechanism<T, is_move_only_T, 0, alignment_T>::VTable::Operations {
    static auto copy_construct(
        std::pmr::polymorphic_allocator<T>& allocator,
        const Storage&                      source_storage
    ) -> Storage
    {
        Storage result;


        if (source_storage.handle == nullptr)
        {
            result.handle = nullptr;
            return result;
        }

        result.handle = allocator.template new_object<U>(
            static_cast<const U&>(*source_storage.handle)
        );
        return result;
    }

    static auto copy_assign(
        std::pmr::polymorphic_allocator<T>& destination_allocator,
        Storage&                            destination_storage,
        const EraseMechanism&               destination_erase_mechanism,
        const Storage&                      source_storage
    ) -> void
    {
        if (destination_erase_mechanism.type_hash() == type_hash()
            && destination_storage.handle != nullptr && source_storage.handle != nullptr)
        {
            static_cast<U&>(*destination_storage.handle) =
                static_cast<const U&>(*source_storage.handle);
            return;
        }

        T* const        new_object = source_storage.handle == nullptr
                                       ? nullptr
                                       : destination_allocator.template new_object<U>(
                                      static_cast<const U&>(*source_storage.handle)
                                  );
        const ScopeFail new_object_guard{
            [&] noexcept -> void
            {
                destination_allocator.delete_object(new_object);   //
            }   //
        };

        if (destination_storage.handle != nullptr)
        {
            destination_allocator.delete_object(destination_storage.handle);
        }

        destination_storage.handle = new_object;
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

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
template <typename U>
constexpr EraseMechanism<T, is_move_only_T, size_T, alignment_T>::EraseMechanism(
    std::in_place_type_t<U>
) noexcept
    : m_vtable{ VTable::template Operations<U>::vtable }
{
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
constexpr auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::move_construct(
    Storage&& source_storage
) const noexcept -> Storage
{
    Storage result;
    move_construct_at(result, std::move(source_storage));
    return result;
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::copy_construct(
    std::pmr::polymorphic_allocator<T>& allocator,
    const Storage&                      source_storage
) const -> Storage
    requires(!is_move_only_T)
{
    Storage result;
    m_vtable.get().copy_construct_at(allocator, result, source_storage);
    return result;
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::move_construct(
    std::pmr::polymorphic_allocator<T>&       destination_allocator,
    const std::pmr::polymorphic_allocator<T>& source_allocator,
    Storage&&                                 source_storage
) const -> Storage
{
    Storage result;
    move_construct_at(
        destination_allocator, result, source_allocator, std::move(source_storage)
    );
    return result;
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
constexpr auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::move_construct_at(
    Storage&  destination_storage,
    Storage&& source_storage
) const noexcept -> void
{
    return m_vtable.get().move_construct_at(
        destination_storage, std::move(source_storage)
    );
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::move_construct_at(
    std::pmr::polymorphic_allocator<T>&       destination_allocator,
    Storage&                                  destination_storage,
    const std::pmr::polymorphic_allocator<T>& source_allocator,
    Storage&&                                 source_storage
) const -> void
{
    m_vtable.get().move_construct_at_using_allocator(
        destination_allocator,
        destination_storage,
        source_allocator,
        std::move(source_storage)
    );
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::drop(
    std::pmr::polymorphic_allocator<T>& allocator,
    Storage&                            storage
) const -> void
{
    m_vtable.get().drop(allocator, storage);
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
template <typename U, typename... Args_T>
auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::construct(
    std::allocator_arg_t,
    std::pmr::polymorphic_allocator<T>& allocator,
    std::in_place_type_t<U>,
    Args_T&&... args
) -> Storage
{
    Storage result;
    VTable::template Operations<U>::create_at(
        allocator, result, std::forward<Args_T>(args)...
    );
    return result;
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::copy_assign(
    std::pmr::polymorphic_allocator<T>& destination_allocator,
    Storage&                            destination_storage,
    const EraseMechanism&               destination_erase_mechanism,
    const Storage&                      source_storage
) const -> void
    requires(!is_move_only_T)
{
    return m_vtable.get().copy_assign(
        destination_allocator,
        destination_storage,
        destination_erase_mechanism,
        source_storage
    );
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
constexpr auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::address_of(
    Storage& storage
) const -> T*
{
    return m_vtable.get().address_of(storage);
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
constexpr auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::address_of(
    const Storage& storage
) const -> const T*
{
    return m_vtable.get().const_address_of(storage);
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
constexpr auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::dereference(
    Storage& storage
) const -> T&
{
    return m_vtable.get().dereference(storage);
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
constexpr auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::dereference(
    const Storage& storage
) const -> const T&
{
    return m_vtable.get().const_dereference(storage);
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::type_hash() const -> uint64_t
{
    return m_vtable.get().type_hash();
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::
    uses_small_buffer() const noexcept -> bool
{
    return m_vtable.get().uses_small_buffer();
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
constexpr auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::swap(
    std::pmr::polymorphic_allocator<T>& lhs_allocator,
    Storage&                            lhs_storage,
    std::pmr::polymorphic_allocator<T>& rhs_allocator,
    Storage&                            rhs_storage,
    const EraseMechanism&               rhs_erase_mechanism
) const -> void
{
    m_vtable.get().swap(
        lhs_allocator, lhs_storage, rhs_allocator, rhs_storage, rhs_erase_mechanism
    );
}

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
    requires(size_T != 0)
auto EraseMechanism<T, is_move_only_T, size_T, alignment_T>::release(
    std::pmr::polymorphic_allocator<T>& allocator,
    Storage&                            storage
) const -> void
{
    m_vtable.get().release(allocator, storage);
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
template <typename U>
constexpr EraseMechanism<T, is_move_only_T, 0, alignment_T>::EraseMechanism(
    std::in_place_type_t<U>
) noexcept
    requires(!is_move_only_T)
    : m_vtable{ VTable::template Operations<U>::vtable }
{
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
template <typename U>
constexpr EraseMechanism<T, is_move_only_T, 0, alignment_T>::EraseMechanism(
    std::in_place_type_t<U>
) noexcept
    requires(is_move_only_T)
{
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::copy_construct(
    std::pmr::polymorphic_allocator<T>& allocator,
    const Storage&                      source_storage
) const -> Storage
    requires(!is_move_only_T)
{
    return m_vtable.get().copy_construct(allocator, source_storage);
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
constexpr auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::move_construct(
    Storage&& source_storage
) noexcept -> Storage
{
    return Storage{ .handle = std::exchange(source_storage.handle, nullptr) };
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::move_construct(
    std::pmr::polymorphic_allocator<T>&       destination_allocator,
    const std::pmr::polymorphic_allocator<T>& source_allocator,
    Storage&&                                 source_storage
) -> Storage
    requires(!is_move_only_T)
{
    if (destination_allocator == source_allocator)
    {
        return Storage{ .handle = std::exchange(source_storage.handle, nullptr) };
    }

    return copy_construct(destination_allocator, source_storage);
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::move_construct(
    [[maybe_unused]] std::pmr::polymorphic_allocator<T>&       destination_allocator,
    [[maybe_unused]] const std::pmr::polymorphic_allocator<T>& source_allocator,
    Storage&&                                                  source_storage
) noexcept -> Storage
    requires(is_move_only_T)
{
    PRECOND(destination_allocator == source_allocator);
    return Storage{ .handle = std::exchange(source_storage.handle, nullptr) };
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::drop(
    std::pmr::polymorphic_allocator<T>& allocator,
    Storage&                            storage
) -> void
{
    if (storage.handle != nullptr)
    {
        allocator.delete_object(storage.handle);
    }
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
template <typename U, typename... Args_T>
auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::construct(
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

template <typename T, bool is_move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::copy_assign(
    std::pmr::polymorphic_allocator<T>& destination_allocator,
    Storage&                            destination_storage,
    const EraseMechanism&               destination_erase_mechanism,
    const Storage&                      source_storage
) const -> void
    requires(!is_move_only_T)
{
    m_vtable.get().copy_assign(
        destination_allocator,
        destination_storage,
        destination_erase_mechanism,
        source_storage
    );
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
constexpr auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::address_of(
    Storage& storage
) -> T*
{
    PRECOND(storage.handle != nullptr);
    return storage.handle;
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
constexpr auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::address_of(
    const Storage& storage
) -> const T*
{
    PRECOND(storage.handle != nullptr);
    return storage.handle;
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
constexpr auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::dereference(
    Storage& storage
) -> T&
{
    PRECOND(storage.handle != nullptr);
    return *storage.handle;
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
constexpr auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::dereference(
    const Storage& storage
) -> const T&
{
    PRECOND(storage.handle != nullptr);
    return *storage.handle;
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::type_hash() const -> uint64_t
    requires(!is_move_only_T)
{
    return m_vtable.get().type_hash();
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
constexpr auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::swap(
    [[maybe_unused]] std::pmr::polymorphic_allocator<T>& lhs_allocator,
    Storage&                                             lhs_storage,
    [[maybe_unused]] std::pmr::polymorphic_allocator<T>& rhs_allocator,
    Storage&                                             rhs_storage,
    const EraseMechanism&
) -> void
{
    PRECOND(lhs_allocator == rhs_allocator);
    std::swap(lhs_storage.handle, rhs_storage.handle);
}

template <typename T, bool is_move_only_T, std::size_t alignment_T>
auto EraseMechanism<T, is_move_only_T, 0, alignment_T>::release(
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

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
consteval auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::is_move_only() -> bool
{
    return is_move_only_T;
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
consteval auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::size() -> std::size_t
{
    return size_T;
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
consteval auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::alignment()
    -> std::size_t
{
    return alignment_T;
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
template <typename U>
consteval auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::storable() -> bool
{
    return std::derived_from<U, T> && storable_c<U>
        && (is_move_only() || std::copyable<U>);
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
Polymorphic<T, is_move_only_T, size_T, alignment_T>::Polymorphic(const Polymorphic& other)
    requires(!is_move_only())
    : Polymorphic{ other, other.m_allocator }
{
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
Polymorphic<T, is_move_only_T, size_T, alignment_T>::Polymorphic(
    const Polymorphic&    other,
    const allocator_type& allocator
)
    requires(!is_move_only())
    : m_allocator{ allocator },
      m_storage{ other.m_erase_mechanism.copy_construct(m_allocator, other.m_storage) },
      m_erase_mechanism{ other.m_erase_mechanism }
{
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
Polymorphic<T, is_move_only_T, size_T, alignment_T>::Polymorphic(
    Polymorphic&& other
) noexcept
    : m_allocator{ other.m_allocator },
      m_storage{ other.m_erase_mechanism.move_construct(std::move(other.m_storage)) },
      m_erase_mechanism{ other.m_erase_mechanism }
{
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
Polymorphic<T, is_move_only_T, size_T, alignment_T>::Polymorphic(
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

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
Polymorphic<T, is_move_only_T, size_T, alignment_T>::~Polymorphic()
{
    m_erase_mechanism.drop(m_allocator, m_storage);
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
template <typename U>
Polymorphic<T, is_move_only_T, size_T, alignment_T>::Polymorphic(U&& value)
    requires(
        !std::is_same_v<std::remove_cvref_t<U>, Polymorphic>
        && !util::specialization_of_c<std::remove_cvref_t<U>, std::in_place_type_t>
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

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
template <typename U>
Polymorphic<T, is_move_only_T, size_T, alignment_T>::Polymorphic(
    std::allocator_arg_t,
    const allocator_type& allocator,
    U&&                   value
)
    requires(
        !std::is_same_v<std::remove_cvref_t<U>, Polymorphic>
        && !util::specialization_of_c<std::remove_cvref_t<U>, std::in_place_type_t>
        && std::is_constructible_v<std::remove_cvref_t<U>, U &&>
        && storable<std::remove_cvref_t<U>>()
    )
    : Polymorphic{
          std::allocator_arg,
          allocator,
          std::in_place_type<std::remove_cvref_t<U>>,
          std::forward<U>(value),
      }
{
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
template <typename U, typename... Args_T>
Polymorphic<T, is_move_only_T, size_T, alignment_T>::Polymorphic(
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

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
template <typename U, typename... Args_T>
Polymorphic<T, is_move_only_T, size_T, alignment_T>::Polymorphic(
    std::allocator_arg_t,
    const allocator_type&   allocator,
    std::in_place_type_t<U> in_place_type,
    Args_T&&... args
)
    requires(std::is_constructible_v<U, Args_T && ...> && storable<U>())
    : m_allocator{ allocator },
      m_storage{
          EraseMechanism<T, is_move_only_T, size_T, alignment_T>::construct(
              std::allocator_arg,
              m_allocator,
              in_place_type,
              std::forward<Args_T>(args)...
          )   //
      },
      m_erase_mechanism{ std::in_place_type<U> }
{
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::operator=(
    const Polymorphic& other
) -> Polymorphic&
    requires(!is_move_only())
{
    if (this == &other)
    {
        return *this;
    }

    other.m_erase_mechanism.copy_assign(
        m_allocator, m_storage, m_erase_mechanism, other.m_storage
    );
    m_erase_mechanism = other.m_erase_mechanism;

    return *this;
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
// NOLINTNEXTLINE(*-noexcept-move-operations,*-noexcept-move,*-noexcept-move-constructor)
auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::operator=(Polymorphic&& other)
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

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::operator=(
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

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::operator->() noexcept -> T*
{
    return m_erase_mechanism.address_of(m_storage);
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::operator->() const noexcept
    -> const T*
{
    return m_erase_mechanism.address_of(m_storage);
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::operator*() noexcept -> T&
{
    return m_erase_mechanism.dereference(m_storage);
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::operator*() const noexcept
    -> const T&
{
    return m_erase_mechanism.dereference(m_storage);
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::get_allocator() const noexcept
    -> allocator_type
{
    return m_allocator;
}

// NOLINTNEXTLINE(*-noexcept-swap)
template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::swap(Polymorphic& other) -> void
{
    m_erase_mechanism.swap(
        m_allocator, m_storage, other.m_allocator, other.m_storage, other.m_erase_mechanism
    );
    std::swap(m_erase_mechanism, other.m_erase_mechanism);
}

template <std::destructible T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
auto Polymorphic<T, is_move_only_T, size_T, alignment_T>::release() -> void
{
    m_erase_mechanism.release(m_allocator, m_storage);
}

// ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
// NOLINTNEXTLINE(*-noexcept-swap)
template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
auto swap(
    Polymorphic<T, is_move_only_T, size_T, alignment_T>& lhs,
    Polymorphic<T, is_move_only_T, size_T, alignment_T>& rhs
) -> void
{
    lhs.swap(rhs);
}

}   // namespace kiln::util
