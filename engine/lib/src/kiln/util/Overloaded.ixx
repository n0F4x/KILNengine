module;

#include <concepts>
#include <type_traits>
#include <utility>

export module kiln.util.Overloaded;

import kiln.util.FunctionWrapper;

namespace kiln::util {

export template <typename... Ts>
class Overloaded : public FunctionWrapper<Ts>... {
public:
    template <typename... Us>
        requires(std::constructible_from<FunctionWrapper<Ts>, Us &&> && ...)
    constexpr explicit Overloaded(Us&&... funcs)
        : FunctionWrapper<Ts>{ std::forward<Us>(funcs) }...
    {
    }

    using FunctionWrapper<Ts>::operator()...;

    consteval static auto operator()(auto...) = delete ("unsupported type");
};

export template <typename... Ts>
Overloaded(Ts&&...) -> Overloaded<std::remove_pointer_t<std::decay_t<Ts>>...>;

}   // namespace kiln::util
