module;

#include <type_traits>

export module kiln.util.concepts.storable;

namespace kiln::util {

export template <typename T>
concept storable_c = std::is_nothrow_destructible_v<T>;

}   // namespace kiln::util
