module;

#include <functional>

export module kiln.util.algorithms.any_of;

import kiln.util.TypeList;

namespace kiln::util {

export template <typename... Ts, typename F>
auto any_of(TypeList<Ts...>, F&& func) -> bool
{
    return (func.template operator()<Ts>() || ...);
}

export template <typename... Ts, typename F, typename Projection_T>
auto any_of(TypeList<Ts...>, F&& func, Projection_T&& project) -> bool
{
    return (std::invoke(func, project.template operator()<Ts>()) || ...);
}

}   // namespace kiln::util
