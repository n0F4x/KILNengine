module;

#include <functional>
#include <tuple>

export module kiln.util.algorithms.transform;

import kiln.util.TypeList;

namespace kiln::util {

export template <typename... Ts, typename F>
auto transform(TypeList<Ts...>, F&& func) -> auto
{
    return std::forward_as_tuple(func.template operator()<Ts>()...);
}

export template <typename... Ts, typename F, typename Projection_T>
auto transform(TypeList<Ts...>, F&& func, Projection_T&& project) -> auto
{
    return std::forward_as_tuple(std::invoke(func, project.template operator()<Ts>())...);
}

}   // namespace kiln::util
