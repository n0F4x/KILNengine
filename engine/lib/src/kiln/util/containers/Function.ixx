module;

#include <cstddef>
#include <functional>
#include <memory>
#include <memory_resource>
#include <type_traits>
#include <utility>

export module kiln.util.containers.Function;

import kiln.util.concepts.decayed;
import kiln.util.concepts.function;
import kiln.util.concepts.specialization_of;
import kiln.util.containers.Polymorphic;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.is_noexcept;
import kiln.util.type_traits.result_of;
import kiln.util.type_traits.Signature;

namespace kiln::util {

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    typename ArgumentList_T>
class FunctionImpl;

export template <
    function_c  Signature_T,
    bool        is_move_only_T = false,
    std::size_t size_T         = 3 * sizeof(void*),
    std::size_t alignment_T    = sizeof(void*)>
using Function =
    FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, arguments_of_t<Signature_T>>;

}   // namespace kiln::util

namespace kiln::util {

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> typename ArgumentList_T,
    typename... FArgs_T>
class FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>> {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;

    consteval static auto is_move_only() -> bool;
    consteval static auto size() -> std::size_t;
    consteval static auto alignment() -> std::size_t;
    template <typename T>
    consteval static auto storable() -> bool;

    FunctionImpl(const FunctionImpl&, const allocator_type& allocator)
        requires(!is_move_only());
    FunctionImpl(FunctionImpl&&, const allocator_type& allocator);


    template <typename F>
    explicit FunctionImpl(F&& func)
        requires(
            !std::is_same_v<std::remove_cvref_t<F>, FunctionImpl>
            && !util::specialization_of_c<F, std::in_place_type_t>
            && std::is_constructible_v<std::decay_t<F>, F &&>
            && storable<std::decay_t<F>>()
        );
    template <typename F>
    explicit FunctionImpl(std::allocator_arg_t, const allocator_type& allocator, F&& func)
        requires(
            !std::is_same_v<std::remove_cvref_t<F>, FunctionImpl>
            && !util::specialization_of_c<F, std::in_place_type_t>
            && std::is_constructible_v<std::decay_t<F>, F &&>
            && storable<std::decay_t<F>>()
        );

    template <decayed_c F, typename... Args_T>
    explicit FunctionImpl(std::in_place_type_t<F>, Args_T&&... args)
        requires(std::is_constructible_v<F, Args_T && ...> && storable<F>());
    template <decayed_c F, typename... Args_T>
    explicit FunctionImpl(
        std::allocator_arg_t,
        const allocator_type& allocator,
        std::in_place_type_t<F>,
        Args_T&&... args
    )
        requires(std::is_constructible_v<F, Args_T && ...> && storable<F>());


    auto operator()(
        this Signature<Signature_T>::template mimic_t<FunctionImpl>,
        FArgs_T... args
    ) noexcept(is_noexcept_v<Signature_T>) -> result_of_t<Signature_T>;


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

private:
    class Interface;
    using polymorphic_type = Polymorphic<Interface, is_move_only_T, size_T, alignment_T>;
    template <typename T>
    class Impl;

