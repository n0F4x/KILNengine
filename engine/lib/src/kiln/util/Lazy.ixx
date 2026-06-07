module;

#include <concepts>
#include <functional>
#include <type_traits>

export module kiln.util.Lazy;

import kiln.util.concepts.naked;

namespace kiln::util {

export template <naked_c F>
    requires std::invocable<F&&> && (!std::is_void_v<std::invoke_result_t<F &&>>)
class Lazy {
public:
    constexpr explicit Lazy(F&& func) : m_func{ std::move(func) } {}

    [[nodiscard]]
    explicit(false)   //
        operator std::invoke_result_t<F&&>() && noexcept(std::is_nothrow_invocable_v<F&&>)
    {
        return std::invoke(std::move(m_func));
    }

private:
    F m_func;
};

}   // namespace kiln::util
