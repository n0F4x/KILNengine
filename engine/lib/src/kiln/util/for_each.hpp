#pragma once

#include <utility>

#include "kiln/util/TypeList.hpp"

namespace kiln::util {

template <typename... Ts, typename F>
auto for_each(TypeList<Ts...>, F&& func) -> F
{
    (func.template operator()<Ts>(), ...);
    return std::forward<F>(func);
}

}   // namespace kiln::util
