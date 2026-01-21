#pragma once

#include <concepts>

namespace kiln::util {

template <typename T, typename... Args>
concept nothrow_constructible_from_c = std::destructible<T>
                                    && std::is_nothrow_constructible_v<T, Args...>;

}   // namespace kiln::util
