module;

#include <cstddef>

export module kiln.util.containers.MoveOnlyAny;

import kiln.util.containers.Any;

namespace kiln::util {

export template <
    std::size_t size_T      = 3 * sizeof(void*),
    std::size_t alignment_T = sizeof(void*)>
using BasicMoveOnlyAny = BasicAny<DefaultAnyTraits<true, size_T, alignment_T>>;

export using MoveOnlyAny = BasicMoveOnlyAny<>;

export template <typename T>
concept move_only_any_c = any_c<T> && (T::is_move_only());

export using util::any_cast;

}   // namespace kiln::util
