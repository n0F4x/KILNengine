module;

#include <concepts>
#include <functional>
#include <utility>

export module kiln.util.containers.FunctionWrapper;

import kiln.util.concepts.naked;
import kiln.util.concepts.function;
import kiln.util.concepts.member_function_pointer;
import kiln.util.concepts.strips_to;
import kiln.util.concepts.type_list;
import kiln.util.concepts.unambiguous_functor;
import kiln.util.concepts.unambiguously_invocable;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.forward_like;
import kiln.util.type_traits.result_of;

namespace kiln::util {

namespace internal {

template <typename FunctionWrapper_T, typename Result_T, type_list_c ArgList_T>
class FunctionInterface;

#define NOEXCEPT_RETURN(...)        \
    noexcept(noexcept(__VA_ARGS__)) \
    {                               \
        return __VA_ARGS__;         \
    }

template <
    typename FunctionWrapper_T,
    typename Result_T,
    template <typename...> typename TypeList_T,
    typename... Args_T>
class FunctionInterface<FunctionWrapper_T, Result_T, TypeList_T<Args_T...>> {
public:
    template <typename Self_T>
    constexpr Result_T operator()(this Self_T&& self, Args_T... args) NOEXCEPT_RETURN(
        std::invoke(
            std::forward_like<Self_T>(self.FunctionWrapper_T::m_invocable),
            std::forward<Args_T>(args)...
        )
    )
};

}   // namespace internal

export template <naked_c Invocable_T>
class FunctionWrapper;

export template <function_c Invocable_T>
    requires naked_c<Invocable_T>
class FunctionWrapper<Invocable_T> : public internal::FunctionInterface<
                                         FunctionWrapper<Invocable_T>,
                                         result_of_t<Invocable_T>,
                                         arguments_of_t<Invocable_T>>   //
{
public:
    constexpr explicit FunctionWrapper(Invocable_T& invocable) : m_invocable{ invocable }
    {
    }

private:
    template <typename FunctionWrapper_T, typename Result_T, type_list_c ArgList_T>
    friend class internal::FunctionInterface;

    std::reference_wrapper<Invocable_T> m_invocable;
};

export template <member_function_pointer_c Invocable_T>
    requires naked_c<Invocable_T>
class FunctionWrapper<Invocable_T> : public internal::FunctionInterface<
                                         FunctionWrapper<Invocable_T>,
                                         result_of_t<Invocable_T>,
                                         arguments_of_t<Invocable_T>>   //
{
public:
    constexpr explicit FunctionWrapper(Invocable_T invocable) : m_invocable{ invocable }
    {
    }

private:
    template <typename FunctionWrapper_T, typename Result_T, type_list_c ArgList_T>
    friend class internal::FunctionInterface;

    Invocable_T m_invocable;
};

export template <typename Class_T, typename Result_T>
class FunctionWrapper<Result_T(Class_T::*)> {
    using MemberObjectPtr = Result_T(Class_T::*);

public:
    constexpr explicit FunctionWrapper(const MemberObjectPtr member_object_pointer)
        : m_member_object_pointer{ member_object_pointer }
    {
    }

    template <strips_to_c<Class_T> T>
    [[nodiscard]]
    constexpr auto operator()(T&& value) noexcept -> forward_like_t<Result_T, T>
    {
        return std::forward_like<T>(m_member_object_pointer(value));
    }

private:
    MemberObjectPtr m_member_object_pointer;
};

export template <unambiguous_functor_c Invocable_T>
    requires util::naked_c<Invocable_T>
class FunctionWrapper<Invocable_T> : public internal::FunctionInterface<
                                         FunctionWrapper<Invocable_T>,
                                         result_of_t<Invocable_T>,
                                         arguments_of_t<Invocable_T>>   //
{
public:
    FunctionWrapper()
        requires std::default_initializable<Invocable_T>
    = default;

    template <typename UInvocable_T>
        requires std::constructible_from<Invocable_T, UInvocable_T&&>
    constexpr explicit FunctionWrapper(UInvocable_T&& invocable)
        : m_invocable{ std::forward<UInvocable_T>(invocable) }
    {
    }

private:
    template <typename FunctionWrapper_T, typename Result_T, type_list_c ArgList_T>
    friend class internal::FunctionInterface;

    Invocable_T m_invocable;
};

}   // namespace kiln::util
