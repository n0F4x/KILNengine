module;

#include <cstddef>

export module kiln.util.containers.MoveOnlyFunction;

import kiln.util.containers.Function;
import kiln.util.concepts.function;

namespace kiln::util {

export template <
    function_c  Signature_T,
    std::size_t size_T      = 3 * sizeof(void*),
    std::size_t alignment_T = sizeof(void*)>
using MoveOnlyFunction = Function<Signature_T, true, size_T, alignment_T>;

}   // namespace kiln::util
