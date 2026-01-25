#pragma once

#include <functional>
#include <utility>

#include "kiln/util/TypeList.hpp"

namespace kiln::util {

template <typename... Ts, typename F>
auto for_each(TypeList<Ts...>, F&& func) -> F
{
    (func.template operator()<Ts>(), ...);
    return std::forward<F>(func);
}

template <typename... Ts, typename F, typename Projection_T>
auto for_each(TypeList<Ts...>, F&& func, Projection_T&& project) -> F
{
    (std::invoke(func, project.template operator()<Ts>()), ...);
    return std::forward<F>(func);
}

}   // namespace kiln::util
