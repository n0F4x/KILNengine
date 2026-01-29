#pragma once

#include <functional>
#include <tuple>

#include "kiln/util/TypeList.hpp"

namespace kiln::util {

template <typename... Ts, typename F>
auto transform(TypeList<Ts...>, F&& func) -> auto
{
    return std::forward_as_tuple(func.template operator()<Ts>()...);
}

template <typename... Ts, typename F, typename Projection_T>
auto transform(TypeList<Ts...>, F&& func, Projection_T&& project) -> auto
{
    return std::forward_as_tuple(std::invoke(func, project.template operator()<Ts>())...);
}

}   // namespace kiln::util