    polymorphic_type m_function;
};

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> typename ArgumentList_T,
    typename... FArgs_T>
class FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
    Interface {
public:
    virtual ~Interface() = default;

    // Implementations are expected to use a const_cast
    virtual auto operator()(FArgs_T...) const noexcept(is_noexcept_v<Signature_T>)
        -> result_of_t<Signature_T> = 0;
};

template <typename T>
struct UsesAllocatorBase;

template <typename T>
    requires(std::uses_allocator_v<T, std::pmr::polymorphic_allocator<>>)
struct UsesAllocatorBase<T> {
    using allocator_type = T::allocator_type;
};

template <typename T>
    requires(!std::uses_allocator_v<T, std::pmr::polymorphic_allocator<>>)
struct UsesAllocatorBase<T> {};

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> typename ArgumentList_T,
    typename... FArgs_T>
template <typename F>
class FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
    Impl : public UsesAllocatorBase<F>,
           public Interface   //
{
public:
    Impl(const Impl&) = default;

    Impl(const Impl& other, const allocator_type& allocator)
        requires(
            !is_move_only() && std::uses_allocator_v<F, std::pmr::polymorphic_allocator<>>
        )
        : m_impl{ other.m_impl, allocator }
    {
    }

    Impl(Impl&&) = default;

    Impl(Impl&& other, const allocator_type& allocator)
        requires(std::uses_allocator_v<F, std::pmr::polymorphic_allocator<>>)
        : m_impl{ std::move(other.m_impl), allocator }
    {
    }

    template <typename... Args_T>
    constexpr explicit Impl(std::in_place_t, Args_T&&... args)
        : m_impl(std::forward<Args_T>(args)...)
    {
    }

    template <typename... Args_T>
    explicit Impl(
        std::allocator_arg_t,
        const allocator_type& allocator,
        std::in_place_t,
        Args_T&&... args
    )
        requires(std::uses_allocator_v<F, std::pmr::polymorphic_allocator<>>)
        : m_impl{
              std::make_obj_using_allocator<F>(allocator, std::forward<Args_T>(args)...)
          }
    {
    }

    auto operator=(const Impl& other) -> Impl&
        requires(!is_move_only_T)
    {
        /*
         * Workaround for lambdas without a copy assignment operator
         */
        if constexpr (std::is_copy_assignable_v<F>)
        {
            m_impl = other.m_impl;
            return *this;
        }
        else
        {
            std::destroy_at(&m_impl);
            std::construct_at(&m_impl, other.m_impl);
            return *this;
        }
    }

    auto operator=(Impl&&) -> Impl& = default;

    // Implementations are expected to use a const_cast
    auto operator()(FArgs_T... args) const noexcept(is_noexcept_v<Signature_T>)
        -> result_of_t<Signature_T> override
    {
        using Mimicked = Signature<Signature_T>::template mimic_t<F>&&;

        return std::invoke(const_cast<Mimicked>(m_impl), std::forward<FArgs_T>(args)...);
    }

private:
    F m_impl;
};

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> class ArgumentList_T,
    typename... FArgs_T>
consteval auto
    FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
        is_move_only() -> bool
{
    return polymorphic_type::is_move_only();
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> class ArgumentList_T,
    typename... FArgs_T>
consteval auto
    FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
        size() -> std::size_t
{
    return polymorphic_type::size();
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> class ArgumentList_T,
    typename... FArgs_T>
consteval auto
    FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
        alignment() -> std::size_t
{
    return polymorphic_type::alignment();
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> class ArgumentList_T,
    typename... FArgs_T>
template <typename F>
consteval auto
    FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
        storable() -> bool
{
    using Result   = result_of_t<Signature_T>;
    using Mimicked = Signature<Signature_T>::template mimic_t<std::decay_t<F>>;

    return polymorphic_type::template storable<Impl<F>>()
        && (is_noexcept_v<Signature_T>
                ? std::is_nothrow_invocable_r_v<Result, Mimicked, FArgs_T...>
                : std::is_invocable_r_v<Result, Mimicked, FArgs_T...>);
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> class ArgumentList_T,
    typename... FArgs_T>
FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
    FunctionImpl(const FunctionImpl& other, const allocator_type& allocator)
    requires(!is_move_only())
    : m_function{ other.m_function, allocator }
{
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> class ArgumentList_T,
    typename... FArgs_T>
FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
    FunctionImpl(FunctionImpl&& other, const allocator_type& allocator)
    : m_function{ std::move(other.m_function), allocator }
{
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> class ArgumentList_T,
    typename... FArgs_T>
template <typename F>
FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
    FunctionImpl(F&& func)
    requires(
        !std::is_same_v<std::remove_cvref_t<F>, FunctionImpl>
        && !util::specialization_of_c<F, std::in_place_type_t>
        && std::is_constructible_v<std::decay_t<F>, F &&> && storable<std::decay_t<F>>()
    )
    : FunctionImpl{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          std::forward<F>(func),
      }
{
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> class ArgumentList_T,
    typename... FArgs_T>
template <typename F>
FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
    FunctionImpl(std::allocator_arg_t, const allocator_type& allocator, F&& func)
    requires(
        !std::is_same_v<std::remove_cvref_t<F>, FunctionImpl>
        && !util::specialization_of_c<F, std::in_place_type_t>
        && std::is_constructible_v<std::decay_t<F>, F &&> && storable<std::decay_t<F>>()
    )
    : FunctionImpl{
          std::allocator_arg,
          allocator,
          std::in_place_type<std::decay_t<F>>,
          std::forward<F>(func),
      }
{
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> class ArgumentList_T,
    typename... FArgs_T>
template <decayed_c F, typename... Args_T>
FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
    FunctionImpl(std::in_place_type_t<F> in_place_type, Args_T&&... args)
    requires(std::is_constructible_v<F, Args_T && ...> && storable<F>())
    : FunctionImpl{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          in_place_type,
          std::forward<Args_T>(args)...,
      }
{
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> class ArgumentList_T,
    typename... FArgs_T>
template <decayed_c F, typename... Args_T>
FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
    FunctionImpl(
        std::allocator_arg_t,
        const allocator_type& allocator,
        std::in_place_type_t<F>,
        Args_T&&... args
    )
    requires(std::is_constructible_v<F, Args_T && ...> && storable<F>())
    : m_function{
          std::allocator_arg,
          allocator,
          std::in_place_type<Impl<F>>,
          std::in_place,
          std::forward<Args_T>(args)...,
      }
{
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> class ArgumentList_T,
    typename... FArgs_T>
auto
    FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
        operator()(
            this typename Signature<Signature_T>::template mimic_t<FunctionImpl> self,
            FArgs_T... args
        ) noexcept(is_noexcept_v<Signature_T>) -> result_of_t<Signature_T>
{
    return self
        .template FunctionImpl<
            is_move_only_T,
            size_T,
            alignment_T,
            Signature_T,
            ArgumentList_T<FArgs_T...>>::m_function
        ->operator()(std::forward<FArgs_T>(args)...);
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> class ArgumentList_T,
    typename... FArgs_T>
auto
    FunctionImpl<is_move_only_T, size_T, alignment_T, Signature_T, ArgumentList_T<FArgs_T...>>::
        get_allocator() const noexcept -> allocator_type
{
    return m_function.get_allocator();
}

}   // namespace kiln::util
