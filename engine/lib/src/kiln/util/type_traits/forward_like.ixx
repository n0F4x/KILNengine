module;

#include <utility>

export module kiln.util.type_traits.forward_like;

namespace kiln::util {

export template <typename T, typename Like_T>
using forward_like_t = decltype(std::forward_like<Like_T>(std::declval<T>()));

}   // namespace kiln::util
