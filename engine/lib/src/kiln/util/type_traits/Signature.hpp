#pragma once

#include <type_traits>

#include "kiln/util/concepts/naked.hpp"
#include "kiln/util/TypeList.hpp"

namespace kiln::util {

template <typename>
struct Signature;

template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...)> {
    using type        = Result_T(Args_T...);
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    template <typename T>
    constexpr static bool mimics_qualifiers = true;
    template <naked_c T>
    using mimic_t = T;
};

template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) noexcept> {
    using type        = Result_T(Args_T...) noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, true>  is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    template <typename T>
    constexpr static bool mimics_qualifiers = true;
    template <naked_c T>
    using mimic_t = T;
};

template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) &> {
    using type        = Result_T(Args_T...) &;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, true>  has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    template <typename T>
    constexpr static bool mimics_qualifiers = std::is_lvalue_reference_v<T>;
    template <naked_c T>
    using mimic_t = T&;
};

template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) & noexcept> {
    using type        = Result_T(Args_T...) & noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, true>  is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, true>  has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    template <typename T>
    constexpr static bool mimics_qualifiers = std::is_lvalue_reference_v<T>;
    template <naked_c T>
    using mimic_t = T&;
};

template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) &&> {
    using type        = Result_T(Args_T...) &&;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, true>  has_rvalue_ref;

    template <typename T>
    constexpr static bool mimics_qualifiers = std::is_rvalue_reference_v<T>;
    template <naked_c T>
    using mimic_t = T&&;
};

template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) && noexcept> {
    using type        = Result_T(Args_T...) && noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, true>  has_rvalue_ref;

    template <typename T>
    constexpr static bool mimics_qualifiers = std::is_rvalue_reference_v<T>;
    template <naked_c T>
    using mimic_t = T&&;
};

template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) const> {
    using type        = Result_T(Args_T...) const;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, true>  has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    template <typename T>
    constexpr static bool mimics_qualifiers = std::is_const_v<std::remove_reference_t<T>>;
    template <naked_c T>
    using mimic_t = const T;
};

template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) const noexcept> {
    using type        = Result_T(Args_T...) const noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, true>  has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    template <typename T>
    constexpr static bool mimics_qualifiers = std::is_const_v<std::remove_reference_t<T>>;
    template <naked_c T>
    using mimic_t = const T;
};

template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) const&> {
    using type        = Result_T(Args_T...) const&;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, true>  has_const;
    constexpr static std::integral_constant<bool, true>  has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    template <typename T>
    constexpr static bool mimics_qualifiers = std::is_const_v<std::remove_reference_t<T>>
                                           && std::is_lvalue_reference_v<T>;
    template <naked_c T>
    using mimic_t = const T&;
};

template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) const & noexcept> {
    using type        = Result_T(Args_T...) const& noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, true>  is_noexcept;
    constexpr static std::integral_constant<bool, true>  has_const;
    constexpr static std::integral_constant<bool, true>  has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    template <typename T>
    constexpr static bool mimics_qualifiers = std::is_const_v<std::remove_reference_t<T>>
                                           && std::is_lvalue_reference_v<T>;
    template <naked_c T>
    using mimic_t = const T&;
};

template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) const&&> {
    using type        = Result_T(Args_T...) const&&;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, true>  has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, true>  has_rvalue_ref;

    template <typename T>
    constexpr static bool mimics_qualifiers = std::is_const_v<std::remove_reference_t<T>>
                                           && std::is_rvalue_reference_v<T>;
    template <naked_c T>
    using mimic_t = const T&&;
};

template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) const && noexcept> {
    using type        = Result_T(Args_T...) const&& noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, true>  is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, true>  has_lvalue_ref;
    constexpr static std::integral_constant<bool, true>  has_rvalue_ref;

    template <typename T>
    constexpr static bool mimics_qualifiers = std::is_const_v<std::remove_reference_t<T>>
                                           && std::is_rvalue_reference_v<T>;
    template <naked_c T>
    using mimic_t = const T&&;
};

template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...)> {
    using type        = Result_T (Class_T::*)(Args_T...);
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = Class_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    using without_class_t = Result_T(Args_T...);
};

template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) noexcept> {
    using type        = Result_T (Class_T::*)(Args_T...) noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = Class_T;
    constexpr static std::integral_constant<bool, true>  is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    using without_class_t = Result_T(Args_T...) noexcept;
};

template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) &> {
    using type        = Result_T (Class_T::*)(Args_T...) &;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = Class_T&;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, true>  has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    using without_class_t = Result_T(Args_T...) &;
};

template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) & noexcept> {
    using type        = Result_T (Class_T::*)(Args_T...) & noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = Class_T&;
    constexpr static std::integral_constant<bool, true>  is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, true>  has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    using without_class_t = Result_T(Args_T...) & noexcept;
};

template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) &&> {
    using type        = Result_T (Class_T::*)(Args_T...) &&;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = Class_T&&;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, true>  has_rvalue_ref;

    using without_class_t = Result_T(Args_T...) &&;
};

template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) && noexcept> {
    using type        = Result_T (Class_T::*)(Args_T...) && noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = Class_T&&;
    constexpr static std::integral_constant<bool, true>  is_noexcept;
    constexpr static std::integral_constant<bool, false> has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, true>  has_rvalue_ref;

    using without_class_t = Result_T(Args_T...) && noexcept;
};

template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) const> {
    using type        = Result_T (Class_T::*)(Args_T...) const;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = const Class_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, true>  has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    using without_class_t = Result_T(Args_T...) const;
};

template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) const noexcept> {
    using type        = Result_T (Class_T::*)(Args_T...) const noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = const Class_T;
    constexpr static std::integral_constant<bool, true>  is_noexcept;
    constexpr static std::integral_constant<bool, true>  has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    using without_class_t = Result_T(Args_T...) const noexcept;
};

template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) const&> {
    using type        = Result_T (Class_T::*)(Args_T...) const&;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = const Class_T&;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, true>  has_const;
    constexpr static std::integral_constant<bool, true>  has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    using without_class_t = Result_T(Args_T...) const&;
};

template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) const & noexcept> {
    using type        = Result_T (Class_T::*)(Args_T...) const& noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = const Class_T&;
    constexpr static std::integral_constant<bool, true>  is_noexcept;
    constexpr static std::integral_constant<bool, true>  has_const;
    constexpr static std::integral_constant<bool, true>  has_lvalue_ref;
    constexpr static std::integral_constant<bool, false> has_rvalue_ref;

    using without_class_t = Result_T(Args_T...) const& noexcept;
};

template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) const&&> {
    using type        = Result_T (Class_T::*)(Args_T...) const&&;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = const Class_T&&;
    constexpr static std::integral_constant<bool, false> is_noexcept;
    constexpr static std::integral_constant<bool, true>  has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, true>  has_rvalue_ref;

    using without_class_t = Result_T(Args_T...) const&&;
};

template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) const && noexcept> {
    using type        = Result_T (Class_T::*)(Args_T...) const&& noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = const Class_T&&;
    constexpr static std::integral_constant<bool, true>  is_noexcept;
    constexpr static std::integral_constant<bool, true>  has_const;
    constexpr static std::integral_constant<bool, false> has_lvalue_ref;
    constexpr static std::integral_constant<bool, true>  has_rvalue_ref;

    using without_class_t = Result_T(Args_T...) const&& noexcept;
};

}   // namespace kiln::util
