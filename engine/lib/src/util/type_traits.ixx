module;

#include <type_traits>
#include <utility>

export module kiln.util.type_traits;

namespace kiln::util {

export template <typename>
struct always_true {
    constexpr static bool value = true;
};

export template <typename T, typename Like_T>
using const_like_t = std::
    conditional_t<std::is_const_v<Like_T>, std::add_const_t<T>, std::remove_const_t<T>>;

export template <typename T, typename Like_T>
using forward_like_t = decltype(std::forward_like<Like_T>(std::declval<T>()));

}   // namespace kiln::util
