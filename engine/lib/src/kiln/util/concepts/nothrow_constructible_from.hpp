#pragma once

#include <concepts>

namespace kiln::util {

template <typename T, typename... Args_T>
concept nothrow_constructible_from_c = std::destructible<T>
                                    && std::is_nothrow_constructible_v<T, Args_T...>;

}   // namespace kiln::util
