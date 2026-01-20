module;

#include <concepts>

export module kiln.util.concepts.nothrow_constructible_from;

namespace kiln::util {

export template <typename T, typename... Args>
concept nothrow_constructible_from_c = std::destructible<T>
                                    && std::is_nothrow_constructible_v<T, Args...>;

}   // namespace kiln::util
