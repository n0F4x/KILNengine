module;

#include <cstddef>

export module kiln.util.containers.CopyableAny;

import kiln.util.containers.Any;

namespace kiln::util {

export template <
    std::size_t size_T      = 3 * sizeof(void*),
    std::size_t alignment_T = sizeof(void*)>
using BasicCopyableAny = BasicAny<DefaultAnyTraits<false, size_T, alignment_T>>;

export using CopyableAny = BasicCopyableAny<>;

export template <typename T>
concept copyable_any_c = any_c<T> && (!T::is_move_only());

}   // namespace kiln::util
