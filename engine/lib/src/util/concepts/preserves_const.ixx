module;

#include <type_traits>

export module kiln.util.concepts.preserves_const;

namespace kiln::util {

export template <typename To, typename From>
concept preserves_const_c = !(std::is_const_v<From> && not std::is_const_v<To>);

}   // namespace kiln::util
