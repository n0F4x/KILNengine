module;

#include <functional>
#include <utility>

export module kiln.util.algorithms.for_each;

import kiln.util.TypeList;

namespace kiln::util {

export template <typename... Ts, typename F>
auto for_each(TypeList<Ts...>, F&& func) -> F
{
    (func.template operator()<Ts>(), ...);
    return std::forward<F>(func);
}

export template <typename... Ts, typename F, typename Projection_T>
auto for_each(TypeList<Ts...>, F&& func, Projection_T&& project) -> F
{
    (std::invoke(func, project.template operator()<Ts>()), ...);
    return std::forward<F>(func);
}

}   // namespace kiln::util
