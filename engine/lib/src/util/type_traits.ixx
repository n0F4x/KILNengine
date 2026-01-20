module;

#include <utility>

export module kiln.util.type_traits;

namespace kiln::util {

export template <typename>
struct always_true {
    constexpr static bool value = true;
};

export template <typename T, typename Like_T>
using forward_like_t = decltype(std::forward_like<Like_T>(std::declval<T>()));

}   // namespace kiln::util
